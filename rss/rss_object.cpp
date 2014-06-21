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

#include "rss_object.h"
#include <ctime>
#include <cstring>

using namespace std;

namespace nestor {
namespace rss {

RssObject::RssObject() :
    title_(""), text_(""), link_(""), guid_("") {
    memset(&pubDate_, 0, sizeof(tm));
}

RssObject::~RssObject() {
}

const std::string& RssObject::title() const {
    return title_;
}

void RssObject::setTitle(const std::string& caption) {
    this->title_ = caption;
}

const std::string& RssObject::text() const {
    return text_;
}

void RssObject::setText(const std::string& text) {
    this->text_ = text;
}

const std::string& RssObject::link() const {
    return link_;
}

void RssObject::setLink(const std::string& link) {
    link_ = link;
}

const std::string& RssObject::guid() const {
    return guid_;
}

void RssObject::setGuid(const std::string& guid) {
    guid_ = guid;
}

const std::tm& RssObject::pubDate() const {
    return pubDate_;
}

void RssObject::setPubDate(const std::tm& pubDate) {
    pubDate_ = pubDate;
}


} /* namespace rss */
} /* namespace nestor */

