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
#ifndef UTILS_STRING_H_
#define UTILS_STRING_H_

#include <string>
#include <vector>

namespace nestor {
namespace utils {

int split(const std::string &str, const std::string &sep,
        std::vector<std::string> &array);
std::istream& safeGetLine(std::istream& is, std::string& t);
void stringToUpper(std::string &str);
std::string stringToUpperCopy(std::string &str);
}
}

#endif /* UTILS_STRING_H_ */
