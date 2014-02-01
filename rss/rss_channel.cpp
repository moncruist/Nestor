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
#include "rss_channel.h"

using namespace std;

namespace nestor {
namespace rss {

RssChannel::RssChannel()
        : RssChannel("", "", "") {
}

RssChannel::RssChannel(icu::UnicodeString title, icu::UnicodeString link,
        icu::UnicodeString description)
        : _title(title), _link(link), _description(description) {
}

RssChannel::~RssChannel() {
    for (auto i : _items)
        delete i;
    _items.clear();
}

void RssChannel::addItem(RssObject* item) {
    _items.push_back(item);
}

RssObject* RssChannel::getItem(unsigned int idx) const {
    if (idx < _items.size())
        return _items[idx];
    return nullptr;
}

unsigned int RssChannel::itemsCount() const {
    return _items.size();
}

const icu::UnicodeString& RssChannel::description() const {
    return _description;
}

void RssChannel::setDescription(const icu::UnicodeString& description) {
    _description = description;
}

const icu::UnicodeString& RssChannel::link() const {
    return _link;
}

void RssChannel::setLink(const icu::UnicodeString& link) {
    _link = link;
}

const icu::UnicodeString& RssChannel::title() const {
    return _title;
}

void RssChannel::setTitle(const icu::UnicodeString& title) {
    _title = title;
}

const map<UnicodeString, UnicodeString>& RssChannel::optional() const {
    return _optional;
}

void RssChannel::setOptional(const map<UnicodeString, UnicodeString>& tags) {
    _optional.clear();
    for (auto i = tags.cbegin(); i != tags.cend(); i++) {
        _optional.insert(make_pair((*i).first, (*i).second));
    }
}

} /* namespace rss */
} /* namespace nestor */


