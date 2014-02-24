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
#include <istream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <functional>
#include <memory>
#include <iostream>
#include <ctype.h>
#include <map>
#include <unordered_set>
#include "http_response_parser.h"
#include "utils/string.h"

using namespace std;
using namespace nestor::utils;

namespace nestor {
namespace net {

const std::string HttpResponseParser::CONTENT_LENGTH_FIELD = "content-length";
const std::string HttpResponseParser::CONTENT_TYPE_FIELD = "content-type";
const std::string HttpResponseParser::SERVER_FIELD = "server";
const std::string HttpResponseParser::CONTENT_ENCODING_FIELD = "content-encoding";

HttpResource* HttpResponseParser::parseRawData(const std::string &data) {
    istringstream iss(data);
    HttpResource *resource = new HttpResource();

    string line;
    // HTTP initial line checking
    safeGetLine(iss, line);
    if (iss.eof()) {
        delete resource;
        return nullptr;
    }

    try {
        parseInitLine(line, *resource);
    } catch (runtime_error &e) {
        cout << e.what() << endl;
        delete resource;
        return nullptr;
    }

    // Parsing HTTP header
    map<string, string> values; // header values
    string prev_field = "";     // last parsed value
    while (true) {
        safeGetLine(iss, line);
        /* Check for ending of http header */
        if (iss.eof() || line.length() == 0) {
            break;
        }

        // Getting header field name
        int tokenEnd;
        string token = extractFieldName(line, tokenEnd);
        string value;
        if (token.length()) {
            // New field
            int delimPos = tokenEnd + 1;
            int lineLength = line.length();
            if (delimPos >= lineLength || line[delimPos] != ':')
                continue;
            value = line.substr(delimPos + 1, lineLength - delimPos - 1);
            trim(value);
            stringToLower(token);
            combineValue(values, token, value);
            prev_field = token;
        } else {
            // Continuation of previous field
            if (prev_field.length() == 0) {
                continue;
            }
            token = prev_field;
            string contval = line;
            ltrim(contval);
            values[token].append(contval);
        }
    }

    for (auto it : values) {
        cout << it.first << " => " << it.second << endl;
    }

    bindValues(values, *resource);

    if (resource->contentLength() > 0) {
        unsigned char *content = new unsigned char[resource->contentLength()];
        iss.read(reinterpret_cast<char *>(content), resource->contentLength());
        resource->setContent(content);
    }

    return resource;
}



void HttpResponseParser::parseInitLine(const std::string &l,
        HttpResource &output) throw (runtime_error) {
    vector<string> parts;
    split(l, " ", parts);
    if (parts.size() < 3) {
        throw runtime_error("Wrong words number");
    }

    string upperpart = stringToUpperCopy(parts[0]);
    if (upperpart.compare("HTTP/1.0") != 0 &&
            upperpart.compare("HTTP/1.1") != 0) {
        throw runtime_error("Wrong protocol version");
    }

    long int code = strtol(parts[1].c_str(), nullptr, 10);
    if (code == 0) {
        throw runtime_error("Unrecognized code");
    }

    output.setCode(static_cast<unsigned int>(code));

    string codedef = parts[2];
    int partsSize = parts.size();
    for (int i = 3; i < partsSize; i++) {
        codedef.append(" ");
        codedef.append(parts[i]);
    }

    output.setCodeDefinition(codedef);
}

string HttpResponseParser::extractFieldName(const std::string& str,
        int& last_pos) {
    static const string special_chars = "()<>@,:;\\\"/[]?={}";
    string token = "";
    if (str.length() == 0) {
        last_pos = -1;
        return token;
    }

    int pos = 0;
    int strlength = str.length();
    for (pos = 0; pos < strlength; pos++) {
        char ch = str[pos];
        if (iscntrl(ch) || isblank(ch) ||
                special_chars.find(ch) != string::npos /* special characters */) {
            last_pos = pos - 1;
            return token;
        }
        token += ch;
    }

    // Token is entire string
    last_pos = pos;
    return token;
}

map<string, string>&
HttpResponseParser::combineValue(map<string, string>& values, string name,
        string value) {
    if (values.count(name) == 0) {
        // New name, just inserting
        values.insert(make_pair(name, value));
    } else {
        if (value.find(',') && values[name].find(',')) {
            vector<string> valueParts, mapValueParts;
            split(value, ",", valueParts);
            split(values[name], ",", mapValueParts);

            unordered_set<string> combine;
            for (string s: valueParts)
                combine.insert(trim(s));
            for (string s: mapValueParts)
                combine.insert(trim(s));

            string resultValue;
            for (unordered_set<string>::iterator i = combine.begin(); i != combine.end(); i++) {
                resultValue += *i;
                unordered_set<string>::iterator next = i;
                next++;
                if (next != combine.end())
                    resultValue += ",";
            }
        } else {
            values[name] = value;
        }
    }
    return values;
}

void HttpResponseParser::bindValues(const map<string, string>& values,
        HttpResource& output) {
    // All magic goes here

    // Content-Length field
    if (values.count(CONTENT_LENGTH_FIELD)) {
        string strLength = values.at(CONTENT_LENGTH_FIELD);
        unsigned int length = strtol(strLength.c_str(), NULL, 10);
        output.setContentLength(length);
    } else {
        output.setContentLength(0);
    }

    // Content-Type field
    if (values.count(CONTENT_TYPE_FIELD)) {
        string type = values.at(CONTENT_TYPE_FIELD);

        vector<string> typeParts, params;
        split(type, ";", typeParts);
        int typePartsSize = typeParts.size();

        output.setContentType(typeParts[0]);

        if (typePartsSize > 1) {
            for (int i = 1; i < typePartsSize; i++) {
                split(typeParts[i], "=", params);
                if (params.size() != 2) continue;

                if (stringToLowerCopy(trim(params[0])) == "charset") {
                    output.setContentCharset(stringToLowerCopy(trim(params[1])));
                }
            }
        }
    } else {
        output.setContentType("");
        output.setContentCharset("");
    }

    // Server field
    if (values.count(SERVER_FIELD)) {
        string server = values.at(SERVER_FIELD);
        output.setServer(server);
    } else {
        output.setServer("");
    }

    // Content-Encoding field
    if (values.count(CONTENT_ENCODING_FIELD)) {
        string encoding = values.at(CONTENT_ENCODING_FIELD);
        output.setServer(encoding);
    } else {
        output.setServer("");
    }
}

} /* namespace rss */
} /* namespace nestor */


