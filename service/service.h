#ifndef SERVICE_H_
#define SERVICE_H_

#include <string>

namespace nestor {
namespace service {

/**
 * Core Nestor logic class.
 */
class Service {
public:
    Service();
    virtual ~Service();

    bool authenticate(std::string login, std::string password);

    void onLogout();
};

} /* namespace service */
} /* namespace nestor */

#endif /* SERVICE_H_ */
