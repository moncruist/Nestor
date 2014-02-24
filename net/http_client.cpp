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
#include "http_client.h"
#include <string>
#include <iostream>
#include <sstream>
#include <cstring>


#include "http_response_parser.h"

using namespace std;

namespace nestor {
namespace net {

HttpClient::HttpClient(string host)
        throw (runtime_error)
        : sock_(nullptr), host_(host) {
    sock_ = new SocketSingle(host, 80);
}

HttpResource * HttpClient::getResource(const string &resource)
        throw(std::runtime_error) {
    ostringstream requestStr;
    string *result;

    requestStr << "GET " << resource << " HTTP/1.1\r\nHost: " << host_ << "\r\n\r\n";
    string str = requestStr.str();
    sock_->write(requestStr.str());

    result = new string(sock_->readAll());

    HttpResource *res = HttpResponseParser::parseRawData(*result);
    delete result;
    return res;
}

HttpClient::~HttpClient() {
    if (sock_) sock_->close();
}

} /* namespace net */
} /* namespace nestor */

