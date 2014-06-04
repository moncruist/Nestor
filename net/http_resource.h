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

#ifndef HTTP_RESOURCE_H_
#define HTTP_RESOURCE_H_

#include <string>

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

    const std::string& server() const;
    void setServer(const std::string& server);
    const std::string& codeDefinition() const;
    void setCodeDefinition(const std::string& codeDefinition);
    const std::string& contentCharset() const;
    void setContentCharset(const std::string& contentCharset);
    const std::string& contentType() const;
    void setContentType(const std::string& contentType);
    unsigned char* content() const;
    void setContent(unsigned char* content);

private:
    std::string server_;
    std::string codeDefinition_;
    std::string contentType_;
    std::string contentCharset_;
    unsigned int code_;
    unsigned int contentLength_;

    unsigned char *content_;
};

} /* namespace net */
} /* namespace nestor */
#endif /* HTTP_RESOURCE_H_ */
