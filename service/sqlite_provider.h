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
#include "sqlite_connection.h"
#include "types.h"

namespace nestor {
namespace service {

class SqliteProviderException : public std::runtime_error {
    using std::runtime_error::runtime_error;
};

class SqliteProvider {
public:
    SqliteProvider(const SqliteConnection *connection);
    virtual ~SqliteProvider();

    void prepareStatements();

    void createUsersTable();
    User *findUserByName(const std::string &username);

private:
    enum Statements {
        STATEMENT_CREATE_USER_TABLE = 0,
        STATEMENT_FIND_USER_BY_USERNAME,
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
