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
        "SELECT * FROM `users` WHERE `username` = :username;",
        //--------------------------------------------------------

        // --------- STATEMENT_FIND_USER_BY_ID--------------------
        "SELECT * FROM `users` WHERE `user_id` = :userid;",
        //--------------------------------------------------------

        // --------- STATEMENT_INSERT_NEW_USER--------------------
        "INSERT INTO `users`(`username`, `password`) "
        "VALUES(:username, :password);",
        //--------------------------------------------------------

        // --------- STATEMENT_UPDATE_USER------------------------
        "UPDATE `users` SET `username`=:username, `password`=:password "
        "WHERE `user_id` = :userid;",
        //--------------------------------------------------------

        // --------- STATEMENT_DELETE_USER------------------------
        "DELETE FROM `users` WHERE `user_id` = :userid;",
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

        // --------- STATEMENT_FIND_CHANNEL_BY_ID-----------------
        "SELECT * FROM `channels` WHERE `channel_id` = :channelid;",
        //--------------------------------------------------------

        // --------- STATEMENT_INSERT_NEW_CHANNEL-----------------
        "INSERT INTO `channels`(`title`, `rss_link`, `link`, `description`,"
        "`update_interval_sec`, `last_update`) "
        "VALUES(:title, :rss_link, :link, :description, :update_interval_sec,"
        ":last_update);",
        //--------------------------------------------------------

        // --------- STATEMENT_UPDATE_CHANNEL---------------------
        "UPDATE `channels` SET `title`=:title, `rss_link`=:rss_link, "
        "`link`=:link, `description`=:description, "
        "`update_interval_sec`=:update_interval_sec, `last_update`=:last_update "
        "WHERE `channel_id` = :channel_id;",
        //--------------------------------------------------------

        // --------- STATEMENT_DELETE_CHANNEL---------------------
        "DELETE FROM `channels` WHERE `channel_id` = :channel_id;",
        //--------------------------------------------------------

        // --------- STATEMENT_CREATE_FEED_TABLE-----------------
        "CREATE TABLE IF NOT EXISTS `feeds`("
        "`feed_id` INTEGER PRIMARY KEY ASC AUTOINCREMENT NOT NULL,"
        "`channel_id` INTEGER NOT NULL,"
        "`guid` TEXT NOT NULL,"
        "`title` TEXT NOT NULL,"
        "`link` TEXT NOT NULL,"
        "`description` TEXT NOT NULL,"
        "`pub_date` TEXT NOT NULL,"
        "`feed_txt` TEXT);\n"
        "CREATE INDEX IF NOT EXISTS `feeds_guid_idx` on `feeds`"
        "(`guid`);\n"
        "CREATE INDEX IF NOT EXISTS `feeds_channel_id_idx` on `feeds`"
        "(`channel_id`);\n",
        //--------------------------------------------------------

        // --------- STATEMENT_FIND_FEED_BY_ID--------------------
        "SELECT * FROM `feeds` WHERE `feed_id` = :feed_id;",
        //--------------------------------------------------------

        // --------- STATEMENT_FIND_FEED_BY_GUID------------------
        "SELECT * FROM `feeds` WHERE `guid` = :guid;",
        //--------------------------------------------------------

        // --------- STATEMENT_FIND_FEED_BY_CHANNEL---------------
        "SELECT * FROM `feeds` WHERE `channel_id` = :channel_id;",
        //--------------------------------------------------------

        // --------- STATEMENT_INSERT_NEW_FEED--------------------
        "INSERT INTO `feeds`(`channel_id`, `guid`, `title`,"
        "`link`, `description`, `pub_date`,`feed_txt`) "
        "VALUES(:channel_id, :guid, :title, :link, :description,"
        ":pub_date, :feed_txt);",
        //--------------------------------------------------------

        // --------- STATEMENT_UPDATE_FEED---------------------
        "UPDATE `feeds` SET `channel_id`=:channel_id, `guid`=:guid, "
        "`title`=:title, `link`=:link, `description`=:description,"
        "`pub_date`=:pub_date, `feed_txt`=:feed_txt "
        "WHERE `feed_id` = :feed_id;",
        //--------------------------------------------------------

        // --------- STATEMENT_DELETE_FEED---------------------
        "DELETE FROM `feeds` WHERE `feed_id` = :feed_id;",
        //--------------------------------------------------------

        // --------- STATEMENT_CREATE_USER_CHANNEL_TABLE---------------
        "CREATE TABLE IF NOT EXISTS `users_channels`("
        "`users_channels_id` INTEGER PRIMARY KEY ASC AUTOINCREMENT NOT NULL,"
        "`user_id` INTEGER NOT NULL,"
        "`channel_id` INTEGER NOT NULL UNIQUE);\n"
        "CREATE INDEX IF NOT EXISTS `users_channels_user_id_idx` on `users_channels`"
        "(`user_id`);\n"
        "CREATE INDEX IF NOT EXISTS `users_channels_channel_id_idx` on `users_channels`"
        "(`channel_id`);",
        //--------------------------------------------------------

        // --------- STATEMENT_FIND_CHANNELS_BY_USER_ID-----------
        "SELECT `ch`.* FROM `channels` AS `ch`, `user_channels` AS `usr_ch` "
        "WHERE `usr_ch`.`user_id` = :user_id AND `usr_ch`.`channel_id` = `ch`.`channel_id`;",
        //--------------------------------------------------------

        // --------- STATEMENT_FIND_USERS_BY_CHANNEL_ID-----------
        "SELECT `usr`.* FROM `users` AS `usr`, `user_channels` AS `usr_ch` "
        "WHERE `usr_ch`.`channel_id` = :channel_id AND `usr_ch`.`user_id` = `usr`.`user_id`;",
        //--------------------------------------------------------

        // --------- STATEMENT_INSERT_NEW_USER_CHANNEL-----------------
        "INSERT INTO `users_channels`(`user_id`, `channel_id`) "
        "VALUES(:user_id, :channel_id);",
        //--------------------------------------------------------

        // --------- STATEMENT_DELETE_USER_CHANNEL-----------------
        "DELETE FROM `users_channels` WHERE `user_id` = :user_id AND `channel_id` = :channel_id;",
        //--------------------------------------------------------
};

