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
#include <fstream>
#include <iomanip>
#include <log4cplus/helpers/property.h>
#include <log4cplus/asyncappender.h>
#include "common/logger.h"
#include "imap_session_test.h"
#include "imap_string_test.h"

using namespace std;
using namespace log4cplus;
using namespace nestor::common;

CPPUNIT_TEST_SUITE_REGISTRATION( ImapSessionTest );
CPPUNIT_TEST_SUITE_REGISTRATION( ImapStringTest );

void test_logger_init(void) {
    log4cplus::initialize();

    BasicConfigurator config;
    config.configure();

    Logger log = Logger::getRoot();
    log.setLogLevel(OFF_LOG_LEVEL);
    SharedAppenderPtr logout = log.getAppender(LOG4CPLUS_TEXT("STDOUT"));
    PatternLayout *layout = new PatternLayout(LOG4CPLUS_TEXT("%d{%d.%m.%Y %H:%M:%S:%q} %-5p [%T]: %m %n"));
    logout->setLayout(auto_ptr<Layout>(layout));
}

int main(int argc, char* argv[])
{
    test_logger_init();
    // informs test-listener about testresults
    CppUnit::TestResult testresult;

    // register listener for collecting the test-results
    CppUnit::TestResultCollector collectedresults;
    testresult.addListener (&collectedresults);

    // register listener for per-test progress output
    CppUnit::BriefTestProgressListener progress;
    testresult.addListener (&progress);

    // insert test-suite at test-runner by registry
    CppUnit::TestRunner testrunner;
    testrunner.addTest (CppUnit::TestFactoryRegistry::getRegistry().makeTest ());
    testrunner.run(testresult);

    // output results in compiler-format
    CppUnit::CompilerOutputter compileroutputter(&collectedresults, std::cout);
    compileroutputter.write ();

    // Output XML for Jenkins CPPunit plugin
//    ofstream xmlFileOut("cppTestBasicMathResults.xml");
//    CppUnit::XmlOutputter xmlOut(&collectedresults, xmlFileOut);
//    xmlOut.write();

    // return 0 if tests were successful
    return collectedresults.wasSuccessful() ? 0 : 1;
}
