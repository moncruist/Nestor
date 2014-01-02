/*
 *  This file is part of Nestor.
 *
 *  Nestor - program for aggregation RSS subscriptions providing
 *  access via IMAP interface.
 *  Copyright (C) 2013  Konstantin Zhukov
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
#include <unicode/unistr.h>
#include "http_resource.h"

using namespace icu;

namespace nestor {
namespace net {

HttpResource::HttpResource() :
    encoding_(UNICODE_STRING_SIMPLE("")),
    server_(UNICODE_STRING_SIMPLE("")),
    codeDefinition_(UNICODE_STRING_SIMPLE("")),
    code_(0), contentLength_(0){

}

HttpResource::~HttpResource() {
    // TODO Auto-generated destructor stub
}

unsigned int HttpResource::code() const {
    return code_;
}

void HttpResource::setCode(unsigned int code) {
    code_ = code;
}

unsigned int HttpResource::contentLength() const {
    return contentLength_;
}

void HttpResource::setContentLength(unsigned int contentLength) {
    contentLength_ = contentLength;
}

const icu::UnicodeString& HttpResource::encoding() const {
    return encoding_;
}

void HttpResource::setEncoding(const icu::UnicodeString& encoding) {
    encoding_ = encoding;
}

const icu::UnicodeString& HttpResource::server() const {
    return server_;
}

void HttpResource::setServer(const icu::UnicodeString& server) {
    server_ = server;
}

const icu::UnicodeString& HttpResource::codeDefinition() const {
    return codeDefinition_;
}

void HttpResource::setCodeDefinition(const icu::UnicodeString& codeDefinition) {
    codeDefinition_ = codeDefinition;
}

} /* namespace net */
} /* namespace nestor */
