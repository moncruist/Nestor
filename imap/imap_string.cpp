#include <string>
#include <sstream>
#include "utils/string.h"
#include "imap_string.h"

using namespace std;

namespace nestor {
namespace imap {

ImapString::ImapString() {
    reset();
}

ImapString::~ImapString() {
}

void ImapString::reset() {
    status_ = ImapStringStatus::UNCOMPLETED;
    type_ = ImapStringType::UNSPECIFIED;
    length_ = parsedLength_ = 0;
    prefixStarted_ = prefixEnded_ = literalCrLfParsed_ = false;

    data_.clear();
}


/**
 * Tries to detect type of imap string in buffer. If it detects literal string,
 * also tries to parse string length. Otherwise error returns.
 * On quoted string only search for openning quote.
 * @param buffer[in] string which containes imap string
 * @param pos[out] If non null stores last valid position. In case of literal
 * string here will be stored position of closing brace. In case quoted here
 * will stored openning quote. -1 on error.
 * @param stringLength[out] If non null stores string length of literal string.
 * If string is quoted or was error here will be stored -1.
 * @return Returns type of imap string. On error return UNSPECIFIED
 */
ImapStringType ImapString::detectType(const std::string &buffer, int *pos, int *stringLength) {
    size_t literalPos, quotePos;
    literalPos = buffer.find("{", 0);
    quotePos = buffer.find("\"", 0);

    if (literalPos == string::npos && quotePos == string::npos) {
        if (pos) *pos = -1;
        if (stringLength) *stringLength = -1;
        return ImapStringType::UNSPECIFIED;
    }

    if (literalPos < quotePos) {
        // Try to parse literal prefix
        size_t literalEndPos = buffer.find('}', literalPos);

        if (literalEndPos == string::npos || (literalEndPos - literalPos) == 1) {
            if (pos) *pos = -1;
            if (stringLength) *stringLength = -1;
            return ImapStringType::UNSPECIFIED;
        }

        string length = buffer.substr(literalPos + 1, literalEndPos - literalPos - 1);
        char *tail;
        *stringLength = strtol(length.c_str(), &tail, 10);
        if (*tail != '\0') {
            if (pos) *pos = -1;
            if (stringLength) *stringLength = -1;
            return ImapStringType::UNSPECIFIED;
        }

        if (pos) *pos = literalEndPos;
        return ImapStringType::LITERAL;
    } else {
        // quoted string, return position of start quote
        if (pos) *pos = quotePos;
        if (stringLength) *stringLength = -1; // We don't know string length here
        return ImapStringType::QUOTED;
    }
}


int ImapString::addBufferToParse(std::string &buffer) {
    int pos, endQuotePos, stringLength;
    pos = stringLength = -1;

    if (status_ == ImapStringStatus::COMPLETED)
        return 0;

    if (type_ == ImapStringType::UNSPECIFIED) {
        type_ = detectType(buffer, &pos, &stringLength);
        if (type_ == ImapStringType::UNSPECIFIED)
            return -1;
        if (stringLength != -1)
            parsedLength_ = stringLength;
        length_ = 0;
    }

    if (pos == -1)
        pos = 0; // Reset pos in case of continuenig parsing
    else
        pos++; // If pos points to quote or literal end,
               // shift pos to the first string character
    
    switch (type_) {
    case ImapStringType::LITERAL_NONSYNC:
    case ImapStringType::LITERAL:
        // Checking if we already parsed CRLF
        if (!literalCrLfParsed_) {
            //Parsing CRLF
            if (pos < buffer.length() - 1 && buffer.substr(pos, 2).compare(CRLF) == 0) {
                pos += 2;
                literalCrLfParsed_ = true;
            }
            status_ = ImapStringStatus::UNCOMPLETED;
        }
        if (literalCrLfParsed_ && pos < buffer.length()) {
            // Parsing rest of the string
            int expectedLength = parsedLength_ - length_;
            if (expectedLength > buffer.length() - pos)
                expectedLength = buffer.length() - pos;
            data_.append(buffer.substr(pos, expectedLength));
            length_ += expectedLength;
            pos += expectedLength;
            if (length_ == parsedLength_) {
                status_ = ImapStringStatus::COMPLETED;
                return pos;
            }
        }
        break;

    case ImapStringType::QUOTED:
        // Here we only need to find the closing quote.
        // Until them any character is part of the imap string.
        endQuotePos = buffer.find("\"", pos);
        if (endQuotePos == string::npos) {
            // We don't find closing quote. Appending tail of
            // the buffer to out already parsed data.
            status_ = ImapStringStatus::UNCOMPLETED;
            data_.append(buffer.substr(pos, string::npos));
            length_ += buffer.length() - pos;
            parsedLength_ = length_;
            return buffer.length() - 1;
        } else {
            // Here we finally have found the closing quote.
            status_ = ImapStringStatus::COMPLETED;
            data_.append(buffer.substr(pos, endQuotePos - pos));
            length_ += endQuotePos - pos;
            parsedLength_ = length_;
            return endQuotePos;
        }
        break;

    // For warning suppression
    default:
        break;
    }
    
    return -1; // We shouldn't go here
}


ImapStringStatus ImapString::status() const {
    return status_;
}


void ImapString::appendString(std::string str) {
    data_.append(str);
}


ImapStringType ImapString::type() const {
    return type_;
}


void ImapString::setType(ImapStringType type) {
    type_ = type;
}

std::string ImapString::toRawData() {
    if (type_ == ImapStringType::UNSPECIFIED)
        return toRawData(ImapStringType::LITERAL);
    return toRawData(type_);
    
}

std::string ImapString::toRawData(ImapStringType exportType) {
    ostringstream formatter;
    if (exportType == ImapStringType::UNSPECIFIED)
        return "";

    if (exportType == ImapStringType::LITERAL ||
        exportType == ImapStringType::LITERAL_NONSYNC) {
        formatter << "{" << length_;
        if (exportType == ImapStringType::LITERAL_NONSYNC)
            formatter << "+";
        formatter << CRLF;
        formatter << data_;
    } else {
        formatter << "\"" << data_ << "\"";
    }

    return formatter.str();
}

std::string ImapString::data() const {
    return data_;
}

unsigned int ImapString::length() const {
    return length_;
}


unsigned int ImapString::parsedLength() const {
    return parsedLength_;
}

} /* namespace imap */
} /* namespace nestor */

