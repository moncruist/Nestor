#include "imap/imap_string.h"
#include "imap_string_test.h"
#include "utils/string.h"
#include <string>
#include <iostream>

using namespace std;
using namespace nestor::imap;

void ImapStringTest::setUp(void) {
}

void ImapStringTest::tearDown(void) {
}

void ImapStringTest::testParsingImapString(void) {

    string imapStringRaw = "{11}" CRLF "Hello World!!!";
    int idx = -1;

    ImapString imapString;
    idx = imapString.addBufferToParse(imapStringRaw);

    string result = imapString.data();
    CPPUNIT_ASSERT(idx == imapStringRaw.length() - 3);
    CPPUNIT_ASSERT(result == "Hello World");
}
