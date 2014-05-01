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
#ifndef SERVICE_H_
#define SERVICE_H_

#include <string>
#include "sqlite_connection.h"
#include "sqlite_provider.h"

namespace nestor {
namespace service {

/**
 * Core Nestor logic class.
 */
class Service {
public:
    Service(const SqliteConnection *connection);
    virtual ~Service();

    virtual bool authenticate(std::string login, std::string password);

    virtual void onLogout();

private:
    const SqliteConnection *connection_;
    SqliteProvider *dataProvider_;
};

} /* namespace service */
} /* namespace nestor */

#endif /* SERVICE_H_ */
