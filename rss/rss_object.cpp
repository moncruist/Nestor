/*
 * rss_object.cpp
 *
 *  Created on: 14 дек. 2013 г.
 *      Author: moncruist
 */

#include "rss_object.h"

namespace nestor {
namespace rss {

RssObject::RssObject() :
    _caption(""), _text(""), _link("") {
    // TODO Auto-generated constructor stub

}

RssObject::~RssObject() {
    // TODO Auto-generated destructor stub
}

const icu::UnicodeString& RssObject::caption() const {
    return _caption;
}

void RssObject::setCaption(const icu::UnicodeString& caption) {
    this->_caption = caption;
}

const icu::UnicodeString& RssObject::text() const {
    return _text;
}

void RssObject::setText(const icu::UnicodeString& text) {
    this->_text = text;
}

const icu::UnicodeString& RssObject::link() const {
    return _link;
}

void RssObject::setLink(const icu::UnicodeString& link) {
    _link = link;
}

} /* namespace rss */
} /* namespace nestor */
