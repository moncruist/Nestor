#ifndef TIMESTAMP_H_
#define TIMESTAMP_H_

#include <string>
#include <ctime>

namespace nestor {
namespace utils {

std::tm stringToTimestamp(const std::string &timestamp, const std::string &format);
std::string timestampToString(const std::tm &timestamp, const std::string &format);

}
}



#endif /* TIMESTAMP_H_ */
