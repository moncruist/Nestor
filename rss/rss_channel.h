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

#ifndef RSS_CHANNEL_H_
#define RSS_CHANNEL_H_

#include <vector>
#include <string>
#include <map>

#include "rss_object.h"

namespace nestor {
namespace rss {

class RssChannel {
public:
    explicit RssChannel();
    RssChannel(std::string title, std::string link, std::string description);
    virtual ~RssChannel();

    void addItem(RssObject *item);
    RssObject *getItem(unsigned int idx) const;

    unsigned int itemsCount() const;
    const std::string& description() const;
    void setDescription(const std::string& description);
    const std::string& link() const;
    void setLink(const std::string& link);
    const std::string& title() const;
    void setTitle(const std::string& title);

    const std::map<std::string, std::string> &optional() const;
    void setOptional(const std::map<std::string, std::string> &tags);

private:
    std::string title_;
    std::string link_;
    std::string description_;
    std::vector<RssObject *> items_;
    std::map<std::string, std::string> optional_;
};

} /* namespace rss */
} /* namespace nestor */
#endif /* RSS_CHANNEL_H_ */
