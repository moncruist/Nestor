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

#ifndef HTTP_RESOURCE_H_
#define HTTP_RESOURCE_H_

#include <unicode/unistr.h>

namespace nestor {
namespace net {

class HttpResource {
public:
    HttpResource();
    virtual ~HttpResource();

    unsigned int code() const;
    void setCode(unsigned int code);
    unsigned int contentLength() const;
    void setContentLength(unsigned int contentLength);
    const icu::UnicodeString& encoding() const;
    void setEncoding(const icu::UnicodeString& encoding);
    const icu::UnicodeString& server() const;
    void setServer(const icu::UnicodeString& server);
    const icu::UnicodeString& codeDefinition() const;
    void setCodeDefinition(const icu::UnicodeString& codeDefinition);

private:
    icu::UnicodeString encoding_;
    icu::UnicodeString server_;
    icu::UnicodeString codeDefinition_;
    unsigned int code_;
    unsigned int contentLength_;
};

} /* namespace net */
} /* namespace nestor */
#endif /* HTTP_RESOURCE_H_ */
