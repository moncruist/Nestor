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
#include <vector>
#include <sstream>
#include "logger.h"
#include "imap_session.h"
#include "utils/string.h"

using namespace std;
using namespace nestor::utils;
using namespace nestor::service;
using namespace nestor::net;

namespace nestor {
namespace imap {

/*
 * Map of parsing methods. Command - parsing method.
 */
const std::map<std::string, ImapSession::CommandParserFunction>
ImapSession::parserFunctions = {
        {"CAPABILITY", &ImapSession::processCapability},
        {"NOOP", &ImapSession::processNoop},
        {"LOGOUT", &ImapSession::processLogout},
        {"AUTHENTICATE", &ImapSession::processAuthenticate},
        {"LOGIN", &ImapSession::processLogin}
};

/**
 * Retrieves tag from command line
 * @param data command data to parse
 * @return return tag or nullptr in case of error.
 */
static string* getCommandTag(const string &data) {
    if (data.length() == 0 || !isalnum(data[0]))
        return nullptr;
    size_t spacePos = data.find(' ', 0);
    if (spacePos == string::npos)
        return nullptr;
    string *result = new string();
    result->assign(data.begin(), data.begin() + spacePos);
    return result;
}


/**
 * Retrieves command name from command line
 * @param data command data to parse
 * @return return command name or nullptr in case of error.
 */
static string* getCommandName(const string &data) {
    if (data.length() == 0)
            return nullptr;

    vector<string> parts;
    split(data, " ", parts);

    if (parts.size() < 2)
        return nullptr;

    string *result = new string(parts[1]);
    return result;
}

ImapSession::ImapSession(service::Service *service, net::SocketSingle *socket)
        : state_(ImapSessionState::START), service_(service), socket_(socket),
          onExitCallback_(nullptr) {
    if (service_ == nullptr)
        throw invalid_argument("ImapSession::ImapSession: service is nullptr");
    if (socket_ == nullptr)
        throw invalid_argument("ImapSession::ImapSession: socket is nullptr");
    switchState(ImapSessionState::CONNECTED);
}

ImapSession::~ImapSession() {
    switchState(ImapSessionState::EXIT);
}

const net::SocketSingle* ImapSession::socket() const {
    return socket_;
}

const service::Service* ImapSession::service() const {
    return service_;
}

void ImapSession::processData() {
    lock_guard<mutex> lock(sessionLock_);

    string data = socket_->readAll();
    incomingData_.append(data);

    bool flag = true;
    while(flag) {
        size_t crlfPos = incomingData_.find(CRLF);

        // Checking for valid line
        if (crlfPos == string::npos)
            break;

        /* Getting line and parse her. Here we only do rough parsing,
         * detailed parsing goes in process<command> methods.*/
        string line = incomingData_.substr(0, crlfPos);
        string *tag, *commandName;


        tag = getCommandTag(line);
        commandName = getCommandName(line);

        if (!tag || !commandName) {
            ImapCommand com {line, "", ""};
            rejectBad(&com, "Missing command");

            incomingData_.erase(0, crlfPos + 2); // deleting wrong line from buffer
            if (tag) delete tag;
            if (commandName) delete commandName;
            continue;
        }

        ImapCommand *command = new ImapCommand();
        command->tag = *tag;
        command->name = *commandName;

        stringToUpper(command->name);

        LOG_LVL("ImapSession", DEBUG, "Received command: {" << command->tag << "," << command->name << "}");

        int commandEnd;

        if (parserFunctions.count(command->name)) {
            CommandParserFunction func =  parserFunctions.at(command->name);
            commandEnd = (this->*func)(command);
            if (commandEnd >= 0) {
                incomingData_.erase(0, commandEnd + 1);
            } else {
                /* Incomplete command. break */
                flag = false;
            }
        } else {
            rejectUnknownCommand(command);
            incomingData_.erase(0, crlfPos + 2); // deleting wrong line from buffer
        }

        writeAnswers();

        if (tag) delete tag;
        if (commandName) delete commandName;

        if (state_ == ImapSessionState::EXIT) {
            break;
        }
    }
}

std::string ImapSession::greetingString() const {
    return "* OK IMAP4revl server ready" CRLF;
}


/* CAPABILITY command */
int ImapSession::processCapability(ImapCommand* command) {
    size_t crlfPos = incomingData_.find(CRLF);
    string line = incomingData_.substr(0, crlfPos);

    int ret = crlfPos + 1; /* last position in command */

    /* Check command syntax */
    if (line.length() != command->tag.length() + 1 /* whitespace */ + command->name.length()) {
        rejectUnknownCommand(command);
        return ret;
    }

    ostringstream oss;
    oss << "* CAPABILITY IMAP4rev1 LITERAL+ AUTH=PLAIN" << CRLF << command->tag << " OK CAPABILITY completed" << CRLF;
    answersData_.append(oss.str());
    return ret;
}


/* NOOP command */
int ImapSession::processNoop(ImapCommand* command) {
    size_t crlfPos = incomingData_.find(CRLF);
    string line = incomingData_.substr(0, crlfPos);

    int ret = crlfPos + 1; /* last position in command */

    /* Check command syntax */
    if (line.length() != command->tag.length() + 1 /* whitespace */ +
            command->name.length()) {
        rejectUnknownCommand(command);
        return ret;
    }

    ostringstream oss;
    oss << command->tag << " OK " << command->name << " completed" << CRLF;
    answersData_.append(oss.str());
    return ret;
}


/* LOGOUT command */
int ImapSession::processLogout(ImapCommand *command) {
    size_t crlfPos = incomingData_.find(CRLF);
    string line = incomingData_.substr(0, crlfPos);

    int ret = crlfPos + 1; /* last position in command */

    /* Check command syntax */
    if (line.length() != command->tag.length() + 1 /* whitespace */ +
            command->name.length()) {
        rejectUnknownCommand(command);
        return ret;
    }

    service_->onLogout();

    ostringstream oss;
    oss << "* BYE IMAP4rev1 Server logging out" << CRLF << command->tag
            << " OK " << command->name << " completed" << CRLF;
    answersData_.append(oss.str());
    writeAnswers();
    switchState(ImapSessionState::EXIT);
    return ret;
}


/* AUTHENTICATE command */
int ImapSession::processAuthenticate(ImapCommand *command) {
    size_t crlfPos = incomingData_.find(CRLF);
    string line = incomingData_.substr(0, crlfPos);
    vector<string> commandParts;
    split(line, " ", commandParts);
    ostringstream oss; // for formatting

    int ret = crlfPos + 1;

    if (state_ != ImapSessionState::NON_AUTH) {
        rejectNo(command, "Wrong state");
        return ret;
    }

    if (commandParts.size() != 3) {
        oss << command->name << " Wrong arguments";
        rejectBad(command, oss.str());
        return ret;
    }

    /* We don't support any authentication mechanism yet */
    oss << "Unsupported authentication " << commandParts[2];
    rejectNo(command, oss.str());
    return ret;
}


/* LOGIN command */
int ImapSession::processLogin(ImapCommand *command) {
    size_t crlfPos = incomingData_.find(CRLF);
    string line = incomingData_.substr(0, crlfPos);
    vector<string> commandParts;
    split(line, " ", commandParts);
    ostringstream oss; // for formatting

    int ret = crlfPos + 1;

    if (state_ != ImapSessionState::NON_AUTH) {
        rejectNo(command, "Wrong state");
        return ret;
    }

    if (commandParts.size() != 4) {
        oss << command->name << " Wrong arguments";
        rejectBad(command, oss.str());
        return ret;
    }

    string username = commandParts[2];
    string password = commandParts[3];

    if (service_->authenticate(username, password)) {
        switchState(ImapSessionState::AUTH);
        oss << command->tag << " OK " << command->name << " completed" << CRLF;
        answersData_.append(oss.str());

        LOG_LVL("ImapSession", INFO, "User " << username << " successfully logged in");
    } else {
        rejectNo(command, "Invalid user name or password");
    }

    return ret;

}

void ImapSession::rejectUnknownCommand(ImapCommand* command) {
    ostringstream oss;
    oss << "Unknown command \"" << command->name << "\"";
    rejectBad(command, oss.str());
}

void ImapSession::rejectBad(ImapCommand *command, const std::string &comment) {
    ostringstream oss;
    oss << command->tag << " BAD " << comment << CRLF;
    answersData_.append(oss.str());
}

void ImapSession::rejectNo(ImapCommand *command, const std::string &comment) {
    ostringstream oss;
    oss << command->tag << " NO " << command->name << " " << comment << CRLF;
    answersData_.append(oss.str());
}

/**
 * Switches the state of session
 * @param newState
 */
void ImapSession::switchState(ImapSessionState newState) {
    if (newState == state_)
        return; // Same state, nothing to do.

    if (state_ == ImapSessionState::EXIT)
        return; // Exited session couldn't start again

    switch(newState) {
    case ImapSessionState::CONNECTED:
        answersData_.append(greetingString());
        state_ = ImapSessionState::NON_AUTH;
        break;

    case ImapSessionState::EXIT:
        /* Perfoming exit. Deleting service and socket. */
        delete service_;
        service_ = nullptr;
        socket_->close();
        delete socket_;
        socket_ = nullptr;

        state_ = newState;
        /* Notify others. */
        if (onExitCallback_)
            onExitCallback_(this);
        break;

    default:
        state_ = newState;
    }
}

void ImapSession::writeAnswers() {
    if (answersData_.length() > 0) {
        try {
            socket_->write(answersData_);
        } catch (SocketIOException &e) {
            LOG_LVL("ImapSession", WARN,
                    "Cannot write to the socket" << socket_->descriptor() << ": " << e.what());
        } catch (SocketTimeoutException &e) {
            LOG_LVL("ImapSession", WARN,
                    "Timeout socket " << socket_->descriptor() << "; " << e.what());
        }
        answersData_.clear();
    }
}

ImapSessionState ImapSession::state() const {
    return state_;
}

void ImapSession::setOnExitCallback(CallbackFunction callback) {
    onExitCallback_ = callback;
}

} /* namespace imap */
} /* namespace nestor */
