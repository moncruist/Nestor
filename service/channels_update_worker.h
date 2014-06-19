#ifndef CHANNELS_UPDATE_WORKER_H_
#define CHANNELS_UPDATE_WORKER_H_

#include <vector>
#include <cstdint>

#include "sqlite_connection.h"

namespace nestor {
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
};

} /* namespace service */
} /* namespace nestor */

#endif /* CHANNELS_UPDATE_WORKER_H_ */
