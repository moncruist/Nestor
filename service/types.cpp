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


long long Post::channelId() const {
    return channelId_;
}

void Post::setChannelId(long long channelId) {
    channelId_ = channelId;
}

const std::string& Post::description() const {
    return description_;
}

void Post::setDescription(const std::string& description) {
    description_ = description;
}

const std::string& Post::guid() const {
    return guid_;
}

void Post::setGuid(const std::string& guid) {
    guid_ = guid;
}

long long Post::id() const {
    return id_;
}

void Post::setId(long long id) {
    id_ = id;
}

const std::string& Post::link() const {
    return link_;
}

void Post::setLink(const std::string& link) {
    link_ = link;
}

const std::tm& Post::publicationDate() const {
    return publicationDate_;
}

void Post::setPublicationDate(const std::tm& publicationDate) {
    publicationDate_ = publicationDate;
}

const std::string& Post::text() const {
    return text_;
}

void Post::setText(const std::string& text) {
    text_ = text;
}

const std::string& Post::title() const {
    return title_;
}

void Post::setTitle(const std::string& title) {
    title_ = title;
}

}
}


