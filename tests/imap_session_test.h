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

class ImapSessionTest : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE (ImapSessionTest);
    CPPUNIT_TEST(testGreeting);
    CPPUNIT_TEST_SUITE_END();

public:
    void setUp(void);
    void tearDown(void);

protected:
    void testGreeting(void);
};

#endif /* IMAP_SESSION_TEST_H_ */
