/*
 *  This file is part of Nestor.
 *
 *  Nestor - program for aggregation RSS subscriptions with access via
 *  IMAP interface.
 *  Copyright (C) 2013  Konstantin Zhukov moncruist@gmail.com
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
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netdb.h>
#include <unistd.h>
#include <fcntl.h>


using namespace std;

namespace nestor {
namespace net {

HttpClient::HttpClient(string host, bool manualConnect)
        throw (runtime_error) :
        host_(host), sockFd_(-1), manualConnect_(manualConnect) {
    try {
        if (!manualConnect_)
            connect();
    } catch(runtime_error &e) {
        close();
        throw;
    }
}

void HttpClient::connect() throw (runtime_error) {
    addrinfo hints, *servinfo;
    int res;
    ostringstream oss_err;

    if (host_.length() == 0)
        throw runtime_error("Host length is 0");

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if ((res = getaddrinfo(host_.c_str(), "http", &hints, &servinfo)) != 0) {
        oss_err << "HttpClient::connect() getaddrinfo error: " << gai_strerror(res);
        throw runtime_error(oss_err.str());
    }

    sockFd_ = socket(AF_INET, SOCK_STREAM, 0);
    if (sockFd_ < 0) {
        oss_err << "HttpClient::connect() socket error: " << strerror(errno);
        freeaddrinfo(servinfo);
        throw runtime_error(oss_err.str());
    }

    if (::connect(sockFd_, servinfo->ai_addr, servinfo->ai_addrlen) < 0) {
        oss_err << "HttpClient::connect() connect error: " << strerror(errno);
        freeaddrinfo(servinfo);
        close();
        throw runtime_error(oss_err.str());
    }

    freeaddrinfo(servinfo);

}

void HttpClient::close() {
    if (sockFd_ >= 0) {
        shutdown(sockFd_, 2);
        ::close(sockFd_);
        sockFd_ = -1;
    }
}

vector<unsigned char>* HttpClient::getResource(string resource) {
    ostringstream requestStr;
    ostringstream oss_err;
    vector<unsigned char> *result;
    unsigned char *tmpbuf;
    fd_set rdset;
    timeval timeout;
    int rc;

    if (sockFd_ < 0) {
        throw runtime_error("HttpClient::getResource: invalid socket");
    }

    requestStr << "GET " << resource << " HTTP/1.0\nHost: " << host_ << "\n\n";
    string str = requestStr.str();
    cout << "Request string:\n" << str << endl;
    int sended = send(sockFd_, str.c_str(), str.length(), MSG_NOSIGNAL | MSG_DONTWAIT); // no SIGPIPE
    if (sended < 0) {
        oss_err << "HttpClient::getResource: send failed: " << strerror(errno);
        throw runtime_error(oss_err.str());
    }

    FD_ZERO(&rdset);
    FD_SET(sockFd_, &rdset);
    timeout.tv_sec = 1;
    rc = select(sockFd_ + 1, &rdset, NULL, NULL, &timeout);
    cout << "Select result: " << rc << endl;
    if (rc == 0) {
        throw runtime_error("HttpClient::getResource: recv timeout\n");
    } else if (rc < 0) {
        oss_err << "HttpClient::getResource: select failed: "
                << !FD_ISSET(sockFd_, &rdset) ?
                "unknown error" : strerror(errno);
        throw runtime_error(oss_err.str());
    }

    result = new vector<unsigned char>();
    tmpbuf = new unsigned char[readbuf_size];

    int readed;
    while ((readed = recv(sockFd_, tmpbuf, sizeof(unsigned char) * readbuf_size, 0)) > 0) {
        for(int i = 0; i < readed; i++)
            result->push_back(tmpbuf[i]);
    }

    if (readed < 0) {
        delete[] tmpbuf;
        delete result;
        oss_err << "HttpClient::getResource: recv failed: " << strerror(errno);
        throw runtime_error(oss_err.str());
    }

    delete[] tmpbuf;

    return result;
}

HttpClient::~HttpClient() {
    close();
}

} /* namespace net */
} /* namespace nestor */

