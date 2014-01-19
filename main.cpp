/*
 *  This file is part of Nestor.
 *
 *  Nestor - program for aggregation RSS subscriptions with access via
 *  IMAP interface.
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
#include <unicode/unistr.h>
#include <unicode/ustream.h>
#include <iostream>
#include <string>
#include <sstream>

#include <algorithm>
#include "net/http_client.h"
#include "net/http_resource.h"

#include <unicode/ucnv.h>
#include <unicode/utypes.h>



using namespace std;
using namespace nestor::net;
using namespace icu;



const string example_rss_host = "lenta.ru";
const string example_rss_resource = "/rss";

int main(int argc, char *argv[]) {
    ostringstream oss;
    HttpClient client(example_rss_host);
    HttpResource *response = client.getResource(example_rss_resource);
    cout << "Code: " << response->code() << endl;
    cout << "Code definition: " << response->codeDefinition() << endl;
    cout << "Content type: " << response->contentType() << endl;
    cout << "Content charset: " << response->contentCharset() << endl;
    cout << "Content length: " << response->contentLength() << endl;
}
