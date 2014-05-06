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


}
}


