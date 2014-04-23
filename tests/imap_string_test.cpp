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
    CPPUNIT_ASSERT(static_cast<size_t>(idx) == imapStringRaw.length() - 3);
    CPPUNIT_ASSERT(imapString.type() == ImapStringType::LITERAL);
    CPPUNIT_ASSERT(result == "Hello World");
    CPPUNIT_ASSERT(imapString.status() == ImapStringStatus::COMPLETED);
}

void ImapStringTest::testQuotedEmptyString(void) {
	int idx;
	string imapStringRaw = "";

	ImapString imapString;
	idx = imapString.addBufferToParse(imapStringRaw);

	CPPUNIT_ASSERT(idx == -1);
	CPPUNIT_ASSERT(imapString.type() == ImapStringType::UNSPECIFIED);
	CPPUNIT_ASSERT(imapString.status() == ImapStringStatus::INVALID);
}

void ImapStringTest::testQuotedSimpleString(void) {
	int idx;
	string imapStringRaw = "\"quoted\"";

	ImapString imapString;
	idx = imapString.addBufferToParse(imapStringRaw);

	string result = imapString.data();
	CPPUNIT_ASSERT(static_cast<size_t>(idx) == imapStringRaw.length() - 1);
	CPPUNIT_ASSERT(imapString.type() == ImapStringType::QUOTED);
	CPPUNIT_ASSERT(imapString.status() == ImapStringStatus::COMPLETED);
	CPPUNIT_ASSERT(result == "quoted");
}

void ImapStringTest::testQuotedUncompletedString(void) {
	int idx;
	string imapStringRaw = "\"quoted";

	ImapString imapString;
	idx = imapString.addBufferToParse(imapStringRaw);


	CPPUNIT_ASSERT(static_cast<size_t>(idx) == imapStringRaw.length() - 1);
	CPPUNIT_ASSERT(imapString.type() == ImapStringType::QUOTED);
	CPPUNIT_ASSERT(imapString.status() == ImapStringStatus::UNCOMPLETED);

	imapStringRaw = " finish\" aaa";
	idx = imapString.addBufferToParse(imapStringRaw);

	string result = imapString.data();
	CPPUNIT_ASSERT(idx == 7);
	CPPUNIT_ASSERT(imapString.type() == ImapStringType::QUOTED);
	CPPUNIT_ASSERT(imapString.status() == ImapStringStatus::COMPLETED);
	CPPUNIT_ASSERT(result == "quoted finish");
}

void ImapStringTest::testLiteralNonsyncString(void) {
	string imapStringRaw = "{11+}" CRLF "Hello World!!!";
	int idx = -1;

	ImapString imapString;
	idx = imapString.addBufferToParse(imapStringRaw);

	string result = imapString.data();
	CPPUNIT_ASSERT_EQUAL(static_cast<int>(imapStringRaw.length() - 3), idx);
	CPPUNIT_ASSERT(imapString.type() == ImapStringType::LITERAL_NONSYNC);
	CPPUNIT_ASSERT(result == "Hello World");
	CPPUNIT_ASSERT(imapString.status() == ImapStringStatus::COMPLETED);
}
