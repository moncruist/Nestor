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

#include <vector>
#include <algorithm>
#include <string>
#include <sstream>
#include <cstring>
#include "string.h"

using namespace std;

namespace nestor {
namespace utils {

int split(const string &str, const string &sep, vector<string> &array) {
    vector<string> result;
    char *cstr = const_cast<char *>(str.c_str());
    char *current;
    int count = 0;

    current = strtok(cstr, sep.c_str());
    while(current != nullptr) {
        array.push_back(current);
        count++;
        current = strtok(nullptr, sep.c_str());
    }
    return count;
}


istream& safeGetLine(istream& is, string& t) {
    t.clear();

    // The characters in the stream are read one-by-one using a std::streambuf.
    // That is faster than reading them one-by-one using the std::istream.
    // Code that uses streambuf this way must be guarded by a sentry object.
    // The sentry object performs various tasks,
    // such as thread synchronization and updating the stream state.

    istream::sentry se(is, true);
    streambuf* sb = is.rdbuf();

    for(;;) {
        int c = sb->sbumpc();
        switch (c) {
        case '\n':
            return is;
        case '\r':
            if(sb->sgetc() == '\n')
                sb->sbumpc();
            return is;
        case EOF:
            // Also handle the case when the last line has no line ending
            if(t.empty())
                is.setstate(ios::eofbit);
            return is;
        default:
            t += (char)c;
        }
    }
    return is;
}

void stringToUpper(string &str) {
    transform(str.begin(), str.end(), str.begin(), ::toupper);
}

string stringToUpperCopy(string &str) {
    string upperstr = str;
    stringToUpper(upperstr);
    return upperstr;
}

void stringToLower(string &str) {
    transform(str.begin(), str.end(), str.begin(), ::tolower);
}

string stringToLowerCopy(string &str) {
    string lowerstr = str;
    stringToLower(lowerstr);
    return lowerstr;
}

}
}


