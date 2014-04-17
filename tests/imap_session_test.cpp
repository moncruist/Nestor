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
#include <string>

using namespace std;
using namespace nestor::imap;
using namespace nestor::service;

void ImapSessionTest::setUp(void) {
    context = new ImapSession(service);
    context->getAnswers();
}

void ImapSessionTest::tearDown(void) {
    delete context;
}

void ImapSessionTest::testGreeting(void) {
    Service service;
    nestor::imap::ImapSession session(service);

    CPPUNIT_ASSERT(session.answersReady() == true);
    std::string answer = session.getAnswers();
    CPPUNIT_ASSERT(answer == "* OK IMAP4revl server ready" CRLF);
}

void ImapSessionTest::testCapabilityCommand(void) {
    string commandStr, expectedAnswer, actualAnswer;
    commandStr = "abcd1 CAPABILITY" CRLF;
    expectedAnswer = "* CAPABILITY IMAP4rev1 LITERAL+ AUTH=PLAIN" CRLF "abcd1 OK CAPABILITY completed" CRLF;

    context->processData(commandStr);
    CPPUNIT_ASSERT(context->answersReady());
    actualAnswer = context->getAnswers();
    CPPUNIT_ASSERT_EQUAL(expectedAnswer, actualAnswer);
}

void ImapSessionTest::testNoopCommand(void) {
    string commandStr, expectedAnswer, actualAnswer;
    commandStr = "abcd2 NOOP" CRLF;
    expectedAnswer = "abcd2 OK NOOP completed" CRLF;

    context->processData(commandStr);
    CPPUNIT_ASSERT(context->answersReady());
    actualAnswer = context->getAnswers();
    CPPUNIT_ASSERT_EQUAL(expectedAnswer, actualAnswer);
}

void ImapSessionTest::testLogoutCommand(void) {
    string commandStr, expectedAnswer, actualAnswer;
    commandStr = "abcd3 LOGOUT" CRLF;
    expectedAnswer = "* BYE IMAP4rev1 Server logging out" CRLF "abcd3 OK LOGOUT completed" CRLF;

    context->processData(commandStr);
    CPPUNIT_ASSERT(context->answersReady());
    actualAnswer = context->getAnswers();
    CPPUNIT_ASSERT_EQUAL(expectedAnswer, actualAnswer);
}
