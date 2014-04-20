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

namespace nestor {
namespace net {

class SocketTimeoutException : public std::runtime_error {
    using std::runtime_error::runtime_error;
};

class SocketIOException : public std::runtime_error {
    using std::runtime_error::runtime_error;
};


class SocketSingle {
public:
    SocketSingle(std::string host = "localhost", unsigned short port = 80,
            bool manualConnect = false, unsigned int timeoutMs = 1000,
            bool nonblocking = true);

    SocketSingle(int fd, unsigned int timeoutMs = 1000,
            bool nonblocking = true);

    virtual void connect();
    virtual void close();

    virtual void write(const char *buf, size_t buflen);

    virtual void write(const std::string &str);

    virtual size_t read(char *buf, size_t buflen);
    virtual std::string readAll();

    virtual bool connected() const;
    virtual int descriptor() const;

    unsigned int timeout() const;
    void setTimeout(unsigned int timeoutMs);

    void setNonBlocking(bool nonblocking);

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
    bool nonblocking_;
};

} /* namespace net */
} /* namespace nestor */

#endif /* SOCKETSINGLE_H_ */
