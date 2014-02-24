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
#include <string>
#include "http_resource.h"

using namespace std;
namespace nestor {
namespace net {

HttpResource::HttpResource() :
    encoding_(""),
    server_(""),
    codeDefinition_(""),
    contentType_(""),
    contentCharset_(""),
    code_(0), contentLength_(0),
    content_(nullptr) {

}

HttpResource::~HttpResource() {
    if (content_) {
        delete[] content_;
    }
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

const string& HttpResource::encoding() const {
    return encoding_;
}

void HttpResource::setEncoding(const string& encoding) {
    encoding_ = encoding;
}

const string& HttpResource::server() const {
    return server_;
}

void HttpResource::setServer(const string& server) {
    server_ = server;
}

const string& HttpResource::codeDefinition() const {
    return codeDefinition_;
}

void HttpResource::setCodeDefinition(const string& codeDefinition) {
    codeDefinition_ = codeDefinition;
}

const std::string& HttpResource::contentCharset() const {
    return contentCharset_;
}

void HttpResource::setContentCharset(const std::string& contentCharset) {
    contentCharset_ = contentCharset;
}

const std::string& HttpResource::contentType() const {
    return contentType_;
}

void HttpResource::setContentType(const std::string& contentType) {
    contentType_ = contentType;
}

unsigned char* HttpResource::content() const {
    return content_;
}

void HttpResource::setContent(unsigned char* content) {
    content_ = content;
}

} /* namespace net */
} /* namespace nestor */


