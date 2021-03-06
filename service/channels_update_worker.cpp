#include <vector>
#include <map>
#include <algorithm>
#include <cstring>
#include <memory>
#include <iostream>
#include <unicode/unistr.h>
#include <unicode/bytestream.h>
#include "channels_update_worker.h"
#include "sqlite_provider.h"
#include "net/http_multi_client.h"
#include "net/http_resource.h"
#include "rss/rss_channel.h"
#include "rss/rss_xml_parser.h"
#include "common/logger.h"
#include "utils/string.h"

using namespace std;
using namespace nestor::net;
using namespace nestor::utils;
using namespace nestor::rss;

namespace nestor {
namespace service {


ChannelsUpdateWorker::ChannelsUpdateWorker(const vector<int64_t> &channelsID,
                                           SqliteConnection *connection)
        : dataProvider_(nullptr) {
    databaseConnection_ = connection;
    channelsID_.clear();
    channelsID_.resize(channelsID.size());
    copy(channelsID.cbegin(), channelsID.cend(), channelsID_.begin());
}


ChannelsUpdateWorker::~ChannelsUpdateWorker() {
    if (dataProvider_)
        delete dataProvider_;
}


/**
 * Checks content charset and if it is not UTF-8 will try to convert to it.
 * @param resource HTTP resource for checking
 */
void ChannelsUpdateWorker::convertContentCharsetIfNeed(HttpResource* resource) {
    string charset = resource->contentCharset();
    stringToLower(charset);
    trim(charset);
    if (charset.compare("utf-8") != 0) {
        SERVICE_LOG("ChannelsUpdateWorker::convertContentCharsetIfNeed: charset="
                    << charset << " is not utf-8. Trying to convert to utf-8 charset.");
        icu::UnicodeString ustr(reinterpret_cast<char *>(resource->content()),
                                resource->contentLength(), charset.c_str());

        string dst;
        StringByteSink<string> sink(&dst);
        ustr.toUTF8(sink);

        unsigned char *data = new unsigned char[dst.length() + 1];
        memcpy(data, dst.c_str(), dst.length() + 1);
        delete[] resource->content();
        resource->setContent(data);
        resource->setContentLength(dst.length());
    }
}


/**
 * Updates RSS channel info with feeds.
 * @param channel Parsed RSS channel object
 */
void ChannelsUpdateWorker::updateRssChannel(RssChannel* channel,
                                            HttpResource* resource, int64_t channelId) {
    unique_ptr<Channel> dbchannel;

    try {
        dataProvider_->beginTransaction();
    } catch (SqliteProviderException &e) {
        SERVICE_LOG_LVL(ERROR, "ChannelsUpdateWorker::updateRssChannel: cannot "
                        "start transaction. Message: " << e.what());
        return;
    }

    try {
        dbchannel = unique_ptr<Channel>(dataProvider_->findChannelById(channelId));
    } catch (SqliteProviderException &e) {
        SERVICE_LOG_LVL(ERROR, "ChannelsUpdateWorker::updateRssChannel: error "
                        "while finding" " channel with id: " << channelId
                        << ". Message: " << e.what());
        return;
    }

    dbchannel->setDescription(channel->description());
    dbchannel->setLink(channel->link());
    dbchannel->setTitle(channel->title());

    std::time_t now = time(nullptr);
    std::tm *nowtm = localtime(&now);

    if (nowtm != nullptr)
        dbchannel->setLastUpdate(*nowtm);

    // Storing updated dbchannel
    bool rc;
    try {
        rc = dataProvider_->updateChannel(*dbchannel);
    } catch (SqliteProviderException &e) {
        SERVICE_LOG_LVL(ERROR, "ChannelsUpdateWorker::updateRssChannel: error "
                        "while updating channel with id: " << channelId
                        << ". Message: " << e.what());
        return;
    }

    if (!rc) {
        SERVICE_LOG_LVL(ERROR, "ChannelsUpdateWorker::updateRssChannel: error "
                        "while updating channel with id: " << channelId
                        << " Data hasn't stored.");
        return;
    }

    unsigned int postNum = channel->itemsCount();
    for (unsigned int i = 0; i < postNum; i++) {
        RssObject *post = channel->getItem(i);
        updateRssObject(*post, *dbchannel);
    }

    try {
        dataProvider_->endTransaction();
    } catch (SqliteProviderException &e) {
        SERVICE_LOG_LVL(ERROR, "ChannelsUpdateWorker::updateRssChannel: cannot "
                        "end transaction. Message: " << e.what());
        return;
    }
}


/**
 * Updates RSS post of channel feed.
 */
int64_t ChannelsUpdateWorker::updateRssObject(RssObject &post,
                                           Channel &channel) {
    unique_ptr<Post> dbpost(nullptr), existPost(nullptr);
    try {
        dbpost = unique_ptr<Post>(dataProvider_->findPostByGuid(post.guid()));
    } catch (SqliteProviderException &e) {
        SERVICE_LOG_LVL(ERROR, "ChannelsUpdateWorker::updateRssObject: error "
                        "while finding post with guid: " << post.guid() <<
                        " Message: " << e.what());
        return -1;
    }


    bool isUpdate = true;
    if (dbpost == nullptr || dbpost->channelId() != channel.id()) {
        if (dbpost) {
            isUpdate = true;
        }
        else {
            isUpdate = false;
        }
        // Cannot find post with GUID. Storing as new post
        if (dbpost == nullptr)
            dbpost = unique_ptr<Post>(new Post());
    }

    dbpost->setGuid(post.guid());
    dbpost->setChannelId(channel.id());
    dbpost->setLink(post.link());
    dbpost->setTitle(post.title());

    // TODO: Make Description and Text different
    dbpost->setDescription(post.text());
    dbpost->setText(post.text());

    dbpost->setPublicationDate(post.pubDate());

    bool rc;
    int64_t ret;

    if (isUpdate) {
        try {
            existPost = unique_ptr<Post>(dataProvider_->findPostByGuid(dbpost->guid()));

            time_t oldTime, newTime;
            tm t1 = existPost->publicationDate(), t2 = dbpost->publicationDate();

            oldTime = mktime(&t1);
            newTime = mktime(&t2);

            if (abs(difftime(oldTime, newTime)) < UPDATE_POST_TIME_THRESHOLD_SEC) {
                SERVICE_LOG_LVL(DEBUG, "ChannelsUpdateWorker::updateRssObject: post " << existPost->guid() << " is not changed");
                ret = existPost->id();
                return ret;
            }

            rc = dataProvider_->updatePost(*dbpost.get());
        } catch (SqliteProviderException &e) {
            SERVICE_LOG_LVL(ERROR, "ChannelsUpdateWorker::updateRssObject: error "
                            "while updating post with guid: " << post.guid() <<
                            " id: " << dbpost->id() <<
                            " Message: " << e.what());
            return -1;
        }
        if (rc) {
            ret = dbpost->id();
        } else {
            SERVICE_LOG_LVL(ERROR, "ChannelsUpdateWorker::updateRssObject: cannot "
                           "update post with guid: " << post.guid() <<
                           " id: " << dbpost->id());
            ret = -1;
        }
    } else {
        try {
            ret = dataProvider_->insertPost(*dbpost.get());
        } catch (SqliteProviderException &e) {
            SERVICE_LOG_LVL(ERROR, "ChannelsUpdateWorker::updateRssObject: error "
                            "while inserting post with guid: " << post.guid() <<
                            " Message: " << e.what());
            return -1;
        }
    }

    return ret;
}


void ChannelsUpdateWorker::run() {
    dataProvider_ = new SqliteProvider(databaseConnection_);
    HttpMultiClient downloader;
    int feedsDownloadCount = 0;
    map<string, int64_t> urlIds;

    // Adding channels urls into downloader
    for (int64_t channelId : channelsID_) {
        Channel *channel;
        // Errors just skipping
        try {
            channel = dataProvider_->findChannelById(channelId);
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

        downloader.appendRequestResource(channel->rssLink());
        urlIds.insert(make_pair(channel->rssLink(), channelId));
        feedsDownloadCount++;
        delete channel;
    }

    // Perform downloading and parsing feeds.
    SERVICE_LOG_LVL(DEBUG, "ChannelsUpdateWorker::run: starting downloading "
                    << feedsDownloadCount << " resources");
    vector<HttpResource *> *recved = nullptr;
    do {
        if (recved) {
            for (HttpResource *res : *recved)
                delete res;
            delete recved;
            recved = nullptr;
        }
        recved = downloader.perform();
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

            SERVICE_LOG_LVL(DEBUG, "ChannelsUpdateWorker::run: checking content charset url=" << res->url());
            convertContentCharsetIfNeed(res);

            SERVICE_LOG_LVL(DEBUG, "ChannelsUpdateWorker::run: parsing RSS feed url=" << res->url());
            char *content = reinterpret_cast<char *>(res->content());
            RssChannel *channel;
            try {
                channel = RssXmlParser::parseRss(content);
            } catch (RssXmlParserException &e) {
                SERVICE_LOG_LVL(ERROR, "ChannelsUpdateWorker::run: error while "
                                "parsing RSS feed: url=" << res->url() <<
                                " message=" << e.what());
                continue;
            }

            SERVICE_LOG_LVL(DEBUG, "ChannelsUpdateWorker::run: updating RSS channel url=" << res->url());
            updateRssChannel(channel, res, urlIds.at(res->requestUrl()));
        }
    } while(recved->size() > 0);

    if (recved)
        delete recved;

    delete dataProvider_;
    dataProvider_ = nullptr;
}

} /* namespace service */
} /* namespace nestor */
