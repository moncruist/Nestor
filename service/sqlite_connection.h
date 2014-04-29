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
#ifndef SQLITE_CONNECTION_H_
#define SQLITE_CONNECTION_H_

#include <string>
#include <stdexcept>
#include "sqlite3.h"

namespace nestor {
namespace service {

class SqliteConnectionException : public std::runtime_error {
    using std::runtime_error::runtime_error;
};

class SqliteConnection {
public:
    explicit SqliteConnection(const std::string &fileName);
    virtual ~SqliteConnection();

    void open();
    void close();

    bool connected() const;

    sqlite3 *handle() const;

    const std::string& fileName() const;
    void setFileName(const std::string& fileName);

private:
    std::string fileName_;
    sqlite3 *handle_;
    bool connected_;
};

} /* namespace service */
} /* namespace nestor */

#endif /* SQLITE_CONNECTION_H_ */
