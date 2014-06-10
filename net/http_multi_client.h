#ifndef HTTP_MULTI_CLIENT_H_
#define HTTP_MULTI_CLIENT_H_

#include <curl/multi.h>
#include <functional>
#include <string>
#include <vector>
#include "http_resource.h"
#include "http_client.h"


namespace nestor {
namespace net {

class HttpMultiClient {
public:
    HttpMultiClient();
    virtual ~HttpMultiClient();

    void appendRequestResource(const std::string &host, const std::string &resource);
    std::vector<HttpResource *> *perform();

private:
    CURLM *multiHandle_;
    std::vector<HttpClient *> clients_;
    int runningStatus_;
    bool finished_;
};

} /* namespace net */
} /* namespace nestor */

#endif /* HTTP_MULTI_CLIENT_H_ */
