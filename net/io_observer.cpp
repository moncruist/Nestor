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
#include <sstream>
#include <errno.h>
#include <cstring>
#include <unistd.h>
#include <iostream>
#include <stdexcept>
#include <algorithm>

#include "common/logger.h"
#include "io_observer.h"

using namespace std;



namespace nestor {
namespace net {


IOObserver::IOObserver()
        : loop_(nullptr) {
    eventCallbacks_.clear();
    eventObjects_.clear();
    eventTimers_.clear();
    eventTimeoutsMs_.clear();
    fdTimers_.clear();

    loop_ = new ev::dynamic_loop();
}

IOObserver::~IOObserver() {
    if (loop_) {
        breakLoop();
        delete loop_;
    }
}

void IOObserver::append(int fd, unsigned int timeoutMs,
                        callbackFunction readCallback,
                        callbackFunction writeCallback,
                        callbackFunction timeoutCallback) {
    ostringstream ossErr;

    if (fd < 0) {
        ossErr << "IOObserver::append: invalid fd = " << fd;
        NET_LOG_LVL(ERROR, ossErr.str());
        throw invalid_argument(ossErr.str());
    }

    if (readCallback == nullptr && writeCallback == nullptr) {
        ossErr << "IOObserver::append: read and write callbacks is not specified for fd: " << fd;
        NET_LOG_LVL(ERROR, ossErr.str());
        throw invalid_argument(ossErr.str());
    }

    if (eventCallbacks_.count(fd)) {
        ossErr << "IOObserver::append: fd " << fd << " is already observing";
        NET_LOG_LVL(ERROR, ossErr.str());
        throw invalid_argument(ossErr.str());
    }

    ev::io *eventObject = new ev::io(*loop_);
    int flags = 0;
    if (readCallback)
        flags |= ev::READ;
    if (writeCallback)
        flags |= ev::WRITE;
    eventObject->set(fd, flags);

    eventObject->set<IOObserver, &IOObserver::eventCallbackWrapper>(this);

    IOObserverCallbacks callbacks = {readCallback, writeCallback, timeoutCallback};
    eventCallbacks_.insert(make_pair(fd, callbacks));
    eventObjects_.insert(make_pair(eventObject, fd));
    eventObject->start();

    if (timeoutMs > 0) {
        if (timeoutCallback == nullptr) {
            NET_LOG_LVL(WARN, "IOObserver::append: timeoutMs was set, but timeoutCallback is nullptr."
                    " Skipping setting timeout for fd " << fd);
            return;
        }

        ev::timer *eventTimeout = new ev::timer(*loop_);
        ev_tstamp repeatSec = static_cast<ev_tstamp>(timeoutMs) / 1000.0;
        eventTimeout->set(0.0, repeatSec);

        eventTimers_.insert(make_pair(eventTimeout, fd));
        fdTimers_.insert(make_pair(fd, eventTimeout));
        eventTimeoutsMs_.insert(make_pair(fd, timeoutMs));
        eventTimeout->again();
    }
}

void IOObserver::remove(int fd) {
    ostringstream ossErr;
    if (eventCallbacks_.count(fd) == 0) {
        ossErr << "IOObserver::remove: fd " << fd << " wasn't appended before.";
        NET_LOG_LVL(ERROR, ossErr.str());
        throw invalid_argument(ossErr.str());
    }

    ev::io *eventObject = findObjectByFd(fd);
    ev::timer *eventTimer = fdTimers_.count(fd) > 0 ? fdTimers_[fd] : nullptr;

    if (eventObject == nullptr) {
        ossErr << "IOObserver::remove: cannot find event object for fd = " << fd;
        NET_LOG_LVL(ERROR, ossErr.str());
        throw runtime_error(ossErr.str());
    }

    eventObject->stop();
    eventObjects_.erase(eventObject);
    eventCallbacks_.erase(fd);

    delete eventObject;

    eventTimeoutsMs_.erase(fd);
    fdTimers_.erase(fd);

    if (eventTimer) {
        eventTimer->stop();
        eventTimers_.erase(eventTimer);

        delete eventTimer;
    }
}


void IOObserver::wait() {
    for (auto ev : eventObjects_)
        ev.first->start();
    for (auto tmr : eventTimers_)
        tmr.first->again();

    loop_->run();
}

void IOObserver::breakLoop() {
    if (loop_) {
        loop_->break_loop(ev::ALL);
        for (auto it = eventCallbacks_.begin(); it != eventCallbacks_.end(); it++)
            remove(it->first);
    }
}

int IOObserver::objectListenCount() const {
	return eventCallbacks_.size();
}

ev::io* IOObserver::findObjectByFd(int fd) {
    for (auto it = eventObjects_.begin(); it != eventObjects_.end(); it++)
        if (it->second == fd)
            return it->first;
    return nullptr;
}

void IOObserver::eventCallbackWrapper(ev::io& e, int revents) {
    ev::io *object = &e;
    if (eventObjects_.count(object) == 0) {
        NET_LOG_LVL(ERROR, "IOObserver::eventCallbackWrapper: cannot find fd for object = " << (void *)object);
        object->stop();
        delete object;
        return;
    }

    int fd = eventObjects_[object];
    NET_LOG("IOObserver::eventCallbackWrapper: event occured: fd = " << fd << "; revents = " << revents);
    if ((revents & ev::READ) && eventCallbacks_[fd].readCallback != nullptr) {
        NET_LOG("IOObserver::eventCallbackWrapper: calling read callback");
        auto callback = eventCallbacks_[fd].readCallback;
        callback(fd);
    }
    if ((revents & ev::WRITE) && eventCallbacks_[fd].writeCallback != nullptr)
        eventCallbacks_[fd].writeCallback(fd);

    if (fdTimers_.count(fd))
        fdTimers_[fd]->again();
}

void IOObserver::timeoutCallbackWrapper(ev::timer& t, int revents) {
    ev::timer *timer = &t;
    if (eventTimers_.count(timer) == 0) {
        NET_LOG_LVL(ERROR, "IOObserver::timeoutCallbackWrapper: cannot find fd for timer = " << (void *)timer);
        timer->stop();
        delete timer;
        return;
    }

    int fd = eventTimers_[timer];
    if ((revents & ev::TIMEOUT) && eventCallbacks_[fd].timeoutCallback != nullptr)
        eventCallbacks_[fd].timeoutCallback(fd);

    timer->again();
}


} /* namespace net */
} /* namespace nestor */


