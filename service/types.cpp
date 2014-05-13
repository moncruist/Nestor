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
#include "types.h"

namespace nestor {
namespace service {

long long User::id() const {
    return id_;
}

void User::setId(long long id) {
    id_ = id;
}

const std::string& User::password() const {
    return password_;
}

void User::setPassword(const std::string& password) {
    password_ = password;
}

const std::string& User::username() const {
    return username_;
}

void User::setUsername(const std::string& username) {
    username_ = username;
}


const std::string& Channel::description() const {
    return description_;
}

void Channel::setDescription(const std::string& description) {
    description_ = description;
}

long long Channel::id() const {
    return id_;
}

void Channel::setId(long long id) {
    id_ = id;
}

std::tm Channel::lastUpdate() const {
    return lastUpdate_;
}

void Channel::setLastUpdate(std::tm lastUpdate) {
    lastUpdate_ = lastUpdate;
}

const std::string& Channel::link() const {
    return link_;
}

void Channel::setLink(const std::string& link) {
    link_ = link;
}

const std::string& Channel::rssLink() const {
    return rssLink_;
}

void Channel::setRssLink(const std::string& rssLink) {
    rssLink_ = rssLink;
}

const std::string& Channel::title() const {
    return title_;
}

void Channel::setTitle(const std::string& title) {
    title_ = title;
}

int Channel::updateInterval() const {
    return updateInterval_;
}

void Channel::setUpdateInterval(int updateInterval) {
    updateInterval_ = updateInterval;
}


long long Feed::channelId() const {
    return channelId_;
}

void Feed::setChannelId(long long channelId) {
    channelId_ = channelId;
}

const std::string& Feed::description() const {
    return description_;
}

void Feed::setDescription(const std::string& description) {
    description_ = description;
}

const std::string& Feed::guid() const {
    return guid_;
}

void Feed::setGuid(const std::string& guid) {
    guid_ = guid;
}

long long Feed::id() const {
    return id_;
}

void Feed::setId(long long id) {
    id_ = id;
}

const std::string& Feed::link() const {
    return link_;
}

void Feed::setLink(const std::string& link) {
    link_ = link;
}

const std::tm& Feed::publicationDate() const {
    return publicationDate_;
}

void Feed::setPublicationDate(const std::tm& publicationDate) {
    publicationDate_ = publicationDate;
}

const std::string& Feed::text() const {
    return text_;
}

void Feed::setText(const std::string& text) {
    text_ = text;
}

const std::string& Feed::title() const {
    return title_;
}

void Feed::setTitle(const std::string& title) {
    title_ = title;
}

}
}


