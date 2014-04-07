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
#ifndef IMAP_SESSION_H_
#define IMAP_SESSION_H_

#include <string>
#include <queue>
#include <mutex>

namespace nestor {
namespace imap {

enum class ImapSessionState {
    CONNECTED,  // client just connected
    NON_AUTH,   // client has not  authenticated
    AUTH,       // client has authenticated and should choose folder
    WORK,       // client has choosed folder
    EXIT        // closed session
};

struct ImapCommand {
    std::string uid;
    std::string command;
    std::string params;
};

class ImapSession {
public:
    ImapSession();
    virtual ~ImapSession();

    void proccessData(const std::string &data);
    std::string getAnswers();

    bool answersReady();

private:
    std::string greetingString() const;
    ImapCommand *parseCommand(std::string &rawData);

private:
    static std::string parseImapString(const std::string &rawData, int &lastIndex);
    static std::string packImapString(const std::string &imapString);

private:
    ImapSessionState state_;
    std::queue<ImapCommand *> commands_;
    bool ready_;
    std::mutex sessionLock_;
};

} /* namespace imap */
} /* namespace nestor */

#endif /* IMAP_SESSION_H_ */
