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
#ifndef SQLITE_PROVIDER_H_
#define SQLITE_PROVIDER_H_

#include <stdexcept>
#include <mutex>
#include <vector>
#include "sqlite_connection.h"
#include "types.h"

namespace nestor {
namespace service {

/**
 * Exception which may be thrown by SqliteProvider methods
 */
class SqliteProviderException : public std::runtime_error {
    using std::runtime_error::runtime_error;
};

/**
 * Class provides functions for storing and querying SQLite database.
 */
class SqliteProvider {
public:
    SqliteProvider(const SqliteConnection *connection);
    virtual ~SqliteProvider();

    /**
     * Compiles all SQL queries and commands.
     */
    void prepareStatements();

    /**
     * Create 'users' table for storing information about users
     * (username and password). Currently, 'users' table entirely
     * maps to the nestor::service::User class.
     * May throw SqliteProviderException.
     */
    void createUsersTable();

    /**
     * Search user in 'users' table by user name.
     * May throw SqliteProviderException.
     * @param username Username of the user to search
     * @return Heap allocated User object or null if no user was found.
     */
    User *findUserByName(const std::string &username);

    /**
     * Search user in 'users' table by user id.
     * May throw SqliteProviderException.
     * @param id Identifier of the user to search.
     * @return Heap allocated User object or null if no user was found.
     */
    User *findUserById(int64_t id);

    /**
     * Inserts new user into the 'users' table.
     * May throw SqliteProviderException.
     * @param user Initialized user object to insert into the table.
     * Id field will be ignored.
     *
     * @return Identifier of the inserted object.
     */
    int64_t insertUser(const User &user);

    /**
     * Updates existent user in the 'users' table.
     * May throw SqliteProviderException.
     * @param user Initialized user object for updating the table.
     *
     * @return Returns true if the user was updated successfully,
     * false - no user was found with specified 'id'.
     */
    bool updateUser(const User &user);

    /**
     * Deletes user from `users` table.
     * @param user User object which contained initialized identifier
     */
    void deleteUser(const User &user);

    /**
     * Create 'channels' table for storing information about RSS
     * channels. Currently, 'channels' table entirely
     * maps to the nestor::service::Channel class.
     * May throw SqliteProviderException.
     */
    void createChannelsTable();

    /**
     * Search channel in 'channels' table by channel id.
     * May throw SqliteProviderException.
     * @param id Identifier of the channel to search.
     * @return Heap allocated Channel object or null if no channel was found.
     */
    Channel *findChannelById(int64_t id);

    /**
     * Inserts new channel into the 'channels' table.
     * May throw SqliteProviderException.
     * @param channel Initialized channel object to insert into the table.
     * Id field will be ignored.
     *
     * @return Identifier of the inserted object.
     */
    int64_t insertChannel(const Channel &channel);

    /**
     * Updates existent channel in the 'channels' table.
     * May throw SqliteProviderException.
     * @param user Initialized channel object for updating the table.
     *
     * @return Returns true if the channel was updated successfully,
     * false - no channel was found with specified 'id'.
     */
    bool updateChannel(const Channel &channel);

    /**
     * Deletes channel from `channels` table.
     * @param channel Channel object which contained initialized identifier
     */
    void deleteChannel(const Channel &channel);

    /**
     * Create 'feeds' table for storing RSS feeds.
     * Currently, 'feeds' table entirely
     * maps to the nestor::service::Feed class.
     * May throw SqliteProviderException.
     */
    void createFeedsTable();

    /**
     * Search feed in 'feeds' table by feed id.
     * May throw SqliteProviderException.
     * @param id Identifier of the feed to search.
     * @return Heap allocated Feed object or null if no feed was found.
     */
    Feed *findFeedById(int64_t id);

    /**
     * Search feed in 'feeds' table by feed guid.
     * May throw SqliteProviderException.
     * @param guid GUID of the feed to search.
     * @return Heap allocated Feed object or null if no feed was found.
     */
    Feed *findFeedByGuid(const std::string &guid);

