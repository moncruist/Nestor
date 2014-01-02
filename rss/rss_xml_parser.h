/*
 * rss_xml_parser.h
 *
 *  Created on: 14 дек. 2013 г.
 *      Author: moncruist
 */

#ifndef RSS_XML_PARSER_H_
#define RSS_XML_PARSER_H_

#include <vector>
#include <unicode/unistr.h>
#include "rss_object.h"

namespace nestor {
namespace rss {

class RssXmlParser {
public:
    RssXmlParser();
    virtual ~RssXmlParser();

    std::vector<RssObject *> *parseRss(const icu::UnicodeString &rss);
};

} /* namespace rss */
} /* namespace nestor */
#endif /* RSS_XML_PARSER_H_ */
