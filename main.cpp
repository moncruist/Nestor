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

vector<ImapSession *> closedSessions;

void startNewConnection(SocketListener *listener, IOObserver *observer) {
	LOG("main", "Starting new conencttion");
	SocketSingle *con = listener->accept();
	ImapSession *session = new ImapSession(new Service(), con);
	auto onRead = [session, con]() {
	    LOG("main", "fd " << con->descriptor()   << "Ready for reading");
	    session->processData();
	};

	auto onWrite = [session, con]() {
        LOG("main", "fd " << con->descriptor()   << "Ready for writing");
        session->writeAnswers();
    };
	observer->append(con->descriptor(), onRead, onWrite);
	con->setOnCloseCallback([observer](SocketSingle *s) {
	    observer->remove(s->descriptor());
	});

	session->setOnExitCallback([](ImapSession *s) { closedSessions.push_back(s); });
}

int main(int argc, char *argv[]) {
    ostringstream oss;
    SocketListener *listener;

    logger_init();
    LOG("main", "Starting Nestor server");



    try {
    	listener = new SocketListener("localhost", 1430);
    	listener->startListen();
    } catch (SocketIOException &e) {
    	LOG_LVL("main", ERROR, "Cannot create listener: " << e.what());
    	return -1;
    }

    IOObserver *observer = new IOObserver(10000 /* 10 seconds */, []() { LOG("main", "No data"); });

    observer->append(listener->descriptor(), bind(startNewConnection, listener, observer));


    LOG("main", "Nestor started");
    while(observer->itemsCount() > 0) {
    	observer->wait();

    	if (closedSessions.size() > 0) {
    	    for (ImapSession *s : closedSessions)
    	        delete s;
    	    closedSessions.clear();
    	}
    }

    delete observer;
    delete listener;

    return 0;
}
