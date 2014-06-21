#ifndef TIMESTAMP_H_
#define TIMESTAMP_H_

#include <string>
#include <ctime>

namespace nestor {
namespace utils {

std::tm stringToTimestamp(const std::string &timestamp, const std::string &format);
std::string timestampToString(const std::tm &timestamp, const std::string &format);

std::tm RFC822ToTimestamp(const std::string &timestamp);
std::string timestampToRFC822(const std::tm &timestamp);

}
}



#endif /* TIMESTAMP_H_ */
