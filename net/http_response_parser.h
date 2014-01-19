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
#include <map>
#include "http_resource.h"

namespace nestor {
namespace net {

class HttpResponseParser {
public:

    /**
     * Parses HTTP packet from raw data.
     * @param data Raw data (e.g. from socket)
     * @return HttpResource
     */
    static HttpResource *parseRawData(const std::string &data);

private:
    /**
     * Parses initial line of HTTP response and assign result code
     * to HttpResource object.
     * @param l initial line of HTTP response
     * @param output resource object
     */
    static void parseInitLine(const std::string &l, HttpResource &output)
            throw (std::runtime_error);

    /**
     * Extract field name from line in HTTP header.
     * @param str line in HTTP header
     * @param[out] last_pos last character index of extracted field name. -1 on error
     * @return field name. On error field name is empty string and last_pos is -1.
     */
    static std::string extractFieldName(const std::string &str, int &last_pos);

    /**
     * Combine new value with previous.
     */
    static std::map<std::string, std::string> &
    combineValue(std::map<std::string, std::string> &values,
            std::string name, std::string value);

    static void bindValues(const std::map<std::string, std::string> &values, HttpResource &output);

    static const std::string contentLengthField;
    static const std::string contentTypeField;
};

} /* namespace rss */
} /* namespace nestor */
#endif /* HTTP_RESPONCE_PARSER_H_ */
