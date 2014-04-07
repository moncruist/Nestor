#ifndef IMAP_STRING_H_
#define IMAP_STRING_H_

#include <string>
#include <exception>
#include <stdexcept>

namespace nestor {
namespace imap {


enum class ImapStringType {
    UNSPECIFIED, // when we haven't determine string type yet
    QUOTED,      // quoted string type, i.e. "Hello"
    LITERAL,      // literal string type i.e. {5}CRLFHello
    LITERAL_NONSYNC // literal nonsynchronized string i.e. {5+}CRLFHello
};

enum class ImapStringStatus {
    INVALID,
    UNCOMPLETED,
    COMPLETED
};

class ImapString {
public:
    ImapString();
    virtual ~ImapString();

    void reset();
    int addBufferToParse(std::string &buffer);
    ImapStringStatus status() const;

    void appendString(std::string str);
    std::string data() const;

    ImapStringType type() const;
    void setType(ImapStringType type);

    std::string toRawData();
    std::string toRawData(ImapStringType exportType);

    unsigned int length() const;
    unsigned int parsedLength() const;

private:
    static ImapStringType detectType(const std::string &buffer, int *pos, int *stringLength);

private:
    ImapStringStatus status_;
    ImapStringType type_;

    unsigned int length_;
    unsigned int parsedLength_;
    std::string data_;

    bool prefixStarted_;
    bool prefixEnded_;
    bool literalCrLfParsed_;
};

} /* namespace imap */
} /* namespace nestor */

#endif /* IMAP_STRING_H_ */
