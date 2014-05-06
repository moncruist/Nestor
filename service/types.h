#ifndef TYPES_H_
#define TYPES_H_

#include <string>
#include <ctime>

namespace nestor {
namespace service {

/**
 * POD class representing User object.
 */
class User {
public:
    long long id() const;
    void setId(long long id);
    const std::string& password() const;
    void setPassword(const std::string& password);
    const std::string& username() const;
    void setUsername(const std::string& username);

private:
    long long id_;
    std::string username_;
    std::string password_;
};


/**
 * POD class representing Channel object.
 */
class Channel {
public:
    const std::string& description() const;
    void setDescription(const std::string& description);
    long long id() const;
    void setId(long long id);
    std::tm lastUpdate() const;
    void setLastUpdate(std::tm lastUpdate);
    const std::string& link() const;
    void setLink(const std::string& link);
    const std::string& rssLink() const;
    void setRssLink(const std::string& rssLink);
    const std::string& title() const;
    void setTitle(const std::string& title);
    int updateInterval() const;
    void setUpdateInterval(int updateInterval);

private:
    long long id_;
    std::string title_;
    std::string rssLink_;
    std::string link_;
    std::string description_;
    int updateInterval_;
    std::tm lastUpdate_;
};

}
}


#endif /* TYPES_H_ */
