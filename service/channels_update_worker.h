#ifndef CHANNELS_UPDATE_WORKER_H_
#define CHANNELS_UPDATE_WORKER_H_

#include <vector>
#include <cstdint>

#include "sqlite_connection.h"

namespace nestor {

/* ======== BEGIN ============= */
/* Forward declaration of classes needed by ChannelsUpdateWorker private
 * methods */
namespace net {
class HttpResource;
}
namespace rss {
class RssChannel;
class RssObject;
}
namespace service {
class SqliteProvider;
class Channel;
}

/* ======== END =============== */

namespace service {

/**
 * Worker class for updating feeds. Update consists of downloading,
 * parsing and storing feeds.
 */
class ChannelsUpdateWorker {
public:
    ChannelsUpdateWorker(const std::vector<int64_t> &channelsID, SqliteConnection *connection);
    virtual ~ChannelsUpdateWorker();

    /**
     * Perform updating feeds.
     */
    void run();

private:
    std::vector<int64_t> channelsID_;
    SqliteConnection *databaseConnection_;
    SqliteProvider *dataProvider_;

private:


    void convertContentCharsetIfNeed(nestor::net::HttpResource* resource);
    void updateRssChannel(nestor::rss::RssChannel *channel, nestor::net::HttpResource* resource, int64_t channelId);
    int64_t updateRssObject(nestor::rss::RssObject *post, Channel *channel);
};

} /* namespace service */
} /* namespace nestor */

#endif /* CHANNELS_UPDATE_WORKER_H_ */
