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
#include <istream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <functional>
#include <memory>
#include <iostream>
#include "http_response_parser.h"
#include "utils/string.h"

using namespace std;
using namespace nestor::utils;

namespace nestor {
namespace net {

HttpResponseParser::HttpResponseParser() {
    // TODO Auto-generated constructor stub

}

HttpResponseParser::~HttpResponseParser() {
    // TODO Auto-generated destructor stub
}

HttpResource* HttpResponseParser::parseRawData(const std::string &data) {
    istringstream iss(data);
    HttpResource *result = new HttpResource();

    string line;
    // HTTP initial line checking
    safeGetLine(iss, line);
    if (iss.eof()) {
        delete result;
        return nullptr;
    }

    try {
        parseInitLine(line, *result);
    } catch (runtime_error &e) {
        cout << e.what() << endl;
        delete result;
        return nullptr;
    }

    return result;
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
    for (int i = 3; i < parts.size(); i++) {
        codedef.append(" ");
        codedef.append(parts[i]);
    }

    output.setCodeDefinition(icu::UnicodeString(codedef.c_str()));
}

} /* namespace rss */
} /* namespace nestor */
