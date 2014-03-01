#include "imap_parser.h"

using namespace std;

namespace nestor {
namespace imap {

std::string ImapParser::parseImapString(const std::string& rawData, int &lastIndex) {
    lastIndex = -1;
    if (rawData.length() == 0) return "";

    enum ImapStringType {
        UNDEFINED,
        LITERAL,
        QUOTED
    };

    string resultString = "";
    
    int stringType = UNDEFINED;

    bool prefixStarted = false;
    bool prefixEnded = false;
    string prefixLength = "";
    int prefixStringLength = 0;

    int actualStringLength = 0;

    for(int i = 0; i < rawData.length(); i++) {
        char c = rawData[i];
        switch(stringType) {
        case UNDEFINED:
            if (c == '{') {
                prefixStarted = true;
                stringType = LITERAL;
            }
            if (c == '"') {
                stringType = QUOTED;
            }
            break;

        case LITERAL:
            if (prefixStarted && prefixEnded) {
                resultString += c;
                actualStringLength++;
                if (actualStringLength == prefixStringLength) {
                    lastIndex = i;
                    return resultString;
                }
            } else {
                if (c != '}') {
                    if (!isdigit(c)) {
                        prefixStarted = false;
                        prefixEnded = false;
                        prefixLength = "";
                        stringType = UNDEFINED;
                        continue;
                    }

                    prefixLength += c;
                } else {
                    prefixEnded = true;
                    prefixStringLength = strtol(prefixLength.c_str(), nullptr,
                                                10);
                    if (prefixStringLength == 0) {
                        prefixStarted = false;
                        prefixEnded = false;
                        prefixLength = "";
                        stringType = UNDEFINED;
                        continue;
                    }

                    if (prefixStringLength > rawData.length() - i - 1) {
                        lastIndex = -1;
                        
                        return "";
                    }
                }
            }
            break;

        case QUOTED:
            if (c != '"') {
                resultString += c;
                actualStringLength++;
            } else {
                lastIndex = i;
                return resultString;
            }
            break;
        }

    }
    
    lastIndex = -1;
    return "";
}

std::string ImapParser::packImapString(const std::string& imapString) {
    return "";
}

} /* namespace imap { */
} /* namespace nestor { */
