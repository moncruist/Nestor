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
#include "service.h"
#include "logger.h"

namespace nestor {
namespace service {

Service::Service(const SqliteConnection *connection)
        : connection_(connection) {
    dataProvider_ = new SqliteProvider(connection);
    dataProvider_->prepareStatements();
}

Service::~Service() {
    delete dataProvider_;
}

bool Service::authenticate(std::string login, std::string password) {
    User *usr;
    try {
        usr = dataProvider_->findUserByName(login);
    } catch (SqliteProviderException &) {
        SERVICE_LOG_LVL(ERROR, "Service::authenticate: cannot find user");
        return false;
    }

    bool result;
    if (!usr || usr->password() != password)
        result = false;
    else
        result = true;

    delete usr;
    return result;
}

void Service::onLogout() {
}

} /* namespace service */
} /* namespace nestor */
