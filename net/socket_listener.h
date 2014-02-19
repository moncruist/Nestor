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
#ifndef SOCKET_LISTENER_H_
#define SOCKET_LISTENER_H_

#include <string>
#include "socket_single.h"

namespace nestor {
namespace net {

/**
 * TCP listener class. Binds to IP and ports and listens.
 * Unlike SocketSingle does not use own select for waiting connections.
 * Always nonblock.
 */
class SocketListener {
public:
    SocketListener(std::string host = "localhost", unsigned short port = 80)
            throw (SocketIOException);
    virtual ~SocketListener();

    void startListen();

    /**
     * Accept new incoming connection.
     * @return connected socket or nullptr if there are no connections.
     */
    SocketSingle *accept();
    void close();

    int descriptor() const;

    const std::string& host() const;
    void setHost(const std::string& host);
    unsigned short port() const;
    void setPort(unsigned short port);

private:
    std::string host_;
    unsigned short port_;
    int sfd_;
    bool started_;
};

} /* namespace net */
} /* namespace nestor */

#endif /* SOCKET_LISTENER_H_ */
