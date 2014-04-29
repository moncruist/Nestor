#include <string>
#include "logger.h"
#include "sqlite_provider.h"

using namespace std;

namespace nestor {
namespace service {

SqliteProvider::SqliteProvider(SqliteConnection *connection)
        : connection_(connection) {
    if (connection == nullptr) {
        string errmsg = "SqliteProvider::ctr: Invalid argument: connection == nullptr";
        SERVICE_LOG_LVL(ERROR, errmsg);
        throw logic_error(errmsg);
    }

    if (!connection->connected()) {
        string errmsg = "SqliteProvider::ctr: Invalid argument: connection is not connected to the database";
        SERVICE_LOG_LVL(ERROR, errmsg);
        throw logic_error(errmsg);
    }
}

SqliteProvider::~SqliteProvider() {
}

} /* namespace service */
} /* namespace nestor */
