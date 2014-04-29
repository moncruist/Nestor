#ifndef SQLITE_CONNECTION_H_
#define SQLITE_CONNECTION_H_

#include <string>
#include <stdexcept>
#include "sqlite3.h"

namespace nestor {
namespace service {

class SqliteConnectionException : public std::runtime_error {
    using std::runtime_error::runtime_error;
};

class SqliteConnection {
public:
    explicit SqliteConnection(const std::string &fileName);
    virtual ~SqliteConnection();

    void open();
    void close();

    bool connected() const;

    sqlite3 *handle() const;

    const std::string& fileName() const;
    void setFileName(const std::string& fileName);

private:
    std::string fileName_;
    sqlite3 *handle_;
    bool connected_;
};

} /* namespace service */
} /* namespace nestor */

#endif /* SQLITE_CONNECTION_H_ */
