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

#ifndef HTTP_CLIENT_H_
#define HTTP_CLIENT_H_

#include <string>
#include <stdexcept>
#include <vector>
#include <netdb.h>
#include <curl/curl.h>
#include "http_resource.h"
#include "socket_single.h"

namespace nestor {
namespace net {

class HttpClient {
public:
    HttpClient();

    HttpResource *getResource(const std::string &resource);
    void setup(const std::string &resource);
    bool perform();
    CURL *handle() const;
    HttpResource *parseReceivedData();

    virtual ~HttpClient();

private:
    static size_t writeFuncHelper( void* ptr, size_t size, size_t nmemb, void* userdata);

private:
    CURL *handle_;
    unsigned char *recvBuffer_;
    size_t recvBufferSize_;
};

} /* namespace net */
} /* namespace nestor */
#endif /* HTTP_CLIENT_H_ */
