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
#ifndef TYPES_H_
#define TYPES_H_

#include <string>
#include <ctime>

namespace nestor {
namespace service {

/**
 * POD class representing User object.
 */
class User {
public:
    long long id() const;
    void setId(long long id);
    const std::string& password() const;
    void setPassword(const std::string& password);
    const std::string& username() const;
    void setUsername(const std::string& username);

private:
    long long id_;
    std::string username_;
    std::string password_;
};


/**
 * POD class representing Channel object.
 */
class Channel {
public:
    const std::string& description() const;
    void setDescription(const std::string& description);
    long long id() const;
    void setId(long long id);
    std::tm lastUpdate() const;
    void setLastUpdate(std::tm lastUpdate);
    const std::string& link() const;
    void setLink(const std::string& link);
    const std::string& rssLink() const;
    void setRssLink(const std::string& rssLink);
    const std::string& title() const;
    void setTitle(const std::string& title);
    int updateInterval() const;
    void setUpdateInterval(int updateInterval);

private:
    long long id_;
    std::string title_;
    std::string rssLink_;
    std::string link_;
    std::string description_;
    int updateInterval_;
    std::tm lastUpdate_;
};

/**
 * POD class representing Feed object.
 */
class Feed {
public:
    long long channelId() const;
    void setChannelId(long long channelId);
    const std::string& description() const;
    void setDescription(const std::string& description);
    const std::string& guid() const;
    void setGuid(const std::string& guid);
    long long id() const;
    void setId(long long id);
    const std::string& link() const;
    void setLink(const std::string& link);
    const std::tm& publicationDate() const;
    void setPublicationDate(const std::tm& publicationDate);
    const std::string& text() const;
    void setText(const std::string& text);
    const std::string& title() const;
    void setTitle(const std::string& title);

private:
    long long id_;
    long long channelId_;
    std::string guid_;
    std::string title_;
    std::string link_;
    std::string description_;
    std::tm publicationDate_;
    std::string text_;
};

}
}


#endif /* TYPES_H_ */
