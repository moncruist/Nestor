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

namespace nestor {
namespace rss {

RssObject::RssObject() :
    caption_(""), text_(""), link_("") {
    // TODO Auto-generated constructor stub

}

RssObject::~RssObject() {
    // TODO Auto-generated destructor stub
}

const icu::UnicodeString& RssObject::caption() const {
    return caption_;
}

void RssObject::setCaption(const icu::UnicodeString& caption) {
    this->caption_ = caption;
}

const icu::UnicodeString& RssObject::text() const {
    return text_;
}

void RssObject::setText(const icu::UnicodeString& text) {
    this->text_ = text;
}

const icu::UnicodeString& RssObject::link() const {
    return link_;
}

void RssObject::setLink(const icu::UnicodeString& link) {
    link_ = link;
}

} /* namespace rss */
} /* namespace nestor */
