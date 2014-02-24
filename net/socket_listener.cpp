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

#include "socket_listener.h"

#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <sys/socket.h>
//#include <sys/types.h>
#include <unistd.h>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <sstream>


using namespace std;

namespace nestor {
namespace net {


SocketListener::SocketListener(std::string host, unsigned short port)
        throw (SocketIOException)
        : host_(host), port_(port), sfd_(-1), started_(false) {
}


SocketListener::~SocketListener() {
    if (sfd_ >= 0)
        close();
}

const std::string& SocketListener::host() const {
    return host_;
}

void SocketListener::setHost(const std::string& host) {
    host_ = host;
}

unsigned short SocketListener::port() const {
    return port_;
}


void SocketListener::startListen() {
    addrinfo hints, *servinfo;
    int res, flags;
    ostringstream oss_err;

    if (host_.length() == 0)
        throw SocketIOException(
                "SocketListener::startListen() Host length is 0");

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags |= AI_NUMERICSERV | AI_PASSIVE;

    char portBuffer[30];
    snprintf(portBuffer, 30, "%d", port_);

    if ((res = getaddrinfo(host_.c_str(), portBuffer, &hints, &servinfo))
            != 0) {
        oss_err << "SocketListener::startListen() getaddrinfo error: "
                << gai_strerror(res);
        goto error;
    }

    sfd_ = socket(servinfo->ai_family, servinfo->ai_socktype,
            servinfo->ai_protocol);
    if (sfd_ < 0) {
        oss_err << "SocketListener::startListen() socket error: " << strerror(errno);
        goto error;
    }

    res = bind(sfd_, hints.ai_addr, hints.ai_addrlen);
    if (res < 0) {
        oss_err << "SocketListener::startListen() bind error: " << strerror(errno);
        goto error;
    }

    flags = fcntl(sfd_, F_GETFL, 0);
    if (flags < 0) {
        oss_err << "SocketListener::startListen() fcntl get error: "
                << strerror(errno);
        goto error;
    }

    flags |= O_NONBLOCK;
    if (fcntl(sfd_, F_SETFL, flags) != 0) {
        oss_err << "SocketListener::startListen() fcntl set error: "
                << strerror(errno);
        goto error;
    }

    freeaddrinfo(servinfo);
    started_ = true;

    return;

error:
    freeaddrinfo(servinfo);
    close();
    throw SocketIOException(oss_err.str());
    return;
}

SocketSingle* SocketListener::accept() {
    if (!started_ || sfd_ < 0)
        return nullptr;
    int confd = ::accept(sfd_, nullptr, nullptr);
    if (confd == -1){
        return nullptr;
    }
    return new SocketSingle(confd, 0, true);
}

void SocketListener::close() {
    if (sfd_ >= 0) {
       ::shutdown(sfd_, SHUT_RDWR);
       ::close(sfd_);
       sfd_ = -1;
       started_ = false;
   }
}

int SocketListener::descriptor() const {
    return sfd_;
}

void SocketListener::setPort(unsigned short port) {
    port_ = port;
}

} /* namespace net */
} /* namespace nestor */
