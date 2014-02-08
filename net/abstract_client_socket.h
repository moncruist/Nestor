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
#ifndef ABSTRACT_CLIENT_SOCKET_H_
#define ABSTRACT_CLIENT_SOCKET_H_

#include <stdexcept>

namespace nestor {
namespace net {

class SocketTimeoutException : public std::runtime_error {
    using std::runtime_error::runtime_error;
};

class SocketIOException : public std::runtime_error {
    using std::runtime_error::runtime_error;
};

/* TODO: Make an abstract class, not interface */
class AbstractClientSocket {
public:
    AbstractClientSocket();
    virtual ~AbstractClientSocket();

    virtual void connect() throw (SocketTimeoutException, SocketIOException) = 0;
    virtual void close() = 0;

    virtual void write(const char *buf, size_t buflen) throw (SocketTimeoutException, SocketIOException) = 0;
    virtual void write(const std::string &str) throw (SocketTimeoutException, SocketIOException) = 0;
    virtual size_t read(char *buf, size_t buflen) throw (SocketTimeoutException, SocketIOException) = 0;
    virtual std::string read() throw (SocketTimeoutException, SocketIOException) = 0;

    virtual bool connected() const = 0;

    virtual int descriptor() const = 0;

};

} /* namespace net */
} /* namespace nestor */

#endif /* ABSTRACT_CLIENT_SOCKET_H_ */
