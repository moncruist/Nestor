/*
 * rss_xml_parser.cpp
 *
 *  Created on: 14 дек. 2013 г.
 *      Author: moncruist
 */

#include <tinyxml2.h>
#include <iostream>
#include <sstream>

#include "utils/string.h"
#include "rss_xml_parser.h"

using namespace std;
using namespace icu;
using namespace tinyxml2;
using namespace nestor::utils;

namespace nestor {
namespace rss {

const char *RssXmlParser::ROOT_RSS_ITEM = "rss";
const char *RssXmlParser::RSS_VERSION_ATTR = "version";
const char *RssXmlParser::CHANNEL_ITEM = "channel";
const char *RssXmlParser::TITLE_ITEM = "title";
const char *RssXmlParser::LINK_ITEM = "link";
const char *RssXmlParser::DESCRIPTION_ITEM = "description";

RssChannel *RssXmlParser::parseRss(const char *rss)
        throw (RssXmlParserException) {
    ostringstream oss_err;
    XMLElement *root, *channel, *tmp;
    XMLDocument doc;
    RssChannel *rssChannel = nullptr;
    double rssVersion;

    doc.Parse(rss);

    root = doc.RootElement();
    if (strcmp(root->Name(), ROOT_RSS_ITEM) != 0)
        throw RssXmlParserException("Missing root \"rss\" element");

    rssVersion = root->DoubleAttribute(RSS_VERSION_ATTR);
    cout << "RSS version: " << rssVersion << endl;

    if (rssVersion < 2.0) {
        oss_err << "Unsupported RSS version " << rssVersion;
        throw RssXmlParserException(oss_err.str());
    }

    channel = root->FirstChildElement(CHANNEL_ITEM);
    if (channel == nullptr)
        throw RssXmlParserException("Missing channel element");


    rssChannel = new RssChannel();

    tmp = channel->FirstChildElement(TITLE_ITEM);
    if (tmp == nullptr) {
        delete rssChannel;
        throw RssXmlParserException("Missing title element");
    }

    rssChannel->setTitle(UnicodeString(tmp->GetText()));

    tmp = channel->FirstChildElement(LINK_ITEM);
    if (tmp == nullptr) {
        delete rssChannel;
        throw RssXmlParserException("Missing link element");
    }

    rssChannel->setLink(UnicodeString(tmp->GetText()));

    tmp = channel->FirstChildElement(DESCRIPTION_ITEM);
    if (tmp == nullptr) {
        delete rssChannel;
        throw RssXmlParserException("Missing description element");
    }

    rssChannel->setDescription(UnicodeString(tmp->GetText()));


    return rssChannel;
}

} /* namespace rss */
} /* namespace nestor */
