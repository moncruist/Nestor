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
#include "service/channels_update_worker.h"

#include <unicode/ucnv.h>
#include <unicode/utypes.h>

#include "common/logger.h"

#include "rss/rss_xml_parser.h"
#include "rss/rss_channel.h"

#include "service/configuration.h"

using namespace std;
using namespace nestor::net;
using namespace nestor::rss;
using namespace nestor::imap;
using namespace nestor::service;
using namespace nestor::common;
using namespace icu;

static vector<ImapSession *> closedSessions;
static SocketListener *listener;
static IOObserver *observer;
static SqliteConnection *connection;

void startNewConnection(SocketListener *listener, IOObserver *observer) {
	MAIN_LOG("Starting new conencttion");
	SocketSingle *con = listener->accept();
	ImapSession *session = new ImapSession(new Service(connection), con);
	auto onRead = [session, con](int fd) {
	    MAIN_LOG("fd " << fd   << "Ready for reading");
	    session->processData();
	    session->writeAnswers();
	};

	observer->append(con->descriptor(), 0, onRead, nullptr, nullptr);
	con->setOnCloseCallback([observer](SocketSingle *s) {
	    observer->remove(s->descriptor());
	});

	session->setOnExitCallback([](ImapSession *s) { closedSessions.push_back(s); });
	session->writeAnswers();
}

void onSigInt(int sig) {
    if (listener && observer) {
        MAIN_LOG("Closing listener");

        observer->remove(listener->descriptor());
        listener->close();
        observer->breakLoop();
    }
}

void checkDatabase(SqliteConnection *connection) {
    MAIN_LOG("Checking Nestor database");
    SqliteProvider prov(connection);
    prov.createUsersTable();
    prov.createChannelsTable();
    prov.createPostsTable();
    prov.createSubsriptionTable();
}

void testWorker(SqliteConnection *connection) {
    MAIN_LOG("Testing ChannelsUpdateWorker");
    SqliteProvider prov(connection);

    Channel *channel = prov.findChannelByRssLink("http://lenta.ru/rss");
    MAIN_LOG("Not found. Creating one");
    if (channel == nullptr) {
        channel = new Channel();
        channel->setRssLink("http://lenta.ru/rss");
        channel->setDescription("http://lenta.ru/rss");
        channel->setLink("http://lenta.ru/rss");
        channel->setTitle("http://lenta.ru/rss");
        channel->setUpdateInterval(3600);
        time_t t = time(nullptr);
        tm *now = localtime(&t);
        channel->setLastUpdate(*now);

        channel->setId(prov.insertChannel(*channel));
    }

    ChannelsUpdateWorker worker({channel->id()}, connection);
    worker.run();
}


int main(int argc, char *argv[]) {
    ostringstream oss;


    Configuration *config = Configuration::instance();

    config->load();
    config->store();

    logger_init(config->logFile());
    connection = new SqliteConnection(config->sqliteConfig().databasePath());
    connection->open();

    checkDatabase(connection);
    testWorker(connection);

    HttpClient client;
    HttpResource *res = client.getResource("http://lenta.ru/rss");
    cout << "Resource content-length: "  << res->contentLength() << " strlen: " << strlen(reinterpret_cast<char *>(res->content()))
            << " " << res->contentLength() + 1 << " byte: " << (int)res->content()[res->contentLength()]<< endl;

    MAIN_LOG("Starting Nestor server");


    try {
    	listener = new SocketListener("localhost", 1430);
    	listener->startListen();
    } catch (SocketIOException &e) {
    	MAIN_LOG_LVL(ERROR, "Cannot create listener: " << e.what());
    	return -1;
    }

    observer = new IOObserver();

    observer->append(listener->descriptor(), 0, bind(startNewConnection, listener, observer), nullptr, nullptr);

    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = onSigInt;
    sigaction(SIGINT, &sa, NULL);

    MAIN_LOG("Nestor started");
    while(observer->objectListenCount() > 0) {
    	observer->wait();

    	if (closedSessions.size() > 0) {
    	    for (ImapSession *s : closedSessions)
    	        delete s;
    	    closedSessions.clear();
    	}
    }

    MAIN_LOG("Nestor finished");

    connection->close();
    logger_deinit();

    delete observer;
    delete listener;
    delete connection;

    return 0;
}
