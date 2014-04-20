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

#include "imap/imap_session.h"
#include "imap_session_test.h"
#include "utils/string.h"
#include "net/socket_single.h"
#include <string>

using namespace std;
using namespace nestor::imap;
using namespace nestor::service;
using namespace nestor::net;

class DummySocket : public SocketSingle {
public:
    string writebuf;
    string readbuf;

public:

    void clearBufs() {
        writebuf.clear();
        readbuf.clear();
    }

    DummySocket() : SocketSingle("", 0, true) {}
    virtual ~DummySocket() { clearBufs(); }
    void connect() override {
        clearBufs();
    }
    void close() override {
        clearBufs();
    }

    void write(const char *buf, size_t buflen) override {
        string st(buf, buflen);
        write(st);
    }

    void write(const std::string &str) override {
        writebuf.append(str);
    }

    size_t read(char *buf, size_t buflen) override {
        size_t len = buflen > readbuf.size() ? readbuf.size() : buflen;
        copy(readbuf.begin(), readbuf.begin() + len, buf);
        readbuf.erase(readbuf.begin(), readbuf.begin() + len);
        return len;
    }

    std::string readAll() override {
        string cp = readbuf;
        readbuf.clear();
        return cp;
    }

    bool connected() const override {
        return true;
    }

    int descriptor() const override {
        return 0;
    }


};

void ImapSessionTest::setUp(void) {
    sock = new DummySocket();
    service = new Service();
    context = new ImapSession(service, sock);
    sock->clearBufs();
}

void ImapSessionTest::tearDown(void) {
    delete context;
}

void ImapSessionTest::testGreeting(void) {
    Service *service = new Service();
    DummySocket *sock = new DummySocket();
    nestor::imap::ImapSession session(service, sock);

    std::string answer = sock->writebuf;
    CPPUNIT_ASSERT(answer == "* OK IMAP4revl server ready" CRLF);
}

void ImapSessionTest::testCapabilityCommand(void) {
    string commandStr, expectedAnswer, actualAnswer;
    commandStr = "abcd1 CAPABILITY" CRLF;
    expectedAnswer = "* CAPABILITY IMAP4rev1 LITERAL+ AUTH=PLAIN" CRLF "abcd1 OK CAPABILITY completed" CRLF;

    sock->readbuf.append(commandStr);

    context->processData();
    actualAnswer = sock->writebuf;
    CPPUNIT_ASSERT_EQUAL(expectedAnswer, actualAnswer);
    sock->clearBufs();
}

void ImapSessionTest::testNoopCommand(void) {
    string commandStr, expectedAnswer, actualAnswer;
    commandStr = "abcd2 NOOP" CRLF;
    expectedAnswer = "abcd2 OK NOOP completed" CRLF;

    sock->readbuf.append(commandStr);

    context->processData();
    actualAnswer = sock->writebuf;
    CPPUNIT_ASSERT_EQUAL(expectedAnswer, actualAnswer);
    sock->clearBufs();
}

void ImapSessionTest::testLogoutCommand(void) {
    string commandStr, expectedAnswer, actualAnswer;
    commandStr = "abcd3 LOGOUT" CRLF;
    expectedAnswer = "* BYE IMAP4rev1 Server logging out" CRLF "abcd3 OK LOGOUT completed" CRLF;

    sock->readbuf.append(commandStr);

    context->processData();
    actualAnswer = sock->writebuf;
    CPPUNIT_ASSERT_EQUAL(expectedAnswer, actualAnswer);
    sock->clearBufs();
}
