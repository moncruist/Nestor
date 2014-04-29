#ifndef SQLITE_PROVIDER_H_
#define SQLITE_PROVIDER_H_

#include <stdexcept>
#include "sqlite_connection.h"

namespace nestor {
namespace service {

class SqliteProviderException : public std::runtime_error {
    using std::runtime_error::runtime_error;
};

class SqliteProvider {
public:
    SqliteProvider(SqliteConnection *connection);
    virtual ~SqliteProvider();

private:
    SqliteConnection *connection_;
};

} /* namespace service */
} /* namespace nestor */

#endif /* SQLITE_PROVIDER_H_ */
