#include "imap/imap_parser.h"
#include "imap_parser_test.h"
#include "utils/string.h"
#include <string>

using namespace std;
using namespace nestor::imap;

void ImapParserTest::setUp(void) {
}

void ImapParserTest::tearDown(void) {
}

void ImapParserTest::testParsingImapString(void) {

    string imapString = "{11}Hello World!!!";
    int idx = -1;

    string result = ImapParser::parseImapString(imapString, idx);
    
    CPPUNIT_ASSERT(idx == 14);
    CPPUNIT_ASSERT(result == "Hello World");
}
