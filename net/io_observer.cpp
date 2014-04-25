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
#include <sys/epoll.h>
#include <sstream>
#include <errno.h>
#include <cstring>
#include <unistd.h>
#include <iostream>

#include "logger.h"
#include "io_observer.h"

using namespace std;



namespace nestor {
namespace net {


IOObserver::IOObserver(unsigned int timeoutMs,
        std::function<void(void)> onTimeout) throw (std::runtime_error)
    : timeoutCallback_(onTimeout), timeoutMs_(timeoutMs) {
    efd_ = epoll_create(10);
    if (efd_ == -1) {
        ostringstream oss_err;
        oss_err << "IOObserver::IOObserver() epoll_create error: " << strerror(errno);
        throw runtime_error(oss_err.str());
    }

    items_.clear();
}

IOObserver::~IOObserver() {
    if (efd_ >= 0)
        close(efd_);
}

void IOObserver::append(int fd, callbackFunction readCallback,
        callbackFunction writeCallback,
        callbackFunction errorCallback) {
    epoll_event ev;
    bool mod;
    int res;

    if (fd < 0) return;

    memset(&ev, 0, sizeof(epoll_event));

    if (readCallback == nullptr && writeCallback == nullptr && errorCallback == nullptr) {
        NET_LOG_LVL(WARN, "No callbacks specified for fd: " << fd << ". Ignore appending.");
        return;
    }

    if (items_.count(fd)) {
        items_[fd].readCallback = readCallback;
        items_[fd].writeCallback = writeCallback;
        items_[fd].errorCallback = errorCallback;
        mod = true;
    } else {
        IOObserverCallbacks callbacks = {readCallback, writeCallback, errorCallback};
        items_.insert(make_pair(fd, callbacks));
        mod = false;
    }

    ev.events = EPOLLET;

    if (items_[fd].readCallback != nullptr)
        ev.events |= EPOLLIN;
    if (items_[fd].writeCallback != nullptr)
        ev.events |= EPOLLOUT;
    if (items_[fd].errorCallback != nullptr)
        ev.events |= EPOLLERR;
    ev.data.fd = fd;

    if (mod)
        res = epoll_ctl(efd_, EPOLL_CTL_MOD, fd, &ev);
    else
        res = epoll_ctl(efd_, EPOLL_CTL_ADD, fd, &ev);

    if (res == -1) {
        cout << "IOObserver::append(): epoll_ctl failed: " << strerror(errno) << endl;
        items_.erase(fd);
        return;
    }
}

void IOObserver::remove(int fd) {
    if (items_.count(fd) == 0)
        return;

    epoll_ctl(efd_, EPOLL_CTL_DEL, fd, nullptr);
    items_.erase(fd);
}

unsigned int IOObserver::timeout() const {
    return timeoutMs_;
}

void IOObserver::setTimeoutCallback(std::function<void(void)> callback) {
    timeoutCallback_ = callback;
}

int IOObserver::wait() {
    int effectiveTimeout = timeoutMs_ > 0 ? timeoutMs_ : -1;
    epoll_event *evntbuf;
    int nfds;

    if (items_.size() == 0) {
        return -1;
    }

    evntbuf = new epoll_event[items_.size()];
    nfds = epoll_wait(efd_, evntbuf, items_.size(), effectiveTimeout);


    if (nfds == -1) {
        cout << "IOObserver::wait(): epoll_wait failed: " << strerror(errno)
                << endl;
        delete[] evntbuf;
        return -1;
    }

    if (nfds == 0 && timeoutCallback_ != nullptr) {
        timeoutCallback_();
        delete[] evntbuf;
        return 0;
    }

    for (int i = 0; i < nfds; i++) {
        int fd = evntbuf[i].data.fd;
        if (!items_.count(fd)) {
            cout << "IOObserver::wait(): unknown fd " << fd << endl;
            continue;
        }

        if ((evntbuf[i].events & EPOLLIN)
                && items_[fd].readCallback != nullptr)
            items_[fd].readCallback();

        if ((evntbuf[i].events & EPOLLOUT)
                && items_[fd].writeCallback != nullptr)
            items_[fd].writeCallback();

        if ((evntbuf[i].events & EPOLLERR)
                && items_[fd].errorCallback != nullptr)
            items_[fd].errorCallback();
    }

    delete[] evntbuf;
    return nfds;
}

int IOObserver::itemsCount() const {
	return items_.size();
}

} /* namespace net */
} /* namespace nestor */
