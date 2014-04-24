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
#include <mutex>
#include "configuration.h"

using namespace std;

namespace nestor {
namespace service {

/* ============ ConfigurationSqlite BEGIN ================ */

const string ConfigurationSqlite::DEFAULT_DATABESE_PATH = "/var/lib/nestor/nestordb.sqlite";

ConfigurationSqlite::ConfigurationSqlite() {
    reset();
}

ConfigurationSqlite::~ConfigurationSqlite() {
    /* dummy for now */
}

void ConfigurationSqlite::reset() {
    databasePath_ = DEFAULT_DATABESE_PATH;
}

string ConfigurationSqlite::databasePath() const {
    return databasePath_;
}

void ConfigurationSqlite::setDatabasePath(string &databasePath) {
    databasePath_ = databasePath;
}

/* ============ ConfigurationSqlite END ================== */

/* ============ Configuration BEGIN ====================== */
Configuration *Configuration::instance_ = nullptr;
recursive_mutex Configuration::instanceLock_;

const string Configuration::DEFAULT_DATABASE_PROVIDER = "sqlite";

Configuration::Configuration() {
    reset();
}

const std::string& Configuration::databaseProvider() const {
    return databaseProvider_;
}

void Configuration::setDatabaseProvider(const std::string& databaseProvider) {
    if (databaseProvider.compare("sqlite") == 0)
        databaseProvider_ = databaseProvider;
}

const ConfigurationSqlite& Configuration::sqliteConfig() const {
    return sqliteConfig_;
}

void Configuration::setSqliteConfig(const ConfigurationSqlite& sqliteConfig) {
    sqliteConfig_ = sqliteConfig;
}

Configuration::~Configuration() {
}

Configuration* Configuration::instance() {
    if (instance_ == nullptr) {
        instanceLock_.lock();
        if (instance_ == nullptr) {
            instance_ = new Configuration();
            instance_->reset();
        }
        instanceLock_.unlock();
    }

    return instance_;
}

void Configuration::reset() {
    lock_guard<recursive_mutex> locker(instanceLock_);

    databaseProvider_ = DEFAULT_DATABASE_PROVIDER;
    sqliteConfig_.reset();
}

void Configuration::load(int argc, char* argv[]) {
}

/* ============ Configuration END== ====================== */

} /* namespace service */
} /* namespace nestor */


