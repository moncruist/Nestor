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

#include <fstream>
#include <iostream>
#include <iomanip>
#include <string>
#include <ios>
#include <log4cplus/helpers/property.h>
#include <log4cplus/fileappender.h>
#include "logger.h"

using namespace std;
using namespace log4cplus;

void logger_init(const string &logFile) {
    log4cplus::initialize();

    BasicConfigurator config;
    config.configure();

    Logger log = Logger::getRoot();
    SharedAppenderPtr logout = log.getAppender(LOG4CPLUS_TEXT("STDOUT"));
    PatternLayout *layout = new PatternLayout(LOG4CPLUS_TEXT("%d{%d.%m.%Y %H:%M:%S:%q} %-5p [%T]: %m %n"));
    logout->setLayout(auto_ptr<Layout>(layout));

    FileAppender *fileAppender = new FileAppender(logFile, ios_base::app, false);
    fileAppender->setName(LOG4CPLUS_TEXT("FILEOUT"));
    PatternLayout *fileLayout = new PatternLayout(LOG4CPLUS_TEXT("%d{%d.%m.%Y %H:%M:%S:%q} %-5p [%T]: %m %n"));
    fileAppender->setLayout(auto_ptr<Layout>(fileLayout));
    log.addAppender(SharedAppenderPtr(fileAppender));
}

void logger_deinit(void) {
    Logger::shutdown();
}