    /**
     * Search feeds in 'feeds' table of specified channel.
     * May throw SqliteProviderException.
     * @param channelId Identifier of feeds channel.
     * @return Heap allocated vector of Feed objects.
     */
    std::vector<Feed *> *getFeedsForChannel(int64_t channelId);
    std::vector<Feed *> *getFeedsForChannel(const Channel &channel);

    /**
     * Inserts new channel into the 'channels' table.
     * May throw SqliteProviderException.
     * @param channel Initialized channel object to insert into the table.
     * Id field will be ignored.
     *
     * @return Identifier of the inserted object.
     */
    int64_t insertFeed(const Feed &feed);

    /**
     * Updates existent channel in the 'channels' table.
     * May throw SqliteProviderException.
     * @param user Initialized channel object for updating the table.
     *
     * @return Returns true if the channel was updated successfully,
     * false - no channel was found with specified 'id'.
     */
    bool updateFeed(const Feed &feed);

    /**
     * Deletes feed from `feeds` table.
     * @param feed Feed object contained which contained initialized identifier
     */
    void deleteFeed(const Feed &feed);

    /**
     * Create table for storing user subscriptions.
     * May throw SqliteProviderException.
     */
    void createSubsriptionTable();

    std::vector<Channel *> *getSubscriptionsForUser(const User &user);
    std::vector<User *> *getUsersForChannel(const Channel &channel);

    void subscribeUser(const User &user, const Channel &channel);

    void unsubscribeUser(const User &user, const Channel &channel);
private:

    /**
     * Returns compiled sqlite statement by index.
     */
    sqlite3_stmt *getStatement(int statementCode);

    /**
     * Checks sqlite3_step result code and throws SqliteProviderException
     * if error was occured. Into exception message adds string tag for
     * identifying in which public method error was occured.
     */
    void checkSqliteResult(int retCode, const std::string &tag);

    /**
     * Maps sqlite3 result row from 'users' table to the User object.
     */
    void parseUserRow(sqlite3_stmt *stmt, User &out);

    /**
     * Maps sqlite3 result row from 'channels' table to the Channel object.
     */
    void parseChannelRow(sqlite3_stmt *stmt, Channel &out);

    /**
     * Maps sqlite3 result row from 'feeds' table to the Feed object.
     */
    void parseFeedRow(sqlite3_stmt *stmt, Feed &out);

    void createTableByStatement(int stmtIndex, const std::string &tag);

private:
    enum Statements {
        // USER table -----------------
        STATEMENT_CREATE_USER_TABLE = 0,
        STATEMENT_FIND_USER_BY_USERNAME,
        STATEMENT_FIND_USER_BY_ID,
        STATEMENT_INSERT_NEW_USER,
        STATEMENT_UPDATE_USER,
        STATEMENT_DELETE_USER,

        // CHANNEL table --------------
        STATEMENT_CREATE_CHANNEL_TABLE,
        STATEMENT_FIND_CHANNEL_BY_ID,
        STATEMENT_INSERT_NEW_CHANNEL,
        STATEMENT_UPDATE_CHANNEL,
        STATEMENT_DELETE_CHANNEL,

        // FEEDS table ----------------
        STATEMENT_CREATE_FEED_TABLE,
        STATEMENT_FIND_FEED_BY_ID,
        STATEMENT_FIND_FEED_BY_GUID,
        STATEMENT_FIND_FEED_BY_CHANNEL,
        STATEMENT_INSERT_NEW_FEED,
        STATEMENT_UPDATE_FEED,
        STATEMENT_DELETE_FEED,

        // USER_CHANNEL table ---------
        STATEMENT_CREATE_USER_CHANNEL_TABLE,
        STATEMENT_FIND_CHANNELS_BY_USER_ID,
        STATEMENT_FIND_USERS_BY_CHANNEL_ID,
        STATEMENT_INSERT_NEW_USER_CHANNEL,
        STATEMENT_DELETE_USER_CHANNEL,
        STATEMENTS_LENGTH
    };
    static const char *SQL_STATEMENTS[STATEMENTS_LENGTH];

private:
    const SqliteConnection *connection_;
    sqlite3_stmt *compiledStatements_[STATEMENTS_LENGTH];

    std::recursive_mutex lock_;
};

} /* namespace service */
} /* namespace nestor */

#endif /* SQLITE_PROVIDER_H_ */
