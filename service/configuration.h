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
#ifndef CONFIGURATION_H_
#define CONFIGURATION_H_

#include <mutex>
#include <string>

namespace nestor {
namespace service {

/**
 * Sqlite specific options
 */
class ConfigurationSqlite {
public:
    static const std::string DEFAULT_DATABESE_PATH;

    explicit ConfigurationSqlite();
    ~ConfigurationSqlite();

    void reset();

    std::string databasePath() const;
    void setDatabasePath(std::string &databasePath);

private:
    std::string databasePath_;
};

/**
 * Singleton class. Represents config of the whole application.
 */
class Configuration {
// constants
public:
    static const std::string DEFAULT_DATABASE_PROVIDER;
public:
    static Configuration *instance();

    void reset();
    void load(int argc, char *argv[]);

    const std::string& databaseProvider() const;
    void setDatabaseProvider(const std::string& databaseProvider);
    const ConfigurationSqlite& sqliteConfig() const;
    void setSqliteConfig(const ConfigurationSqlite& sqliteConfig);

private:
    explicit Configuration();
    ~Configuration();

// static members
private:
    static Configuration *instance_;
    static std::recursive_mutex instanceLock_;

// members
private:
    std::string databaseProvider_;
    ConfigurationSqlite sqliteConfig_;
};

} /* namespace service */
} /* namespace nestor */

#endif /* CONFIGURATION_H_ */
