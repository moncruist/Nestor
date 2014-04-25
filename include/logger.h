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
#ifndef LOGGER_H_
#define LOGGER_H_

#include <log4cplus/logger.h>
#include <log4cplus/loggingmacros.h>
#include <log4cplus/configurator.h>
#include <iomanip>

void logger_init(void);


#define LOG_LVL(name, lvl, txt) do { \
    log4cplus::Logger logger = log4cplus::Logger::getInstance(LOG4CPLUS_TEXT(name));    \
    LOG4CPLUS_##lvl(logger, txt); \
} while(0)

#define LOG(name, txt) LOG_LVL(name, INFO, txt)

#define MAIN_LOGGER_NAME    "main"
#define IMAP_LOGGER_NAME    "imap"
#define NET_LOGGER_NAME     "net"
#define SERVICE_LOGGER_NAME "service"

#define MAIN_LOG_LVL(lvl, txt) LOG_LVL(MAIN_LOGGER_NAME, lvl, txt)
#define MAIN_LOG(txt) LOG(MAIN_LOGGER_NAME, txt)


#define IMAP_LOG_LVL(lvl, txt) LOG_LVL(IMAP_LOGGER_NAME, lvl, txt)
#define IMAP_LOG(txt) LOG(IMAP_LOGGER_NAME, txt)

#define NET_LOG_LVL(lvl, txt) LOG_LVL(NET_LOGGER_NAME, lvl, txt)
#define NET_LOG(txt) LOG(NET_LOGGER_NAME, txt)

#define SERVICE_LOG_LVL(lvl, txt) LOG_LVL(SERVICE_LOGGER_NAME, lvl, txt)
#define SERVICE_LOG(txt) LOG(SERVICE_LOGGER_NAME, txt)

#endif /* LOGGER_H_ */
