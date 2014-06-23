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
#include "common/logger.h"
#include "sqlite_connection.h"

using namespace std;

namespace nestor {
namespace service {

SqliteConnection::SqliteConnection(const std::string &fileName)
        : fileName_(fileName), handle_(nullptr), connected_(false) {
    onCloseCallbacks_.clear();
}

SqliteConnection::~SqliteConnection() {
    try {
        close();
    } catch(SqliteConnectionException &e) {
        SERVICE_LOG_LVL(ERROR, "SqliteConnection::dtr: exception while closing database");
    }
}


void SqliteConnection::open() {
    int code = sqlite3_open(fileName_.c_str(), &handle_);
    if (code != SQLITE_OK) {
        SERVICE_LOG_LVL(ERROR, "SqliteConnection::open: cannot connect to file: " << fileName_
                << ". Error code: " << code << " Description: " << sqlite3_errstr(code));
        throw SqliteConnectionException(sqlite3_errstr(code));
    }
    connected_ = true;
}

void SqliteConnection::close() {
    if (handle_) {
        for (auto callback: onCloseCallbacks_)
            callback(this);
        int code = sqlite3_close(handle_);
        handle_ = nullptr;
        connected_ = false;
        if (code != SQLITE_OK) {
            SERVICE_LOG_LVL(ERROR, "SqliteConnection::close: cannot close db file: " << fileName_
                    << ". Error code: " << code << " Description: " << sqlite3_errstr(code));
            throw SqliteConnectionException(sqlite3_errstr(code));
        }
    }
}


sqlite3* SqliteConnection::handle() const {
    return handle_;
}


bool SqliteConnection::connected() const {
    return connected_;
}


const std::string& SqliteConnection::fileName() const {
    return fileName_;
}


void SqliteConnection::setFileName(const std::string& fileName) {
    fileName_ = fileName;
}

int SqliteConnection::subscribeOnClose(SqliteConnectionCallback callback) {
    onCloseCallbacks_.push_back(callback);
    return onCloseCallbacks_.size() - 1;
}

} /* namespace service */
} /* namespace nestor */


