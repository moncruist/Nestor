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
#ifndef SOCKETSINGLE_H_
#define SOCKETSINGLE_H_

#include <stdexcept>
#include <string>
#include "abstract_client_socket.h"

namespace nestor {
namespace net {

class SocketSingle: public AbstractClientSocket {
public:
    SocketSingle(std::string host = "localhost", unsigned short port = 80,
            bool manualConnect = false, unsigned int timeoutMs = 1000)
                    throw (SocketTimeoutException, SocketIOException);
    virtual void connect()
            throw (SocketTimeoutException, SocketIOException);
    virtual void close();

    virtual void write(const char *buf, size_t buflen)
            throw (SocketTimeoutException, SocketIOException);

    virtual void write(const std::string &str)
            throw (SocketTimeoutException, SocketIOException);

    virtual size_t read(char *buf, size_t buflen)
            throw (SocketTimeoutException, SocketIOException);
    virtual std::string read()
            throw (SocketTimeoutException, SocketIOException);

    virtual bool connected() const;
    virtual int descriptor() const;

    virtual ~SocketSingle();

private:
    int wait(bool waitRead = true, bool waitWrite = true);

private:
    std::string host_;
    unsigned short port_;
    int sockFd_;
    bool manualConnect_;
    bool connected_;
    unsigned int timeoutMs_;
};

} /* namespace net */
} /* namespace nestor */

#endif /* SOCKETSINGLE_H_ */
