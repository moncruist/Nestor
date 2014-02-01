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

#ifndef RSS_CHANNEL_H_
#define RSS_CHANNEL_H_

#include <vector>
#include <string>
#include <map>

#include <unicode/unistr.h>
#include "rss_object.h"

namespace nestor {
namespace rss {

class RssChannel {
public:
    explicit RssChannel();
    RssChannel(icu::UnicodeString title, icu::UnicodeString link, icu::UnicodeString description);
    virtual ~RssChannel();

    void addItem(RssObject *item);
    RssObject *getItem(unsigned int idx) const;

    unsigned int itemsCount() const;
    const icu::UnicodeString& description() const;
    void setDescription(const icu::UnicodeString& description);
    const icu::UnicodeString& link() const;
    void setLink(const icu::UnicodeString& link);
    const icu::UnicodeString& title() const;
    void setTitle(const icu::UnicodeString& title);

    const std::map<icu::UnicodeString, icu::UnicodeString> &optional() const;
    void setOptional(const std::map<icu::UnicodeString, icu::UnicodeString> &tags);

private:
    icu::UnicodeString _title;
    icu::UnicodeString _link;
    icu::UnicodeString _description;
    std::vector<RssObject *> _items;
    std::map<icu::UnicodeString, icu::UnicodeString> _optional;
};

} /* namespace rss */
} /* namespace nestor */
#endif /* RSS_CHANNEL_H_ */
