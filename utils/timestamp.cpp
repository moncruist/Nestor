#include <cstring>
#include <clocale>
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

std::tm RFC822ToTimestamp(const std::string &timestamp) {
    char *lorig = strdup(setlocale(LC_TIME, nullptr));
    setlocale(LC_TIME, "C");

    tm newTime;
    memset(&newTime, 0, sizeof(newTime));
    strptime(timestamp.c_str(), "%a, %d %b %Y %H:%M:%S %z", &newTime);

    setlocale(LC_TIME, lorig);
    free(lorig);
    return newTime;
}

std::string timestampToRFC822(const std::tm &timestamp) {
    char *lorig = strdup(setlocale(LC_TIME, nullptr));
    setlocale(LC_TIME, "C");

    char str[100];
    memset(str, 0, sizeof(char) * 100);
    strftime(str, 100, "%a, %d %b %Y %H:%M:%S %z", &timestamp);

    setlocale(LC_TIME, lorig);
    free(lorig);
    return string(str);
}

}
}