/**
 * Format in which datetime is stored in sqlite database.
 */
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
    createTableByStatement(STATEMENT_CREATE_USER_TABLE, "SqliteProvider::createUsersTable");
}


User *SqliteProvider::findUserByName(const string& username) {
    lock_guard<recursive_mutex> locker(lock_);
    sqlite3_stmt *stmt = getStatement(STATEMENT_FIND_USER_BY_USERNAME);
    sqlite3_reset(stmt);
    sqlite3_bind_text(stmt, sqlite3_bind_parameter_index(stmt, "username"),
                      username.c_str(), -1, SQLITE_STATIC);
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
    sqlite3_bind_int64(stmt, sqlite3_bind_parameter_index(stmt, "userid"), id);
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

int64_t SqliteProvider::insertUser(const User &user) {
    lock_guard<recursive_mutex> locker(lock_);
    sqlite3_stmt *stmt = getStatement(STATEMENT_INSERT_NEW_USER);
    sqlite3_reset(stmt);
    sqlite3_bind_text(stmt, sqlite3_bind_parameter_index(stmt, "username"),
                      user.username().c_str(),
                      -1, nullptr);
    sqlite3_bind_text(stmt, sqlite3_bind_parameter_index(stmt, "password"),
                      user.password().c_str(),
                      -1, nullptr);
    int ret = sqlite3_step(stmt);
    checkSqliteResult(ret, "SqliteProvider::insertUser");
    int64_t newId = sqlite3_last_insert_rowid(connection_->handle());
    return newId;
}

bool SqliteProvider::updateUser(const User &user) {
    lock_guard<recursive_mutex> locker(lock_);

    sqlite3_stmt *stmt = getStatement(STATEMENT_UPDATE_USER);
    sqlite3_reset(stmt);
    sqlite3_bind_text(stmt, sqlite3_bind_parameter_index(stmt, "username"),
                      user.username().c_str(),
                      -1, nullptr);
    sqlite3_bind_text(stmt, sqlite3_bind_parameter_index(stmt, "password"),
                      user.password().c_str(),
                      -1, nullptr);
    sqlite3_bind_int64(stmt, sqlite3_bind_parameter_index(stmt, "userid"),
                       user.id());
    int ret = sqlite3_step(stmt);
    checkSqliteResult(ret, "SqliteProvider::updateUser");
    int64_t rowsAffected = sqlite3_changes(connection_->handle());
    if (rowsAffected > 0) return true;
    else return false;
}


void SqliteProvider::deleteUser(const User& user) {
    lock_guard<recursive_mutex> locker(lock_);
    sqlite3_stmt *stmt = getStatement(STATEMENT_DELETE_USER_CHANNEL);
    sqlite3_reset(stmt);

    int userIdPos         = sqlite3_bind_parameter_index(stmt, "user_id");

    sqlite3_bind_int64(stmt, userIdPos, user.id());
    int ret = sqlite3_step(stmt);
    checkSqliteResult(ret, "SqliteProvider::deleteUser");
}


void SqliteProvider::createChannelsTable() {
    createTableByStatement(STATEMENT_CREATE_CHANNEL_TABLE, "SqliteProvider::createChannelsTable");
}

Channel* SqliteProvider::findChannelById(int64_t id) {
    lock_guard<recursive_mutex> locker(lock_);
    sqlite3_stmt *stmt = getStatement(STATEMENT_FIND_CHANNEL_BY_ID);
    sqlite3_reset(stmt);
    sqlite3_bind_int64(stmt, sqlite3_bind_parameter_index(stmt, "channelid"), id);
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


int64_t SqliteProvider::insertChannel(const Channel& channel) {
    lock_guard<recursive_mutex> locker(lock_);
    sqlite3_stmt *stmt = getStatement(STATEMENT_INSERT_NEW_CHANNEL);
    sqlite3_reset(stmt);
    sqlite3_bind_text(stmt, sqlite3_bind_parameter_index(stmt, "title"),
                      channel.title().c_str(),
                      -1, nullptr);
    sqlite3_bind_text(stmt, sqlite3_bind_parameter_index(stmt, "rss_link"),
                      channel.rssLink().c_str(),
                      -1, nullptr);
    sqlite3_bind_text(stmt, sqlite3_bind_parameter_index(stmt, "link"),
                      channel.link().c_str(),
                      -1, nullptr);
    sqlite3_bind_text(stmt, sqlite3_bind_parameter_index(stmt, "description"),
                      channel.description().c_str(),
                      -1, nullptr);
    sqlite3_bind_int64(stmt, sqlite3_bind_parameter_index(stmt, "update_interval_sec"),
                      channel.updateInterval());
    sqlite3_bind_text(stmt, sqlite3_bind_parameter_index(stmt, "last_update"),
                      timestampToString(channel.lastUpdate(), SQLITE_DATE_FORMAT_STDLIB_SYNTAX).c_str(),
                      -1, nullptr);
    int ret = sqlite3_step(stmt);
    checkSqliteResult(ret, "SqliteProvider::insertChannel");
    int64_t newId = sqlite3_last_insert_rowid(connection_->handle());
    return newId;
}


bool SqliteProvider::updateChannel(const Channel &channel) {
    lock_guard<recursive_mutex> locker(lock_);

    int channelIdIdx, titleIdx, rssLinkIdx, linkIdx;
    int descIdx, updSecIdx, lastUpdIdx;

    sqlite3_stmt *stmt = getStatement(STATEMENT_UPDATE_CHANNEL);
    sqlite3_reset(stmt);

    channelIdIdx = sqlite3_bind_parameter_index(stmt, "channel_id");
    titleIdx = sqlite3_bind_parameter_index(stmt, "title");
    rssLinkIdx = sqlite3_bind_parameter_index(stmt, "rss_link");
    linkIdx = sqlite3_bind_parameter_index(stmt, "link");
    descIdx = sqlite3_bind_parameter_index(stmt, "description");
    updSecIdx = sqlite3_bind_parameter_index(stmt, "update_interval_sec");
    lastUpdIdx = sqlite3_bind_parameter_index(stmt, "last_update");

    string last_upd_str = timestampToString(channel.lastUpdate(), SQLITE_DATE_FORMAT_STDLIB_SYNTAX);
    
    sqlite3_bind_int64(stmt, channelIdIdx, channel.id());
    sqlite3_bind_text(stmt, titleIdx, channel.title().c_str(), -1, nullptr);
    sqlite3_bind_text(stmt, rssLinkIdx, channel.rssLink().c_str(), -1, nullptr);
    sqlite3_bind_text(stmt, linkIdx, channel.link().c_str(), -1, nullptr);
    sqlite3_bind_text(stmt, descIdx, channel.description().c_str(), -1, nullptr);
    sqlite3_bind_int64(stmt, updSecIdx, channel.updateInterval());
    sqlite3_bind_text(stmt, lastUpdIdx, last_upd_str.c_str(), -1, nullptr);
    int ret = sqlite3_step(stmt);
    checkSqliteResult(ret, "SqliteProvider::updateChannel");
    int64_t rowsAffected = sqlite3_changes(connection_->handle());
    if (rowsAffected > 0) return true;
    else return false;
}

void SqliteProvider::deleteChannel(const Channel& channel) {
    lock_guard<recursive_mutex> locker(lock_);
    sqlite3_stmt *stmt = getStatement(STATEMENT_DELETE_CHANNEL);
    sqlite3_reset(stmt);

    int channelIdPos    = sqlite3_bind_parameter_index(stmt, "channel_id");

    sqlite3_bind_int64(stmt, channelIdPos, channel.id());
    int ret = sqlite3_step(stmt);
    checkSqliteResult(ret, "SqliteProvider::deleteChannel");
}

void SqliteProvider::createFeedsTable() {
    createTableByStatement(STATEMENT_CREATE_FEED_TABLE, "SqliteProvider::createFeedsTable");
}


Feed* SqliteProvider::findFeedById(int64_t id) {
    lock_guard<recursive_mutex> locker(lock_);
    sqlite3_stmt *stmt = getStatement(STATEMENT_FIND_FEED_BY_ID);
    sqlite3_reset(stmt);
    int feedIdIdx = sqlite3_bind_parameter_index(stmt, "feed_id");
    sqlite3_bind_int64(stmt, feedIdIdx, id);
    int ret = sqlite3_step(stmt);
    checkSqliteResult(ret, "SqliteProvider::findFeedById");

    if (ret == SQLITE_DONE) {
        /* 0 rows was found */
        return nullptr;
    }

    Feed *feed = new Feed();
    try {
        parseFeedRow(stmt, *feed);
    } catch (logic_error &e) {
        SERVICE_LOG_LVL(ERROR, e.what());
        delete feed;
        throw SqliteProviderException(e.what());
    }

    while (ret != SQLITE_DONE) {
        ret = sqlite3_step(stmt);
    }

    return feed;
}

Feed* SqliteProvider::findFeedByGuid(const std::string& guid) {
    lock_guard<recursive_mutex> locker(lock_);
    sqlite3_stmt *stmt = getStatement(STATEMENT_FIND_FEED_BY_GUID);
    sqlite3_reset(stmt);
    int guidIdIdx = sqlite3_bind_parameter_index(stmt, "guid");
    sqlite3_bind_text(stmt, guidIdIdx, guid.c_str(), -1, nullptr);
    int ret = sqlite3_step(stmt);
    checkSqliteResult(ret, "SqliteProvider::findFeedByGuid");

    if (ret == SQLITE_DONE) {
        /* 0 rows was found */
        return nullptr;
    }

    Feed *feed = new Feed();
    try {
        parseFeedRow(stmt, *feed);
    } catch (logic_error &e) {
        SERVICE_LOG_LVL(ERROR, e.what());
        delete feed;
        throw SqliteProviderException(e.what());
    }

    while (ret != SQLITE_DONE) {
        ret = sqlite3_step(stmt);
    }

    return feed;
}

vector<Feed*>* SqliteProvider::getFeedsForChannel(int64_t channelId) {
    lock_guard<recursive_mutex> locker(lock_);
    sqlite3_stmt *stmt = getStatement(STATEMENT_FIND_FEED_BY_CHANNEL);
    sqlite3_reset(stmt);
    int channelIdIdx = sqlite3_bind_parameter_index(stmt, "channel_id");
    sqlite3_bind_int64(stmt, channelIdIdx, channelId);
    int ret = sqlite3_step(stmt);
    checkSqliteResult(ret, "SqliteProvider::findFeedsByChannel");

    if (ret == SQLITE_DONE) {
        /* 0 rows was found */
        return nullptr;
    }

    vector<Feed*> *foundFeeds = new vector<Feed *>();


    while (ret != SQLITE_DONE) {
        Feed *feed = new Feed();
        try {
            parseFeedRow(stmt, *feed);
        } catch (logic_error &e) {
            /* In case of exception we are just not adding
             * feed into result array. */
            SERVICE_LOG_LVL(ERROR, e.what());
            delete feed;
            ret = sqlite3_step(stmt);
            continue;
        }
        foundFeeds->push_back(feed);
        ret = sqlite3_step(stmt);
    }

    return foundFeeds;
}

vector<Feed*>* SqliteProvider::getFeedsForChannel(const Channel& channel) {
    return getFeedsForChannel(channel.id());
}

int64_t SqliteProvider::insertFeed(const Feed& feed) {
    lock_guard<recursive_mutex> locker(lock_);
    sqlite3_stmt *stmt = getStatement(STATEMENT_INSERT_NEW_FEED);
    sqlite3_reset(stmt);

    int channelIdPos    = sqlite3_bind_parameter_index(stmt, "channel_id");
    int guidPos         = sqlite3_bind_parameter_index(stmt, "guid");
    int titlePos        = sqlite3_bind_parameter_index(stmt, "title");
    int linkPos         = sqlite3_bind_parameter_index(stmt, "link");
    int descPos         = sqlite3_bind_parameter_index(stmt, "description");
    int pubDatePos      = sqlite3_bind_parameter_index(stmt, "pub_date");
    int feedTxtPos      = sqlite3_bind_parameter_index(stmt, "feed_txt");

    sqlite3_bind_int64(stmt, channelIdPos, feed.channelId());
    sqlite3_bind_text(stmt, guidPos, feed.guid().c_str(), -1, nullptr);
    sqlite3_bind_text(stmt, titlePos, feed.title().c_str(), -1, nullptr);
    sqlite3_bind_text(stmt, linkPos, feed.link().c_str(), -1, nullptr);
    sqlite3_bind_text(stmt, descPos, feed.description().c_str(), -1, nullptr);
    sqlite3_bind_text(stmt, pubDatePos,
                      timestampToString(feed.publicationDate(),
                                        SQLITE_DATE_FORMAT_STDLIB_SYNTAX).c_str(),
                      -1, nullptr);
    sqlite3_bind_text(stmt, feedTxtPos, feed.text().c_str(), -1, nullptr);
    int ret = sqlite3_step(stmt);
    checkSqliteResult(ret, "SqliteProvider::insertFeed");
    int64_t newId = sqlite3_last_insert_rowid(connection_->handle());
    return newId;
}

bool SqliteProvider::updateFeed(const Feed& feed) {
    lock_guard<recursive_mutex> locker(lock_);
    sqlite3_stmt *stmt = getStatement(STATEMENT_UPDATE_FEED);
    sqlite3_reset(stmt);

    int feedIdPos       = sqlite3_bind_parameter_index(stmt, "feed_id");
    int channelIdPos    = sqlite3_bind_parameter_index(stmt, "channel_id");
    int guidPos         = sqlite3_bind_parameter_index(stmt, "guid");
    int titlePos        = sqlite3_bind_parameter_index(stmt, "title");
    int linkPos         = sqlite3_bind_parameter_index(stmt, "link");
    int descPos         = sqlite3_bind_parameter_index(stmt, "description");
    int pubDatePos      = sqlite3_bind_parameter_index(stmt, "pub_date");
    int feedTxtPos      = sqlite3_bind_parameter_index(stmt, "feed_txt");

    sqlite3_bind_int64(stmt, feedIdPos, feed.id());
    sqlite3_bind_int64(stmt, channelIdPos, feed.channelId());
    sqlite3_bind_text(stmt, guidPos, feed.guid().c_str(), -1, nullptr);
    sqlite3_bind_text(stmt, titlePos, feed.title().c_str(), -1, nullptr);
    sqlite3_bind_text(stmt, linkPos, feed.link().c_str(), -1, nullptr);
    sqlite3_bind_text(stmt, descPos, feed.description().c_str(), -1, nullptr);
    sqlite3_bind_text(stmt, pubDatePos,
                      timestampToString(feed.publicationDate(),
                                        SQLITE_DATE_FORMAT_STDLIB_SYNTAX).c_str(),
                      -1, nullptr);
    sqlite3_bind_text(stmt, feedTxtPos, feed.text().c_str(), -1, nullptr);
    int ret = sqlite3_step(stmt);
    checkSqliteResult(ret, "SqliteProvider::updateFeed");
    int64_t rowsAffected = sqlite3_changes(connection_->handle());
    if (rowsAffected > 0) return true;
    else return false;
}

void SqliteProvider::deleteFeed(const Feed& feed) {
    lock_guard<recursive_mutex> locker(lock_);
    sqlite3_stmt *stmt = getStatement(STATEMENT_DELETE_FEED);
    sqlite3_reset(stmt);

    int feedIdPos    = sqlite3_bind_parameter_index(stmt, "feed_id");

    sqlite3_bind_int64(stmt, feedIdPos, feed.id());
    int ret = sqlite3_step(stmt);
    checkSqliteResult(ret, "SqliteProvider::deleteFeed");
}

void SqliteProvider::createSubsriptionTable() {
    createTableByStatement(STATEMENT_CREATE_USER_CHANNEL_TABLE, "SqliteProvider::createSubsriptionTable");
}

vector<Channel*>* SqliteProvider::getSubscriptionsForUser(const User& user) {
    lock_guard<recursive_mutex> locker(lock_);
    sqlite3_stmt *stmt = getStatement(STATEMENT_FIND_CHANNELS_BY_USER_ID);
    sqlite3_reset(stmt);
    int userIdIdx = sqlite3_bind_parameter_index(stmt, "user_id");
    sqlite3_bind_int64(stmt, userIdIdx, user.id());
    int ret = sqlite3_step(stmt);
    checkSqliteResult(ret, "SqliteProvider::getSubscriptionsForUser");

    if (ret == SQLITE_DONE) {
        /* 0 rows was found */
        return nullptr;
    }

    vector<Channel *> *foundChannels = new vector<Channel *>();


    while (ret != SQLITE_DONE) {
        Channel *channel = new Channel();
        try {
            parseChannelRow(stmt, *channel);
        } catch (logic_error &e) {
            /* In case of exception we are just not adding
             * feed into result array. */
            SERVICE_LOG_LVL(ERROR, e.what());
            delete channel;
            ret = sqlite3_step(stmt);
            continue;
        }
        foundChannels->push_back(channel);
        ret = sqlite3_step(stmt);
    }

    return foundChannels;
}

std::vector<User*>* SqliteProvider::getUsersForChannel(const Channel& channel) {
    lock_guard<recursive_mutex> locker(lock_);
    sqlite3_stmt *stmt = getStatement(STATEMENT_FIND_CHANNELS_BY_USER_ID);
    sqlite3_reset(stmt);
    int channelIdIdx = sqlite3_bind_parameter_index(stmt, "channel_id");
    sqlite3_bind_int64(stmt, channelIdIdx, channel.id());
    int ret = sqlite3_step(stmt);
    checkSqliteResult(ret, "SqliteProvider::getUsersForChannel");

    if (ret == SQLITE_DONE) {
        /* 0 rows was found */
        return nullptr;
    }

    vector<User *> *foundUsers = new vector<User *>();


    while (ret != SQLITE_DONE) {
        User *user = new User();
        try {
            parseUserRow(stmt, *user);
        } catch (logic_error &e) {
            /* In case of exception we are just not adding
             * feed into result array. */
            SERVICE_LOG_LVL(ERROR, e.what());
            delete user;
            ret = sqlite3_step(stmt);
            continue;
        }
        foundUsers->push_back(user);
        ret = sqlite3_step(stmt);
    }

    return foundUsers;
}

void SqliteProvider::subscribeUser(const User& user, const Channel& channel) {
    lock_guard<recursive_mutex> locker(lock_);
    sqlite3_stmt *stmt = getStatement(STATEMENT_INSERT_NEW_USER_CHANNEL);
    sqlite3_reset(stmt);

    int channelIdPos    = sqlite3_bind_parameter_index(stmt, "channel_id");
    int userIdPos         = sqlite3_bind_parameter_index(stmt, "user_id");

    sqlite3_bind_int64(stmt, channelIdPos, channel.id());
    sqlite3_bind_int64(stmt, userIdPos, user.id());
    int ret = sqlite3_step(stmt);
    checkSqliteResult(ret, "SqliteProvider::subscribeUser");
}


void SqliteProvider::unsubscribeUser(const User& user, const Channel& channel) {
    lock_guard<recursive_mutex> locker(lock_);
    sqlite3_stmt *stmt = getStatement(STATEMENT_DELETE_USER_CHANNEL);
    sqlite3_reset(stmt);

    int channelIdPos    = sqlite3_bind_parameter_index(stmt, "channel_id");
    int userIdPos         = sqlite3_bind_parameter_index(stmt, "user_id");

    sqlite3_bind_int64(stmt, channelIdPos, channel.id());
    sqlite3_bind_int64(stmt, userIdPos, user.id());
    int ret = sqlite3_step(stmt);
    checkSqliteResult(ret, "SqliteProvider::unsubscribeUser");
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
        out.setLastUpdate(stringToTimestamp(timestamp, SQLITE_DATE_FORMAT_STDLIB_SYNTAX));
    }
    else {
        throw logic_error("SqliteProvider::parseChannelRow: invalid column 6 type");
    }
}


void SqliteProvider::parseFeedRow(sqlite3_stmt* stmt, Feed& out) {
    if (!stmt)
        throw logic_error("SqliteProvider::parseFeedRow: invalid argument `stmt`");
    int columns = sqlite3_column_count(stmt);
    if (columns != 8) {
        ostringstream oss;
        oss << "SqliteProvider::parseFeedRow: invalid column count in result set. Expected: 8. Actual: " << columns;
        throw logic_error(oss.str());
    }

    if (sqlite3_column_type(stmt, 0) == SQLITE_INTEGER)
        out.setId(sqlite3_column_int(stmt, 0));
    else
        throw logic_error("SqliteProvider::parseFeedRow: invalid column 0 type");

    if (sqlite3_column_type(stmt, 1) == SQLITE_INTEGER)
        out.setChannelId(sqlite3_column_int(stmt, 1));
    else
        throw logic_error("SqliteProvider::parseFeedRow: invalid column 1 type");

    if (sqlite3_column_type(stmt, 2) == SQLITE_TEXT)
        out.setGuid(string(reinterpret_cast<const char *>(sqlite3_column_text(stmt, 2))));
    else
        throw logic_error("SqliteProvider::parseFeedRow: invalid column 2 type");

    if (sqlite3_column_type(stmt, 3) == SQLITE_TEXT)
        out.setTitle(string(reinterpret_cast<const char *>(sqlite3_column_text(stmt, 3))));
    else
        throw logic_error("SqliteProvider::parseFeedRow: invalid column 3 type");

    if (sqlite3_column_type(stmt, 4) == SQLITE_TEXT)
        out.setLink(string(reinterpret_cast<const char *>(sqlite3_column_text(stmt, 4))));
    else
        throw logic_error("SqliteProvider::parseFeedRow: invalid column 4 type");

    if (sqlite3_column_type(stmt, 5) == SQLITE_TEXT)
        out.setDescription(string(reinterpret_cast<const char *>(sqlite3_column_text(stmt, 5))));
    else
        throw logic_error("SqliteProvider::parseFeedRow: invalid column 5 type");

    if (sqlite3_column_type(stmt, 6) == SQLITE_TEXT) {
        string timestamp = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 6));
        out.setPublicationDate(stringToTimestamp(timestamp, SQLITE_DATE_FORMAT_STDLIB_SYNTAX));
    }
    else {
        throw logic_error("SqliteProvider::parseFeedRow: invalid column 6 type");
    }

    if (sqlite3_column_type(stmt, 7) == SQLITE_TEXT)
        out.setText(string(reinterpret_cast<const char *>(sqlite3_column_text(stmt, 7))));
    else
        throw logic_error("SqliteProvider::parseFeedRow: invalid column 7 type");
}

void SqliteProvider::createTableByStatement(int stmtIndex, const std::string& tag) {
    lock_guard<recursive_mutex> locker(lock_);
    sqlite3_stmt *stmt = getStatement(stmtIndex);
    sqlite3_reset(stmt);
    int ret = sqlite3_step(stmt);
    checkSqliteResult(ret, tag);

    if (ret != SQLITE_DONE) {
        string str = tag + string(": unexpected return code");
        SERVICE_LOG_LVL(ERROR, str);
        throw SqliteProviderException(str);
    }
}

} /* namespace service */
} /* namespace nestor */


