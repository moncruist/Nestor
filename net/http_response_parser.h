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
#ifndef HTTP_RESPONCE_PARSER_H_
#define HTTP_RESPONCE_PARSER_H_

#include <string>
#include <istream>
#include <stdexcept>
#include "http_resource.h"

namespace nestor {
namespace net {

class HttpResponseParser {
public:
    HttpResponseParser();
    virtual ~HttpResponseParser();

    HttpResource *parseRawData(const std::string &data);

private:
    void parseInitLine(const std::string &l, HttpResource &output)
            throw (std::runtime_error);
};

} /* namespace rss */
} /* namespace nestor */
#endif /* HTTP_RESPONCE_PARSER_H_ */
