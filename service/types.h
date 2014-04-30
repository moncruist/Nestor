#ifndef TYPES_H_
#define TYPES_H_

#include <string>

namespace nestor {
namespace service {

struct User {
    long long id;
    std::string username;
    std::string password;
};

}
}


#endif /* TYPES_H_ */
