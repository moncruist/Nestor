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
#ifndef RSS_OBJECT_H_
#define RSS_OBJECT_H_

#include <ctime>
#include <unicode/unistr.h>

namespace nestor {
namespace rss {

class RssObject {
public:
    RssObject();
    virtual ~RssObject();
    const icu::UnicodeString& caption() const;
    void setCaption(const icu::UnicodeString& caption);
    const icu::UnicodeString& text() const;
    void setText(const icu::UnicodeString& text);
    const icu::UnicodeString& link() const;
    void setLink(const icu::UnicodeString& link);
    const icu::UnicodeString& guid() const;
    void setGuid(const icu::UnicodeString& guid);
    const std::tm& pubDate() const;
    void setPubDate(const std::tm& pubDate);

private:
    icu::UnicodeString caption_;
    icu::UnicodeString text_;
    icu::UnicodeString link_;
    icu::UnicodeString guid_;
    std::tm pubDate_;
};

} /* namespace rss */
} /* namespace nestor */
#endif /* RSS_OBJECT_H_ */
