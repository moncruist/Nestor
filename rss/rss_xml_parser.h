/*
 * rss_xml_parser.h
 *
 *  Created on: 14 дек. 2013 г.
 *      Author: moncruist
 */

#ifndef RSS_XML_PARSER_H_
#define RSS_XML_PARSER_H_

#include <vector>
#include <exception>
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
