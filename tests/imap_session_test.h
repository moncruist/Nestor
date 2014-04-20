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

#ifndef IMAP_SESSION_TEST_H_
#define IMAP_SESSION_TEST_H_

#include <iostream>
#include <cppunit/TestCase.h>
#include <cppunit/TestFixture.h>
#include <cppunit/TestSuite.h>
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/TestRunner.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/XmlOutputter.h>
#include "service/service.h"
#include "imap/imap_session.h"

class DummySocket;

class ImapSessionTest : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE (ImapSessionTest);
    CPPUNIT_TEST(testGreeting);
    CPPUNIT_TEST(testCapabilityCommand);
    CPPUNIT_TEST(testNoopCommand);
    CPPUNIT_TEST(testLogoutCommand);
    CPPUNIT_TEST_SUITE_END();

public:
    void setUp(void);
    void tearDown(void);

protected:
    void testGreeting(void);
    void testCapabilityCommand(void);
    void testNoopCommand(void);
    void testLogoutCommand(void);

private:
    DummySocket *sock;
    nestor::imap::ImapSession *context;
    nestor::service::Service *service;
};

#endif /* IMAP_SESSION_TEST_H_ */
