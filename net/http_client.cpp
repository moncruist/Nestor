/*
 *  This file is part of Nestor.
 *
 *  Nestor - program for aggregation RSS subscriptions providing
 *  access via IMAP interface.
 *  Copyright (C) 2013-2014  Konstantin Zhukov
 *
 *  Nestor is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  Nestor is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see {http://www.gnu.org/licenses/}.
 */
#include "http_client.h"
#include <string>
#include <iostream>
#include <sstream>
#include <cstring>

#include <curl/curl.h>
#include "logger.h"
#include "http_response_parser.h"
#include "utils/string.h"

using namespace std;
using namespace nestor::utils;

namespace nestor {
namespace net {


HttpClient::HttpClient(string host)
        : host_(host), recvBuffer_(nullptr), recvBufferSize_(0) {
    handle_ = curl_easy_init();
    if (handle_ == nullptr) {
        string errmsg = "HttpClient::HttpClient: cannot initialize curl handle";
        NET_LOG_LVL(ERROR, errmsg);
        throw runtime_error(errmsg);
    }
}

HttpResource * HttpClient::getResource(const string &resource) {
    setup(resource);

    if (!perform())
        return nullptr;

    return parseReceivedData();
}

void HttpClient::setup(const std::string &resource) {
    string url = host_ + resource;
    curl_easy_setopt(handle_, CURLOPT_URL, url.c_str());
    curl_easy_setopt(handle_, CURLOPT_WRITEFUNCTION, writeFuncHelper);
    curl_easy_setopt(handle_, CURLOPT_WRITEDATA, this);
}

bool HttpClient::perform() {
    int resultCode = curl_easy_perform(handle_);

    if (resultCode != CURLE_OK) {
        ostringstream ossErr;
        ossErr << "HttpClient::getResource: receive failed. Curl code: " << resultCode
                << "; Description: " << curl_easy_strerror((CURLcode)resultCode);
        NET_LOG_LVL(ERROR, ossErr.str());
        return false;
    }

    return true;
}

CURL* HttpClient::handle() const {
    return handle_;
}

HttpResource* HttpClient::parseReceivedData() {
    if (recvBuffer_ == nullptr)
        return nullptr;

    HttpResource *res = new HttpResource();

    // moving recvBuffer_ to res
    res->setContent(recvBuffer_);
    res->setContentLength(recvBufferSize_);
    recvBuffer_ = nullptr;
    recvBufferSize_ = 0;

    long respCode;
    curl_easy_getinfo(handle_, CURLINFO_RESPONSE_CODE, &respCode);
    res->setCode(respCode);

    char *contentType;
    curl_easy_getinfo(handle_, CURLINFO_CONTENT_TYPE, &contentType);
    if (contentType != nullptr) {
        string type = contentType;

        vector<string> typeParts, params;
        split(type, ";", typeParts);
        int typePartsSize = typeParts.size();

        res->setContentType(typeParts[0]);

        if (typePartsSize > 1) {
            for (int i = 1; i < typePartsSize; i++) {
                split(typeParts[i], "=", params);
                if (params.size() != 2) continue;

                if (stringToLowerCopy(trim(params[0])) == "charset") {
                    res->setContentCharset(stringToLowerCopy(trim(params[1])));
                }
            }
        }
    } else {
        res->setContentType("");
        res->setContentCharset("");
    }

    return res;
}

size_t HttpClient::writeFuncHelper(void* ptr, size_t size, size_t nmemb, void* userdata) {
    HttpClient *obj = static_cast<HttpClient *>(userdata);
    size_t startPos = 0;
    size_t totalBytes = size * nmemb;

    if (totalBytes == 0) return 0;

    if (obj->recvBuffer_ == nullptr) {
        // allocating new buffer
        obj->recvBuffer_ = new unsigned char[totalBytes];
        obj->recvBufferSize_ = size * nmemb;
    } else {
        // rellocating buffer with bigger size
        unsigned char *newRecvBuffer = new unsigned char [obj->recvBufferSize_ + totalBytes];
        memcpy(newRecvBuffer, obj->recvBuffer_, obj->recvBufferSize_);
        delete[] obj->recvBuffer_;
        obj->recvBuffer_ = newRecvBuffer;

        startPos = obj->recvBufferSize_;
        obj->recvBufferSize_ += totalBytes;
    }

    memcpy(&obj->recvBuffer_[startPos], ptr, totalBytes);
    return totalBytes;
}


HttpClient::~HttpClient() {
    if (recvBuffer_)
        delete[] recvBuffer_;
    curl_easy_cleanup(handle_);
}

} /* namespace net */
} /* namespace nestor */

