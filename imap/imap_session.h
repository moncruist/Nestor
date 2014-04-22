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
#include <map>
#include <queue>
#include <mutex>
#include <functional>

#include "net/socket_single.h"
#include "service/service.h"

namespace nestor {
namespace imap {

enum class ImapSessionState {
    START,      // Start state
    CONNECTED,  // client just connected
    NON_AUTH,   // client has not  authenticated
    AUTH,       // client has authenticated and should choose folder
    WORK,       // client has choosed folder
    EXIT        // closed session
};

struct ImapCommand {
    std::string tag;
    std::string name;
    std::string params;
};

class ImapSession {
// typedefs
public:
    using CallbackFunction = std::function<void (nestor::imap::ImapSession *)>;
public:

    /**
     * ImapSession constructor. Requires valid service and socket objects for
     * work. Takes ownership of this objects.
     * @param service Pointer to the service object for getting data for response
     * @param socket Pointer to the socket, which is used as transport layer.
     */
    ImapSession(service::Service *service, net::SocketSingle *socket);
    virtual ~ImapSession();

    void processData();
    void writeAnswers();
    const net::SocketSingle *socket() const;
    const service::Service *service() const;

    ImapSessionState state() const;
    void setOnExitCallback(CallbackFunction callback);

private:
    std::string greetingString() const;
    void rejectUnknownCommand(ImapCommand *command);
    void rejectBad(ImapCommand *command, const std::string &comment);
    void rejectNo(ImapCommand *command, const std::string &comment);
    void switchState(ImapSessionState newState);

    /* Command processing functions. Should meets CommandParserFunction
     * signature. After successful work every function should write command
     * answer to the answersData_. Each function shouldn't aquire mutex
     * sessionLock_ because it's already locked.
     * Input - ImapCommand structure with filled tag and name fields.
     * Output - last meaningful symbol position of command in incomingData_
     * or -1 if command is incomplete.  */
    int processCapability(ImapCommand *command);
    int processNoop(ImapCommand *command);
    int processLogout(ImapCommand *command);
    int processAuthenticate(ImapCommand *command);
    int processLogin(ImapCommand *command);



private:
    typedef int (ImapSession::*CommandParserFunction)(ImapCommand *);
    static const std::map<std::string, CommandParserFunction> parserFunctions;

    ImapSessionState state_;
    std::string incomingData_;
    std::string answersData_;
    std::queue<ImapCommand *> completedCommands_;
    std::mutex sessionLock_;

    service::Service *service_;
    net::SocketSingle *socket_;
    CallbackFunction onExitCallback_;
};

} /* namespace imap */
} /* namespace nestor */

#endif /* IMAP_SESSION_H_ */
