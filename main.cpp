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

#include <signal.h>
#include <cstring>
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

#include "service/configuration.h"

using namespace std;
using namespace nestor::net;
using namespace nestor::rss;
using namespace nestor::imap;
using namespace nestor::service;
using namespace icu;

static vector<ImapSession *> closedSessions;
static SocketListener *listener;
static IOObserver *observer;

void startNewConnection(SocketListener *listener, IOObserver *observer) {
	MAIN_LOG("Starting new conencttion");
	SocketSingle *con = listener->accept();
	ImapSession *session = new ImapSession(new Service(), con);
	auto onRead = [session, con]() {
	    MAIN_LOG("fd " << con->descriptor()   << "Ready for reading");
	    session->processData();
	};

	auto onWrite = [session, con]() {
	    MAIN_LOG("fd " << con->descriptor()   << "Ready for writing");
        session->writeAnswers();
    };
	observer->append(con->descriptor(), onRead, onWrite);
	con->setOnCloseCallback([observer](SocketSingle *s) {
	    observer->remove(s->descriptor());
	});

	session->setOnExitCallback([](ImapSession *s) { closedSessions.push_back(s); });
}

void onSigInt(int sig) {
    if (listener && observer) {
        MAIN_LOG("Closing listener");

        observer->remove(listener->descriptor());
        listener->close();
    }
}

int main(int argc, char *argv[]) {
    ostringstream oss;


    Configuration *config = Configuration::instance();

    config->load();
    config->store();

    logger_init(config->logFile());

    MAIN_LOG("Starting Nestor server");


    try {
    	listener = new SocketListener("localhost", 1430);
    	listener->startListen();
    } catch (SocketIOException &e) {
    	MAIN_LOG_LVL(ERROR, "Cannot create listener: " << e.what());
    	return -1;
    }

    observer = new IOObserver(10000 /* 10 seconds */, []() { MAIN_LOG("No data"); });

    observer->append(listener->descriptor(), bind(startNewConnection, listener, observer));

    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = onSigInt;
    sigaction(SIGINT, &sa, NULL);

    MAIN_LOG("Nestor started");
    while(observer->itemsCount() > 0) {
    	observer->wait();

    	if (closedSessions.size() > 0) {
    	    for (ImapSession *s : closedSessions)
    	        delete s;
    	    closedSessions.clear();
    	}
    }

    MAIN_LOG("Nestor finished");

    delete observer;
    delete listener;

    logger_deinit();
    return 0;
}
