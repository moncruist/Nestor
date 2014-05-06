/*
 *  This file is part of Nestor.
 *
 *  Nestor - program for aggregation RSS subscriptions providing
 *  access via IMAP interface.
 *  Copyright (C) 2013-2014  Konstantin Zhukov
 *
 *  Nestor is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  Nestor is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see {http://www.gnu.org/licenses/}.
 */
#include <string>
#include <cstring>
#include <sstream>
#include "logger.h"
#include "sqlite_provider.h"
#include "utils/timestamp.h"

using namespace std;
using namespace nestor::utils;

namespace nestor {
namespace service {

const char *SqliteProvider::SQL_STATEMENTS[STATEMENTS_LENGTH] = {
        // --------- STATEMENT_CREATE_USER_TABLE------------------
        "CREATE TABLE IF NOT EXISTS `users`("
        "`user_id` INTEGER PRIMARY KEY ASC AUTOINCREMENT NOT NULL,"
        "`username` VARCHAR(255) NOT NULL,"
        "`password` VARCHAR(255) NOT NULL);\n"
        "CREATE INDEX IF NOT EXISTS `users_username_idx` on `users`"
        "(`username`);",
        //--------------------------------------------------------

        // --------- STATEMENT_FIND_USER_BY_USERNAME--------------
        "SELECT * FROM `users` WHERE `username` = ?;",
        //--------------------------------------------------------

        // --------- STATEMENT_FIND_USER_BY_ID--------------------
        "SELECT * FROM `users` WHERE `user_id` = ?;",
        //--------------------------------------------------------

        // --------- STATEMENT_CREATE_CHANNEL_TABLE---------------
        "CREATE TABLE IF NOT EXISTS `channels`("
        "`channel_id` INTEGER PRIMARY KEY ASC AUTOINCREMENT NOT NULL,"
        "`title` VARCHAR(200) NOT NULL,"
        "`rss_link` VARCHAR(2048) NOT NULL UNIQUE,"
        "`link` VARCHAR(2048) NOT NULL,"
        "`description` VARCHAR(400) NOT NULL,"
        "`update_interval_sec` INTEGER NOT NULL DEFAULT '3600',"
        "`last_update` TEXT);\n"
        "CREATE INDEX IF NOT EXISTS `channels_rss_link_idx` on `channels`"
        "(`rss_link`);",
        //--------------------------------------------------------

        // --------- STATEMENT_FIND_CHANNEL_BY_ID--------------------
        "SELECT * FROM `channels` WHERE `channel_id` = ?;",
        //--------------------------------------------------------
};

static const string SQLITE_DATE_FORMAT_STDLIB_SYNTAX = "%Y-%m-%d %H:%M:%S";

SqliteProvider::SqliteProvider(const SqliteConnection *connection)
        : connection_(connection) {
    if (connection == nullptr) {
        string errmsg = "SqliteProvider::ctr: Invalid argument: connection == nullptr";
        SERVICE_LOG_LVL(ERROR, errmsg);
        throw logic_error(errmsg);
    }

    if (!connection->connected()) {
        string errmsg = "SqliteProvider::ctr: Invalid argument: connection is not connected to the database";
        SERVICE_LOG_LVL(ERROR, errmsg);
        throw logic_error(errmsg);
    }

    memset(compiledStatements_, 0, sizeof(sqlite3_stmt *) * STATEMENTS_LENGTH);
}

SqliteProvider::~SqliteProvider() {
    for (int i = 0; i < STATEMENTS_LENGTH; i++) {
        if (compiledStatements_[i]) {
            sqlite3_finalize(compiledStatements_[i]);
        }
    }
    memset(compiledStatements_, 0, sizeof(sqlite3_stmt *) * STATEMENTS_LENGTH);
}

void SqliteProvider::prepareStatements() {
    lock_guard<recursive_mutex> locker(lock_);
    for (int i = 0; i < STATEMENTS_LENGTH; i++) {
        if (compiledStatements_[i] != nullptr) {
            sqlite3_prepare_v2(connection_->handle(), SQL_STATEMENTS[i], -1,
                    &compiledStatements_[i], NULL);
        }
    }
}


void SqliteProvider::createUsersTable() {
    lock_guard<recursive_mutex> locker(lock_);
    sqlite3_stmt *stmt = getStatement(STATEMENT_CREATE_USER_TABLE);
    sqlite3_reset(stmt);
    int ret = sqlite3_step(stmt);
    checkSqliteResult(ret, "SqliteProvider::createUsersTable");

    if (ret != SQLITE_DONE) {
        string str = "SqliteProvider::createUsersTable: unexpected return code";
        SERVICE_LOG_LVL(ERROR, str);
        throw SqliteProviderException(str);
    }
}


User *SqliteProvider::findUserByName(const string& username) {
    lock_guard<recursive_mutex> locker(lock_);
    sqlite3_stmt *stmt = getStatement(STATEMENT_FIND_USER_BY_USERNAME);
    sqlite3_reset(stmt);
    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);
    int ret = sqlite3_step(stmt);
    checkSqliteResult(ret, "SqliteProvider::findUserByName");

