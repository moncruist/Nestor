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

using namespace std;

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
        "SELECT * FROM `users` WHERE `username` = ?;"
        //--------------------------------------------------------
};

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
        sqlite3_prepare_v2(connection_->handle(), SQL_STATEMENTS[i], -1, &compiledStatements_[i], NULL);
    }
}


void SqliteProvider::createUsersTable() {
    lock_guard<recursive_mutex> locker(lock_);
    if (compiledStatements_[STATEMENT_CREATE_USER_TABLE] == nullptr)
        prepareStatements();
    sqlite3_stmt *stmt = compiledStatements_[STATEMENT_CREATE_USER_TABLE];
    sqlite3_reset(stmt);
    int ret = sqlite3_step(stmt);
    if (ret == SQLITE_BUSY || ret == SQLITE_ERROR) {
        ostringstream oss;
        oss << "SqliteProvider::findUserByName: error while executing SQL query: " << sqlite3_errmsg(connection_->handle());
        SERVICE_LOG_LVL(ERROR, oss.str());
        throw SqliteProviderException(oss.str());
    }

    if (ret != SQLITE_DONE) {
        /* We expect SQLITE_ROW, but get SQLITE_DONE. Something strange happened. */
        string str = "SqliteProvider::findUserByName: unexpected return code";
        SERVICE_LOG_LVL(ERROR, str);
        throw SqliteProviderException(str);
    }
}


User *SqliteProvider::findUserByName(const string& username) {
    lock_guard<recursive_mutex> locker(lock_);
    if (compiledStatements_[STATEMENT_FIND_USER_BY_USERNAME] == nullptr)
        prepareStatements();
    sqlite3_stmt *stmt = compiledStatements_[STATEMENT_FIND_USER_BY_USERNAME];
    sqlite3_reset(stmt);
    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);
    int ret = sqlite3_step(stmt);
    if (ret == SQLITE_BUSY || ret == SQLITE_ERROR) {
        ostringstream oss;
        oss << "SqliteProvider::findUserByName: error while executing SQL query: " << sqlite3_errmsg(connection_->handle());
        SERVICE_LOG_LVL(ERROR, oss.str());
        throw SqliteProviderException(oss.str());
    }

    if (ret == SQLITE_DONE) {
        /* 0 rows was found */
        return nullptr;
    }

    User *usr = new User();
    usr->id = sqlite3_column_int(stmt, 0);
    usr->username = string(reinterpret_cast<const char *>(sqlite3_column_text(stmt, 1)));
    usr->password = string(reinterpret_cast<const char *>(sqlite3_column_text(stmt, 2)));

    while (ret != SQLITE_DONE) {
         ret = sqlite3_step(stmt);
    }

    return usr;
}

} /* namespace service */
} /* namespace nestor */


