/*
 * http_client.h
 *
 *  Created on: 30 нояб. 2013 г.
 *      Author: moncruist
 */

#ifndef HTTP_CLIENT_H_
#define HTTP_CLIENT_H_

#include <string>
#include <stdexcept>
#include <vector>
#include <netdb.h>

namespace nestor {
namespace net {

class HttpClient {
public:
    HttpClient(std::string host = "localhost", bool manualConnect = false)
            throw (std::runtime_error);

    void connect() throw (std::runtime_error);

    void close();

    std::vector<unsigned char> *getResource(std::string resource);

    virtual ~HttpClient();
private:
    std::string host_;
    int sockFd_;
    bool manualConnect_;
    addrinfo *servinfo;

    static const int readbuf_size = 1024;
};

} /* namespace net */
} /* namespace nestor */
#endif /* HTTP_CLIENT_H_ */
