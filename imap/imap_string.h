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

#ifndef IMAP_STRING_H_
#define IMAP_STRING_H_

#include <string>
#include <exception>
#include <stdexcept>

namespace nestor {
namespace imap {


enum class ImapStringType {
    UNSPECIFIED, // when we haven't determine string type yet
    QUOTED,      // quoted string type, i.e. "Hello"
    LITERAL,      // literal string type i.e. {5}CRLFHello
    LITERAL_NONSYNC // literal nonsynchronized string i.e. {5+}CRLFHello
};

enum class ImapStringStatus {
    INVALID,
    UNCOMPLETED,
    COMPLETED
};

class ImapString {
public:
    ImapString();
    virtual ~ImapString();

    void reset();
    int addBufferToParse(std::string &buffer);
    ImapStringStatus status() const;

    void appendString(std::string str);
    std::string data() const;

    ImapStringType type() const;
    void setType(ImapStringType type);

    std::string toRawData();
    std::string toRawData(ImapStringType exportType);

    unsigned int length() const;
    unsigned int parsedLength() const;

private:
    static ImapStringType detectType(const std::string &buffer, int *pos, int *stringLength);

private:
    ImapStringStatus status_;
    ImapStringType type_;

    unsigned int length_;
    unsigned int parsedLength_;
    std::string data_;

    bool prefixStarted_;
    bool prefixEnded_;
    bool literalCrLfParsed_;
};

} /* namespace imap */
} /* namespace nestor */

#endif /* IMAP_STRING_H_ */
