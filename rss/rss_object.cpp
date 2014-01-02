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
    caption(""), text("") {
    // TODO Auto-generated constructor stub

}

RssObject::~RssObject() {
    // TODO Auto-generated destructor stub
}

const icu::UnicodeString& RssObject::getCaption() const {
    return caption;
}

void RssObject::setCaption(const icu::UnicodeString& caption) {
    this->caption = caption;
}

const icu::UnicodeString& RssObject::getText() const {
    return text;
}

void RssObject::setText(const icu::UnicodeString& text) {
    this->text = text;
}

} /* namespace rss */
} /* namespace nestor */
