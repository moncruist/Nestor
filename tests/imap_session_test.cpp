#include "imap/imap_session.h"
#include "imap_session_test.h"
#include "utils/string.h"
#include <string>


void ImapSessionTest::setUp(void) {
}

void ImapSessionTest::tearDown(void) {
}

void ImapSessionTest::testGreeting(void) {
    nestor::imap::ImapSession session;

    CPPUNIT_ASSERT(session.answersReady() == true);
    std::string answer = session.getAnswers();
    CPPUNIT_ASSERT(answer == "* OK IMAP4revl server ready" CRLF);
}
