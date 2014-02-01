/*
 * rss_xml_parser.cpp
 *
 *  Created on: 14 дек. 2013 г.
 *      Author: moncruist
 */

#include <tinyxml2.h>
#include <iostream>
#include <sstream>
#include <unordered_set>
#include <map>

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
const char *RssXmlParser::ITEM_TAG = "item";


static XMLElement *getExpectedElement(XMLElement *parent, const char *name)
        throw (RssXmlParserException) {
    XMLElement *result = parent->FirstChildElement(name);
    if (result == nullptr) {
        ostringstream oerr;
        oerr << "Missing element \"" << name << "\" in parent \"" << parent->Name() << "\"";
        throw RssXmlParserException(oerr.str());
    }
    return result;
}


static vector<RssObject *> *parseItems(XMLElement *channel) {
    vector<RssObject *> *items = new vector<RssObject *>();
    XMLElement *rssItem = channel->FirstChildElement(RssXmlParser::ITEM_TAG);
    while (rssItem != nullptr) {
        RssObject *obj = new RssObject();

        try {
            obj->setCaption(UnicodeString(getExpectedElement(rssItem, RssXmlParser::TITLE_ITEM)->GetText()));
            obj->setLink(UnicodeString(getExpectedElement(rssItem, RssXmlParser::LINK_ITEM)->GetText()));
            obj->setText(UnicodeString(getExpectedElement(rssItem, RssXmlParser::DESCRIPTION_ITEM)->GetText()));
        } catch (RssXmlParserException &e) {
            delete obj;
            rssItem = rssItem->NextSiblingElement(RssXmlParser::ITEM_TAG);
            continue;
        }

        items->push_back(obj);
        rssItem = rssItem->NextSiblingElement(RssXmlParser::ITEM_TAG);
    }
    return items;
}


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

    channel = getExpectedElement(root, CHANNEL_ITEM);


    rssChannel = new RssChannel();

    // Parsing RSS <channel> tag
    // Required tags
    try {
        rssChannel->setTitle(UnicodeString(getExpectedElement(channel, TITLE_ITEM)->GetText()));
        rssChannel->setLink(UnicodeString(getExpectedElement(channel, LINK_ITEM)->GetText()));
        rssChannel->setDescription(UnicodeString(getExpectedElement(channel, DESCRIPTION_ITEM)->GetText()));

    } catch (RssXmlParserException &e) {
        delete rssChannel;
        throw;
    }

    // Optional tags
    unordered_set<string> alreadyParsed = {TITLE_ITEM, LINK_ITEM, DESCRIPTION_ITEM,
            ITEM_TAG /* will parse "item" later*/ };
    map<UnicodeString, UnicodeString> optionalTags;

    tmp = channel->FirstChildElement();
    do {
        if (alreadyParsed.count(tmp->Name())) {
            tmp = tmp->NextSiblingElement();
            continue;
        }

        optionalTags.insert(make_pair(tmp->Name(), tmp->GetText()));
        tmp = tmp->NextSiblingElement();
    } while(tmp != nullptr);

    rssChannel->setOptional(optionalTags);

    auto items = parseItems(channel);
    for (auto i : *items)
        rssChannel->addItem(i);

    delete items;

    return rssChannel;
}

} /* namespace rss */
} /* namespace nestor */