    if (ret == SQLITE_DONE) {
        /* 0 rows was found */
        return nullptr;
    }

    User *usr = new User();
    parseUserRow(stmt, *usr);

    while (ret != SQLITE_DONE) {
         ret = sqlite3_step(stmt);
    }

    return usr;
}


User* SqliteProvider::findUserById(int64_t id) {
    lock_guard<recursive_mutex> locker(lock_);
    sqlite3_stmt *stmt = getStatement(STATEMENT_FIND_USER_BY_ID);
    sqlite3_reset(stmt);
    sqlite3_bind_int64(stmt, 1, id);
    int ret = sqlite3_step(stmt);
    checkSqliteResult(ret, "SqliteProvider::findUserById");

    if (ret == SQLITE_DONE) {
        /* 0 rows was found */
        return nullptr;
    }

    User *usr = new User();
    try {
        parseUserRow(stmt, *usr);
    } catch (logic_error &e) {
        SERVICE_LOG_LVL(ERROR, e.what());
        delete usr;
        throw SqliteProviderException(e.what());
    }

    while (ret != SQLITE_DONE) {
         ret = sqlite3_step(stmt);
    }

    return usr;
}

void SqliteProvider::createChannelsTable() {
    lock_guard<recursive_mutex> locker(lock_);
    sqlite3_stmt *stmt = getStatement(STATEMENT_CREATE_CHANNEL_TABLE);
    sqlite3_reset(stmt);
    int ret = sqlite3_step(stmt);
    checkSqliteResult(ret, "SqliteProvider::createChannelsTable");

    if (ret != SQLITE_DONE) {
        string str = "SqliteProvider::createChannelsTable: unexpected return code";
        SERVICE_LOG_LVL(ERROR, str);
        throw SqliteProviderException(str);
    }
}

Channel* SqliteProvider::findChannelById(int64_t id) {
    lock_guard<recursive_mutex> locker(lock_);
    sqlite3_stmt *stmt = getStatement(STATEMENT_FIND_CHANNEL_BY_ID);
    sqlite3_reset(stmt);
    sqlite3_bind_int64(stmt, 1, id);
    int ret = sqlite3_step(stmt);
    checkSqliteResult(ret, "SqliteProvider::findChannelById");

    if (ret == SQLITE_DONE) {
        /* 0 rows was found */
        return nullptr;
    }

    Channel *channel = new Channel();
    try {
        parseChannelRow(stmt, *channel);
    } catch (logic_error &e) {
        SERVICE_LOG_LVL(ERROR, e.what());
        delete channel;
        throw SqliteProviderException(e.what());
    }

    while (ret != SQLITE_DONE) {
         ret = sqlite3_step(stmt);
    }

    return channel;
}


sqlite3_stmt* SqliteProvider::getStatement(int statementCode) {
    lock_guard<recursive_mutex> locker(lock_);
    if (statementCode < 0 || statementCode >= STATEMENTS_LENGTH)
        return nullptr;

    if (compiledStatements_[statementCode] == nullptr) {
        sqlite3_prepare_v2(connection_->handle(), SQL_STATEMENTS[statementCode],
                -1, &compiledStatements_[statementCode], NULL);
    }
    return compiledStatements_[statementCode];
}

void SqliteProvider::checkSqliteResult(int retCode, const std::string& tag) {
    if (retCode == SQLITE_BUSY || retCode == SQLITE_ERROR) {
        ostringstream oss;
        oss << tag << ": error while executing SQL query: " << sqlite3_errmsg(connection_->handle());
        SERVICE_LOG_LVL(ERROR, oss.str());
        throw SqliteProviderException(oss.str());
    }
}



void SqliteProvider::parseUserRow(sqlite3_stmt* stmt, User& out) {

    if (!stmt)
        throw logic_error("SqliteProvider::parseUserRow: invalid argument `stmt`");
    int columns = sqlite3_column_count(stmt);
    if (columns != 3) {
        ostringstream oss;
        oss << "SqliteProvider::parseUserRow: invalid column count in result set. Expected: 3. Actual: " << columns;
        throw logic_error(oss.str());
    }

    if (sqlite3_column_type(stmt, 0) == SQLITE_INTEGER)
        out.setId(sqlite3_column_int(stmt, 0));
    else
        throw logic_error("SqliteProvider::parseUserRow: invalid column 0 type");

    if (sqlite3_column_type(stmt, 1) == SQLITE_TEXT)
        out.setUsername(string(reinterpret_cast<const char *>(sqlite3_column_text(stmt, 1))));
    else
        throw logic_error("SqliteProvider::parseUserRow: invalid column 1 type");

    if (sqlite3_column_type(stmt, 2) == SQLITE_TEXT)
        out.setPassword(string(reinterpret_cast<const char *>(sqlite3_column_text(stmt, 2))));
    else
        throw logic_error("SqliteProvider::parseUserRow: invalid column 2 type");
}


void SqliteProvider::parseChannelRow(sqlite3_stmt* stmt, Channel& out) {
    if (!stmt)
        throw logic_error("SqliteProvider::parseChannelRow: invalid argument `stmt`");
    int columns = sqlite3_column_count(stmt);
    if (columns != 7) {
        ostringstream oss;
        oss << "SqliteProvider::parseChannelRow: invalid column count in result set. Expected: 7. Actual: " << columns;
        throw logic_error(oss.str());
    }

    if (sqlite3_column_type(stmt, 0) == SQLITE_INTEGER)
        out.setId(sqlite3_column_int(stmt, 0));
    else
        throw logic_error("SqliteProvider::parseChannelRow: invalid column 0 type");

    if (sqlite3_column_type(stmt, 1) == SQLITE_TEXT)
        out.setTitle(string(reinterpret_cast<const char *>(sqlite3_column_text(stmt, 1))));
    else
        throw logic_error("SqliteProvider::parseChannelRow: invalid column 1 type");

    if (sqlite3_column_type(stmt, 2) == SQLITE_TEXT)
        out.setRssLink(string(reinterpret_cast<const char *>(sqlite3_column_text(stmt, 2))));
    else
        throw logic_error("SqliteProvider::parseChannelRow: invalid column 2 type");

    if (sqlite3_column_type(stmt, 3) == SQLITE_TEXT)
        out.setLink(string(reinterpret_cast<const char *>(sqlite3_column_text(stmt, 3))));
    else
        throw logic_error("SqliteProvider::parseChannelRow: invalid column 3 type");

    if (sqlite3_column_type(stmt, 4) == SQLITE_TEXT)
        out.setDescription(string(reinterpret_cast<const char *>(sqlite3_column_text(stmt, 4))));
    else
        throw logic_error("SqliteProvider::parseChannelRow: invalid column 4 type");

    if (sqlite3_column_type(stmt, 5) == SQLITE_INTEGER)
        out.setUpdateInterval(sqlite3_column_int(stmt, 5));
    else
        throw logic_error("SqliteProvider::parseChannelRow: invalid column 5 type");

    if (sqlite3_column_type(stmt, 6) == SQLITE_TEXT) {
        string timestamp = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 6));
        out.setLastUpdate(timestampConvert(timestamp, SQLITE_DATE_FORMAT_STDLIB_SYNTAX));
    }
    else {
        throw logic_error("SqliteProvider::parseChannelRow: invalid column 6 type");
    }
}

} /* namespace service */
} /* namespace nestor */


