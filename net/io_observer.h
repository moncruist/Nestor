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
#ifndef IO_OBSERVER_H_
#define IO_OBSERVER_H_

#include <map>
#include <functional>
#include <stdexcept>

#include <ev++.h>

namespace nestor {
namespace net {

/**
 * IOObserver class is used for waiting events on specified descritptors.
 * When event occurs, one of the callbacks (read, write or error callback) is
 * called. Uses epoll for events monitoring.
 */
class IOObserver {
public:
    typedef std::function<void(int)> callbackFunction;
    IOObserver();
    virtual ~IOObserver();

    void append(int fd, unsigned int timeoutMs,
            callbackFunction readCallback,
            callbackFunction writeCallback,
            callbackFunction timeoutCallback);
    void remove(int fd);

    /**
     * Blocks execution for waiting some events
     */
    void wait();

    void breakLoop();

    int objectListenCount() const;

private:
    void eventCallbackWrapper(ev::io &e, int revents);
    void timeoutCallbackWrapper(ev::timer &t, int revents);

    ev::io *findObjectByFd(int fd);

private:
    struct IOObserverCallbacks {
        callbackFunction readCallback;
        callbackFunction writeCallback;
        callbackFunction timeoutCallback;
    };

    std::map<int, IOObserverCallbacks> eventCallbacks_;
    std::map<ev::io *, int> eventObjects_;
    std::map<ev::timer *, int> eventTimers_;
    std::map<int, ev::timer *> fdTimers_;
    std::map<int, unsigned int> eventTimeoutsMs_;

    ev::dynamic_loop *loop_;
};

} /* namespace net */
} /* namespace nestor */

#endif /* IO_OBSERVER_H_ */
