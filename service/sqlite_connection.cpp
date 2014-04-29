#include "logger.h"
#include "sqlite_connection.h"

using namespace std;

namespace nestor {
namespace service {

SqliteConnection::SqliteConnection(const std::string &fileName)
        : fileName_(fileName), handle_(nullptr), connected_(false) {
}

SqliteConnection::~SqliteConnection() {
    try {
        close();
    } catch(SqliteConnectionException &e) {
        SERVICE_LOG_LVL(ERROR, "SqliteConnection::dtr: exception while closing database");
    }
}


void SqliteConnection::open() {
    int code = sqlite3_open(fileName_.c_str(), &handle_);
    if (code != SQLITE_OK) {
        SERVICE_LOG_LVL(ERROR, "SqliteConnection::open: cannot connect to file: " << fileName_
                << ". Error code: " << code << " Description: " << sqlite3_errstr(code));
        throw SqliteConnectionException(sqlite3_errstr(code));
    }
    connected_ = true;
}

void SqliteConnection::close() {
    if (handle_) {
        int code = sqlite3_close(handle_);
        handle_ = nullptr;
        connected_ = false;
        if (code != SQLITE_OK) {
            SERVICE_LOG_LVL(ERROR, "SqliteConnection::close: cannot close db file: " << fileName_
                    << ". Error code: " << code << " Description: " << sqlite3_errstr(code));
            throw SqliteConnectionException(sqlite3_errstr(code));
        }
    }
}


sqlite3* SqliteConnection::handle() const {
    return handle_;
}


bool SqliteConnection::connected() const {
    return connected_;
}


const std::string& SqliteConnection::fileName() const {
    return fileName_;
}


void SqliteConnection::setFileName(const std::string& fileName) {
    fileName_ = fileName;
}

} /* namespace service */
} /* namespace nestor */
