#include <vector>
#include <algorithm>
#include <cstring>
#include <unicode/unistr.h>
#include <unicode/bytestream.h>
#include "channels_update_worker.h"
#include "sqlite_provider.h"
#include "net/http_multi_client.h"
#include "net/http_resource.h"
#include "rss/rss_xml_parser.h"
#include "logger.h"
#include "utils/string.h"

using namespace std;
using namespace nestor::net;
using namespace nestor::utils;

namespace nestor {
namespace service {

ChannelsUpdateWorker::ChannelsUpdateWorker(const vector<int64_t> &channelsID,
                                           SqliteConnection *connection) {
    databaseConnection_ = connection;
    channelsID_.clear();
    channelsID_.resize(channelsID.size());
    copy(channelsID.cbegin(), channelsID.cend(), channelsID_.begin());
}

ChannelsUpdateWorker::~ChannelsUpdateWorker() {
}


void ChannelsUpdateWorker::run() {
    SqliteProvider dataProvider(databaseConnection_);
    HttpMultiClient downloader;
    int feedsDownloadCount = 0;
    vector<HttpResource *> receivedResources;

    // Adding channels urls into downloader
    for (int64_t channelId : channelsID_) {
        Channel *channel;
        // Errors just skipping
        try {
            channel = dataProvider.findChannelById(channelId);
        } catch (SqliteProviderException &e) {
            SERVICE_LOG_LVL(ERROR, "ChannelsUpdateWorker::run: error while finding"
                            " channel with id: " << channelId << ". Message: "
                            << e.what());
            continue;
        }
        if (channel == nullptr) {
            SERVICE_LOG_LVL(WARN, "ChannelsUpdateWorker::run: cannot find channel "
                            "with id: " << channelId);
            continue;
        }

        downloader.appendRequestResource(channel->link());
        feedsDownloadCount++;
    }

    // Perform downloading and parsing feeds.
    SERVICE_LOG_LVL(DEBUG, "ChannelsUpdateWorker::run: starting downloading "
                    << feedsDownloadCount << " resources");
    do {
        vector<HttpResource *> *recved = downloader.perform();
        if (recved == nullptr) {
            SERVICE_LOG_LVL(ERROR, "ChannelsUpdateWorker::run: cannot download resources");
            break;
        }

        SERVICE_LOG_LVL(DEBUG, "ChannelsUpdateWorker::run: received " << recved->size() << " resources");

        // While others feeds are still downloading we parse already downloaded feeds.
        for (HttpResource *res : *recved) {
            SERVICE_LOG_LVL(DEBUG, "ChannelsUpdateWorker::run: start parsing url=" << res->url());

            string contentType = res->contentType();
            stringToLower(contentType);
            trim(contentType);

            if (contentType.compare("application/rss+xml") != 0) {
                SERVICE_LOG_LVL(ERROR, "ChannelsUpdateWorker::run: wrong content type \"" << contentType << "\"");
                continue;
            }

            string charset = res->contentCharset();
            stringToLower(charset);
            trim(charset);
            if (charset.compare("utf-8") != 0) {
                SERVICE_LOG("ChannelsUpdateWorker::run: charset=" << charset <<
                            " is not utf-8. Trying to convert to utf-8 charset.");
                icu::UnicodeString ustr(reinterpret_cast<char *>(res->content()), res->contentLength(), charset.c_str());

                string dst;
                StringByteSink<string> sink(&dst);
                ustr.toUTF8(sink);

                unsigned char *data = new unsigned char[dst.length() + 1];
                memcpy(data, dst.c_str(), dst.length() + 1);
                delete[] res->content();
                res->setContent(data);
                res->setContentLength(dst.length());
            }
        }
    } while(0);
}

} /* namespace service */
} /* namespace nestor */
