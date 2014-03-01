#ifndef IMAP_PARSER_TEST_H_
#define IMAP_PARSER_TEST_H_

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

class ImapParserTest : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE (ImapParserTest);
    CPPUNIT_TEST(testParsingImapString);
    CPPUNIT_TEST_SUITE_END();

public:
    void setUp(void);
    void tearDown(void);

protected:
    void testParsingImapString(void);
};

#endif /* IMAP_PARSER_TEST_H_ */
