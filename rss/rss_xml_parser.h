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

#ifndef RSS_XML_PARSER_H_
#define RSS_XML_PARSER_H_

#include <vector>
#include <exception>
#include <stdexcept>
#include <unicode/unistr.h>
#include "rss_object.h"
#include "rss_channel.h"

namespace nestor {
namespace rss {

class RssXmlParserException : public std::runtime_error {
    using std::runtime_error::runtime_error;
};

class RssXmlParser {
    // constants
public:
    static const char *ROOT_RSS_ITEM;
    static const char *RSS_VERSION_ATTR;
    static const char *CHANNEL_ITEM;
    static const char *TITLE_ITEM;
    static const char *LINK_ITEM;
    static const char *DESCRIPTION_ITEM;
    static const char *ITEM_TAG;
public:
    static RssChannel *parseRss(const char *rss)
            throw (RssXmlParserException);
};

} /* namespace rss */
} /* namespace nestor */
#endif /* RSS_XML_PARSER_H_ */
