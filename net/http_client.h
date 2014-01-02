/*
 *  This file is part of Nestor.
 *
 *  Nestor - program for aggregation RSS subscriptions providing
 *  access via IMAP interface.
 *  Copyright (C) 2013  Konstantin Zhukov
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

#ifndef HTTP_CLIENT_H_
#define HTTP_CLIENT_H_

#include <string>
#include <stdexcept>
#include <vector>
#include <netdb.h>
#include "http_resource.h"

namespace nestor {
namespace net {

class HttpClient {
public:
    HttpClient(std::string host = "localhost", bool manualConnect = false)
            throw (std::runtime_error);

    void connect() throw (std::runtime_error);

    void close();

    HttpResource *getResource(const std::string &resource);

    virtual ~HttpClient();
private:
    std::string host_;
    int sockFd_;
    bool manualConnect_;
    addrinfo *servinfo;

    static const int readbuf_size = 1024;
};

} /* namespace net */
} /* namespace nestor */
#endif /* HTTP_CLIENT_H_ */
