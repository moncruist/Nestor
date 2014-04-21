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
#include <unicode/unistr.h>
#include <unicode/ustream.h>
#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <functional>

#include <algorithm>
#include "net/http_client.h"
#include "net/http_resource.h"
#include "net/socket_listener.h"
#include "net/io_observer.h"
#include "net/socket_single.h"
#include "imap/imap_session.h"
#include "service/service.h"

#include <unicode/ucnv.h>
#include <unicode/utypes.h>

#include "logger.h"

#include "rss/rss_xml_parser.h"
#include "rss/rss_channel.h"


using namespace std;
using namespace nestor::net;
using namespace nestor::rss;
using namespace nestor::imap;
using namespace nestor::service;
using namespace icu;

vector<ImapSession *> sessions;

void startNewConnection(SocketListener *listener, IOObserver *observer) {
	LOG_LVL("main", DEBUG, "Starting new conencttion");
	SocketSingle *con = listener->accept();
	ImapSession *session = new ImapSession(new Service(), con);
	observer->append(con->descriptor(), bind(&ImapSession::processData, session));
}

int main(int argc, char *argv[]) {
    ostringstream oss;
    SocketListener *listener;

    logger_init();
    LOG("main", "Starting Nestor server");

    IOObserver *observer = new IOObserver(10000 /* 10 secondes */, []() { LOG("main", "No data"); });

    try {
    	listener = new SocketListener("localhost", 143);
    	listener->startListen();
    } catch (SocketIOException &e) {
    	LOG_LVL("main", ERROR, "Cannot create listener: " << e.what());
    	return -1;
    }

    observer->append(listener->descriptor(), bind(startNewConnection, listener, observer));


    LOG("main", "Nestor started");
    while(observer->itemsCount() > 0) {
    	observer->wait();
    }

    delete observer;
    delete listener;

    return 0;
}
