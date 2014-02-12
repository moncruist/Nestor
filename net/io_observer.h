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
#ifndef IO_OBSERVER_H_
#define IO_OBSERVER_H_

#include <map>
#include <functional>
#include <stdexcept>

namespace nestor {
namespace net {

struct IOObserverCallbacks {
    std::function<void (void)> readCallback;
    std::function<void (void)> writeCallback;
    std::function<void (void)> errorCallback;
};

class IOObserver {
public:
    IOObserver(unsigned int timeoutMs = 1000,
            std::function<void(void)> onTimeout = nullptr) throw (std::runtime_error);
    virtual ~IOObserver();

    void append(int fd, std::function<void (void)> readCallback = nullptr,
            std::function<void (void)> writeCallback = nullptr,
            std::function<void (void)> errorCallback = nullptr);
    void remove(int fd);

    unsigned int timeout() const;

    void setTimeoutCallback(std::function<void(void)> callback);

    void wait();

private:
    std::function<void (void)> timeoutCallback_;
    std::map<int, IOObserverCallbacks> items_;

    unsigned int timeoutMs_;
    int efd_;
};

} /* namespace net */
} /* namespace nestor */

#endif /* IO_OBSERVER_H_ */
