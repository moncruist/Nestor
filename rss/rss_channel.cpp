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
#include "rss_channel.h"

using namespace std;

namespace nestor {
namespace rss {

RssChannel::RssChannel()
        : RssChannel("", "", "") {
}

RssChannel::RssChannel(std::string title, std::string link,
        std::string description)
        : title_(title), link_(link), description_(description) {
}

RssChannel::~RssChannel() {
    for (auto i : items_)
        delete i;
    items_.clear();
}

void RssChannel::addItem(RssObject* item) {
    items_.push_back(item);
}

RssObject* RssChannel::getItem(unsigned int idx) const {
    if (idx < items_.size())
        return items_[idx];
    return nullptr;
}

unsigned int RssChannel::itemsCount() const {
    return items_.size();
}

const std::string& RssChannel::description() const {
    return description_;
}

void RssChannel::setDescription(const std::string& description) {
    description_ = description;
}

const std::string& RssChannel::link() const {
    return link_;
}

void RssChannel::setLink(const std::string& link) {
    link_ = link;
}

const std::string& RssChannel::title() const {
    return title_;
}

void RssChannel::setTitle(const std::string& title) {
    title_ = title;
}

const map<string, string>& RssChannel::optional() const {
    return optional_;
}

void RssChannel::setOptional(const map<string, string>& tags) {
    optional_.clear();
    for (auto i = tags.cbegin(); i != tags.cend(); i++) {
        optional_.insert(make_pair((*i).first, (*i).second));
    }
}

} /* namespace rss */
} /* namespace nestor */


