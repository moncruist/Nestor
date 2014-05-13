#include <cstring>
#include "timestamp.h"

using namespace std;

namespace nestor {
namespace utils {

std::tm stringToTimestamp(const std::string &timestamp, const std::string &format) {
    tm newTime;
    memset(&newTime, 0, sizeof(newTime));
    strptime(timestamp.c_str(), format.c_str(), &newTime);
    return newTime;
}

std::string timestampToString(const std::tm &timestamp, const std::string &format) {
    char str[100];
    memset(str, 0, sizeof(char) * 100);
    strftime(str, 100, format.c_str(), &timestamp);
    return string(str);
}

}
}


