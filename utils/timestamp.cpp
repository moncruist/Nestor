#include <cstring>
#include "timestamp.h"

using namespace std;

namespace nestor {
namespace utils {

std::tm timestampConvert(const std::string &timestamp, const std::string &format) {
    tm newTime;
    memset(&newTime, 0, sizeof(newTime));
    strptime(timestamp.c_str(), format.c_str(), &newTime);
    return newTime;
}

}
}


