#include <stdexcept>
#include <sstream>
#include "http_multi_client.h"
#include "common/logger.h"

using namespace std;

namespace nestor {
namespace net {

HttpMultiClient::HttpMultiClient() {
    runningStatus_ = -1;
    finished_ = false;
    multiHandle_ = curl_multi_init();
    if (multiHandle_ == nullptr) {
        string errmsg = "HttpMultiClient::HttpMultiClient: cannot create libcurl multi handle";
        NET_LOG_LVL(ERROR, errmsg);
        throw runtime_error(errmsg);
    }
}

HttpMultiClient::~HttpMultiClient() {
    for (HttpClient *c : clients_) {
        curl_multi_remove_handle(multiHandle_, c->handle());
        delete c;
    }
    clients_.clear();
    clientResources_.clear();
    curl_multi_cleanup(multiHandle_);
}


void HttpMultiClient::appendRequestResource(const std::string& resource) {
    HttpClient *client = new HttpClient();
    client->setup(resource);
    clients_.push_back(client);

    curl_multi_add_handle(multiHandle_, client->handle());
    clientResources_.insert(make_pair(client, resource));
}

std::vector<HttpResource*>* HttpMultiClient::perform() {
    if (finished_)
        return new vector<HttpResource *>(); // Just empty vector, nullptr is for error

    vector<HttpResource *> *resources = nullptr;

    if (runningStatus_ == -1)
        runningStatus_ = clients_.size();
    int prevStatus = runningStatus_;
    while (prevStatus == runningStatus_) {
        int rc = curl_multi_perform(multiHandle_, &runningStatus_);

        if (rc != CURLM_OK) {
            NET_LOG_LVL(ERROR, "HttpMultiClient::perform: curl multi perform failed with code " << rc
                        << ": " << curl_multi_strerror((CURLMcode)rc));
            return nullptr;
        }

        resources = new vector<HttpResource *>();
        if (runningStatus_ != prevStatus) {
            CURLMsg *msg;
            int msgsLeft;
            while ((msg = curl_multi_info_read(multiHandle_, &msgsLeft))) {
                if (msg->msg == CURLMSG_DONE) {
                    HttpClient *doneClient = nullptr;
                    for (HttpClient *c : clients_) {
                        if (c->handle() == msg->easy_handle) {
                            doneClient = c;
                            break;
                        }
                    }

                    HttpResource *res = doneClient->parseReceivedData();
                    res->setRequestUrl(clientResources_[doneClient]);
                    resources->push_back(res);
                }
            }
        }


        if (runningStatus_ == 0) {
            finished_ = true;
            break;
        }

        rc = curl_multi_wait(multiHandle_, nullptr, 0, 1000, nullptr);
        if (rc != CURLM_OK) {
            NET_LOG_LVL(ERROR, "HttpMultiClient::perform: curl multi wait failed with code " << rc
                        << ": " << curl_multi_strerror((CURLMcode)rc));
            return nullptr;
        }
    }


    NET_LOG_LVL(DEBUG, "HttpMultiClient::perform: clients ready: " << resources->size());

    return resources;
}

} /* namespace net */
} /* namespace nestor */
