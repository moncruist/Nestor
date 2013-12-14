/*
 *  This file is part of Nestor.
 *
 *  Nestor - program for aggregation RSS subscriptions with access via
 *  IMAP interface.
 *  Copyright (C) 2013  Konstantin Zhukov moncruist@gmail.com
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

#include <iostream>
#include <string>
#include <sstream>
#include "net/http_client.h"

using namespace std;
using namespace nestor::net;

int main(int argc, char *argv[]) {
    ostringstream oss;
    HttpClient client("en.wikipedia.org");
    auto page = client.getResource("/wiki/Main_Page");
    for (auto c : *page) {
        oss << c;
    }
    cout << oss.str();
}
