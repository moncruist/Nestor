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
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <mutex>
#include <cstring>
#include "logger.h"
#include "configuration.h"

using namespace std;
using namespace libconfig;

namespace nestor {
namespace service {

/* ============ ConfigurationSqlite BEGIN ================ */
const string ConfigurationSqlite::DEFAULT_DATABASE_PATH = "/var/lib/nestor/nestordb.sqlite";
static const string CONF_SQLITE_GLOBAL = "sqlite";
static const string CONF_SQLITE_DB_PATH = "database_path";
const string ConfigurationSqlite::DATABASE_PATH_CONFIG_PATH = CONF_SQLITE_GLOBAL + "." + CONF_SQLITE_DB_PATH;

ConfigurationSqlite::ConfigurationSqlite() {
    reset();
}

ConfigurationSqlite::~ConfigurationSqlite() {
    /* dummy for now */
}

void ConfigurationSqlite::reset() {
    databasePath_ = DEFAULT_DATABASE_PATH;
}

string ConfigurationSqlite::databasePath() const {
    return databasePath_;
}

void ConfigurationSqlite::load(const libconfig::Config* parser) {
    if (parser == nullptr) {
        SERVICE_LOG_LVL(ERROR, "ConfigurationSqlite::load: Invalid argument: parser == nullptr");
        return;
    }

    string confDbPath;
    if (parser->lookupValue(DATABASE_PATH_CONFIG_PATH, confDbPath))
        setDatabasePath(confDbPath);
}


#define CHECK_AND_RECREATE(setting, name, type) do { \
        if (!setting.exists(name)) { \
            setting.add(name, type); \
        } \
        else if (setting[name].getType() != type) { \
            setting.remove(name); \
            setting.add(name, type); \
        } \
    } while(0)

void ConfigurationSqlite::store(libconfig::Config* parser) {
    Setting &root = parser->getRoot();
    CHECK_AND_RECREATE(root, CONF_SQLITE_GLOBAL, Setting::TypeGroup);

    Setting &group = root[CONF_SQLITE_GLOBAL];
    CHECK_AND_RECREATE(group, CONF_SQLITE_DB_PATH, Setting::TypeString);
    group[CONF_SQLITE_DB_PATH] = databasePath_;
}

void ConfigurationSqlite::setDatabasePath(string &databasePath) {
    databasePath_ = databasePath;
}

/* ============ ConfigurationSqlite END ================== */

/* ============ Configuration BEGIN ====================== */
Configuration *Configuration::instance_ = nullptr;
recursive_mutex Configuration::instanceLock_;

const int Configuration::DATABASE_PROVIDERS_LENGTH = 1;

const std::string DATABASE_PROVIDERS_DATA[Configuration::DATABASE_PROVIDERS_LENGTH] = {
        "sqlite"
};

const std::string * const Configuration::DATABASE_PROVIDERS = DATABASE_PROVIDERS_DATA;

const string Configuration::DEFAULT_DATABASE_PROVIDER = "sqlite";

const char *Configuration::DATABASE_PROVIDER_PATH = "database_provider";

Configuration::Configuration() {
    parser_ = new libconfig::Config();
    reset();
}



Configuration::~Configuration() {
    if (parser_) delete parser_;
}


const std::string& Configuration::databaseProvider() const {
    return databaseProvider_;
}


void Configuration::setDatabaseProvider(const std::string& databaseProvider) {
    for (int i = 0; i < DATABASE_PROVIDERS_LENGTH; i++) {
        if (databaseProvider.compare(DATABASE_PROVIDERS[i]) == 0) {
            databaseProvider_ = databaseProvider;
            return;
        }
    }

    SERVICE_LOG_LVL(WARN, "Configuration::setDatabaseProvider: Unsupported database provider: " << databaseProvider);
}


ConfigurationSqlite& Configuration::sqliteConfig() {
    return sqliteConfig_;
}

void Configuration::setSqliteConfig(const ConfigurationSqlite& sqliteConfig) {
    sqliteConfig_ = sqliteConfig;
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

    setDatabaseProvider(DEFAULT_DATABASE_PROVIDER);
    sqliteConfig_.reset();
}

void Configuration::load(const std::string &configFile) {
    struct stat fileInfo;
    int r;

    r = stat(configFile.c_str(), &fileInfo);
    if (r == -1) {
        SERVICE_LOG_LVL(ERROR, "Configuration::load: File " << configFile<< " - stat failed with error: " << strerror(errno));
        return;
    }

    if (!S_ISREG(fileInfo.st_mode)) {
        SERVICE_LOG_LVL(ERROR, "Configuration::load: File " << configFile << " is not a regular file");
        return;
    }

    try {
        parser_->readFile(configFile.c_str());
    } catch (const FileIOException &e) {
        SERVICE_LOG_LVL(ERROR,
                "Configuration::load: Cannot load config file: " << configFile);
        return;
    } catch (const ParseException &e) {
        SERVICE_LOG_LVL(ERROR,
                "Configuration::load: Cannot parse config file: " << e.getFile() << ". Line: "
                << e.getLine() << " - " << e.getError());
        return;
    }

    string provider;
    if (parser_->lookupValue(DATABASE_PROVIDER_PATH, provider))
        setDatabaseProvider(provider);

    sqliteConfig_.load(parser_);

    SERVICE_LOG("Configuration::load: Configuration successfully loaded");
}

void Configuration::store(const std::string& configFile) {
    delete parser_;
    parser_ = new Config();
    Setting &root = parser_->getRoot();
    root.add(DATABASE_PROVIDER_PATH, Setting::TypeString) = databaseProvider_;

    sqliteConfig_.store(parser_);

    try {
        parser_->writeFile(configFile.c_str());
    } catch (const FileIOException &e) {
        SERVICE_LOG_LVL(ERROR,
                "Configuration::store: Cannot load config file: " << configFile);
        return;
    }

    SERVICE_LOG("Configuration::store: Configuration successfully stored");
}

/* ============ Configuration END== ====================== */

} /* namespace service */
} /* namespace nestor */


