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
#include <string>
#include <sstream>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netdb.h>
#include <unistd.h>
#include <fcntl.h>
#include <cstring>
#include <cerrno>
#include <iostream>
#include "socket_single.h"

using namespace std;

namespace nestor {
namespace net {

enum SocketSelectResult {
    SOCKET_TIMEOUT = 0x00,
    SOCKET_READY_READ = 0x01,
    SOCKET_READY_WRITE = 0x02,
    SOCKET_ERROR = 0x04
};

SocketSingle::SocketSingle(string host, unsigned short port, bool manualConnect,
        unsigned int timeoutMs) throw (SocketTimeoutException,
                SocketIOException)
        : host_(host), port_(port), sockFd_(-1), manualConnect_(manualConnect), connected_(
                false), timeoutMs_(timeoutMs) {
    try {
        if (!manualConnect_)
            connect();
    } catch (runtime_error &e) {
        close();
        throw;
    }
}

void SocketSingle::connect() throw (SocketTimeoutException, SocketIOException) {
    addrinfo hints, *servinfo;
    int res;
    ostringstream oss_err;

    bool was_timeout = false;

    if (host_.length() == 0)
        throw SocketIOException("SocketSingle::connect() Host length is 0");

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags |= AI_NUMERICSERV;

    char portBuffer[30];
    snprintf(portBuffer, 30, "%d", port_);

    if ((res = getaddrinfo(host_.c_str(), portBuffer, &hints, &servinfo))
            != 0) {
        oss_err << "SocketSingle::connect() getaddrinfo error: "
                << gai_strerror(res);
        goto error;
    }

    sockFd_ = socket(servinfo->ai_family, servinfo->ai_socktype,
            servinfo->ai_protocol);
    if (sockFd_ < 0) {
        oss_err << "Socket::connect() socket error: " << strerror(errno);
        goto error;
    }

    // Making nonblocking socket if timeout was specified
    if (timeoutMs_) {
        int flags = fcntl(sockFd_, F_GETFL, 0);
        if (flags < 0) {
            oss_err << "SocketSingle::connect() fcntl get error: "
                    << strerror(errno);
            goto error;
        }

        flags |= O_NONBLOCK;
        if (fcntl(sockFd_, F_SETFL, flags) != 0) {
            oss_err << "SocketSingle::connect() fcntl set error: "
                    << strerror(errno);
            goto error;
        }
    }

    if ((res = ::connect(sockFd_, servinfo->ai_addr, servinfo->ai_addrlen))
            < 0) {
        if (timeoutMs_ && errno == EINPROGRESS) {
            // Non blocking mode, waiting for connect via select
            int status = wait();
            if (!(status & SOCKET_READY_WRITE)) {
                oss_err << "SocketSingle::connect() connect error: ";
                if (status & SOCKET_TIMEOUT) {
                    oss_err << "TIMEOUT ";
                    was_timeout = true;
                }
                if (status & SOCKET_ERROR)
                    oss_err << "EXCEPTION ";
                goto error;
            }
        } else {
            oss_err << "SocketSingle::connect() connect error: "
                    << strerror(errno);
            goto error;
        }
    }

    connected_ = true;
    freeaddrinfo(servinfo);
    return;

    error: freeaddrinfo(servinfo);
    close();
    if (was_timeout)
        throw SocketTimeoutException(oss_err.str());
    else
        throw SocketIOException(oss_err.str());
}

bool SocketSingle::connected() const {
    return connected_;
}

void SocketSingle::close() {
    if (!connected_)
        return;

    if (sockFd_ >= 0) {
        shutdown(sockFd_, 2);
        ::close(sockFd_);
        sockFd_ = -1;
    }
    connected_ = false;
}

SocketSingle::~SocketSingle() {
    close();
}

void SocketSingle::write(const char* buf, size_t buflen)
        throw (SocketTimeoutException, SocketIOException) {
    size_t sended = 0;
    int res;
    ostringstream oss_err;

    while (sended < buflen) {
        if (timeoutMs_) {
            int state = wait(false, true);

            if (state & SOCKET_TIMEOUT)
                throw SocketTimeoutException("SocketSingle::write timeout");
            if (!(state & SOCKET_READY_WRITE))
                throw SocketIOException("SocketSingle::write cannot write");
            if (state & SOCKET_ERROR) {
                oss_err << "SocketSingle::write wait error " << strerror(errno);
                throw SocketIOException(oss_err.str());
            }
        }

        if ((res = send(sockFd_, &buf[sended], buflen - sended, 0)) < 0) {
            if (errno != EWOULDBLOCK) {
                oss_err << "SocketSingle::write send error " << strerror(errno);
                throw SocketIOException(oss_err.str());
            }
        } else {
            sended += res;
        }
    }
}

void SocketSingle::write(const std::string& str) throw (SocketTimeoutException,
        SocketIOException) {
    write(str.c_str(), str.length());
}

size_t SocketSingle::read(char* buf, size_t buflen)
        throw (SocketTimeoutException, SocketIOException) {
    ostringstream oss_err;
    size_t readed = 0;
    int res;
    do {
        if (timeoutMs_) {
            int state = wait(true, false);

            if (state & SOCKET_ERROR) {
                oss_err << "SocketSingle::read wait error " << strerror(errno);
                throw SocketIOException(oss_err.str());
            }

            if (state & SOCKET_TIMEOUT)
                throw SocketTimeoutException("SocketSingle::read timeout");
            if (!(state & SOCKET_READY_READ)) {
                break;
            }

        }

        res = recv(sockFd_, &buf[readed], buflen - readed, 0);

        if (res < 0){
            if(errno != EWOULDBLOCK) {
                oss_err << "SocketSingle::read error " << strerror(errno);
                throw SocketIOException(oss_err.str());
            } else {
                break;
            }
        }

        readed += res;
        if (readed == buflen)
            break;
    } while (res != 0);
    return readed;
}

std::string SocketSingle::read() throw (SocketTimeoutException,
        SocketIOException) {
    char buf[1024];
    int readed;
    string result = "";
    while ((readed = read(buf, 1024)) > 0) {
        result.append(buf, readed);
    }
    return result;
}

/* returns SocketSelectResult flags */
int SocketSingle::wait(bool waitRead, bool waitWrite) {
    fd_set rfds, wfds;
    timeval timeout;

    if (!waitRead && !waitWrite)
        return 0;

    FD_ZERO(&rfds);
    FD_ZERO(&wfds);

    FD_SET(sockFd_, &rfds);
    FD_SET(sockFd_, &wfds);

    timeout.tv_sec = timeoutMs_ / 1000;
    timeout.tv_usec = (timeoutMs_ - timeout.tv_sec * 1000) * 1000;

    int numevnts = select(sockFd_ + 1, waitRead ? &rfds : NULL,
            waitWrite ? &wfds : NULL, NULL, timeoutMs_ ? &timeout : NULL);

    if (numevnts == 0)
        return SOCKET_TIMEOUT;

    int flag = 0;
    if (FD_ISSET(sockFd_, &rfds) && waitRead)
        flag |= SOCKET_READY_READ;
    if (FD_ISSET(sockFd_, &wfds) && waitWrite)
        flag |= SOCKET_READY_WRITE;

    int error;
    int len = sizeof(error);
    getsockopt(sockFd_, SOL_SOCKET, SO_ERROR, &error,
            reinterpret_cast<socklen_t *>(&len));

    if (error) {
        close();
        errno = error;
        return SOCKET_ERROR;
    }

    return flag;
}

int SocketSingle::descriptor() const {
    return sockFd_;
}

} /* namespace net */
} /* namespace nestor */


