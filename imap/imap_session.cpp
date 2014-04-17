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
#include "imap_session.h"
#include "utils/string.h"

using namespace std;
using namespace nestor::utils;
using namespace nestor::service;

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

ImapSession::ImapSession(Service &service)
        : service_(service) {
    switchState(ImapSessionState::CONNECTED);
}

ImapSession::~ImapSession() {
}

void ImapSession::processData(const std::string& data) {
    lock_guard<mutex> lock(sessionLock_);
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
            incomingData_.erase(0, crlfPos + 2); // deleting wrong line from buffer
            if (tag) delete tag;
            if (commandName) delete commandName;
            continue;
        }

        ImapCommand *command = new ImapCommand();
        command->tag = *tag;
        command->name = *commandName;

        stringToUpper(command->name);

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

        if (tag) delete tag;
        if (commandName) delete commandName;
    }
}

std::string ImapSession::getAnswers() {
    lock_guard<mutex> lock(sessionLock_);
    string copy = answersData_;
    answersData_.clear();
    return copy;
}

bool ImapSession::answersReady() {
    return answersData_.length() > 0;
}

std::string ImapSession::greetingString() const {
    return "* OK IMAP4revl server ready" CRLF;
}


/* CAPABILITY command */
int ImapSession::processCapability(ImapCommand* command) {
    size_t crlfPos = incomingData_.find(CRLF);
    string line = incomingData_.substr(0, crlfPos);

    /* Check command syntax */
    if (line.length() != command->tag.length() + 1 /* whitespace */ + command->name.length()) {
        rejectUnknownCommand(command);
        return crlfPos + 1; /* last position in command */
    }

    ostringstream oss;
    oss << "* CAPABILITY IMAP4rev1 LITERAL+ AUTH=PLAIN" << CRLF << command->tag << " OK CAPABILITY completed" << CRLF;
    answersData_.append(oss.str());
    return crlfPos + 1;
}


/* NOOP command */
int ImapSession::processNoop(ImapCommand* command) {
    size_t crlfPos = incomingData_.find(CRLF);
    string line = incomingData_.substr(0, crlfPos);

    /* Check command syntax */
    if (line.length() != command->tag.length() + 1 /* whitespace */ +
            command->name.length()) {
        rejectUnknownCommand(command);
        return crlfPos + 1; /* last position in command */
    }

    ostringstream oss;
    oss << command->tag << " OK " << command->name << " completed" << CRLF;
    answersData_.append(oss.str());
    return crlfPos + 1;
}


/* LOGOUT command */
int ImapSession::processLogout(ImapCommand *command) {
    size_t crlfPos = incomingData_.find(CRLF);
    string line = incomingData_.substr(0, crlfPos);

    /* Check command syntax */
    if (line.length() != command->tag.length() + 1 /* whitespace */ +
            command->name.length()) {
        rejectUnknownCommand(command);
        return crlfPos + 1; /* last position in command */
    }

    service_.onLogout();

    ostringstream oss;
    oss << "* BYE IMAP4rev1 Server logging out" << CRLF << command->tag
            << " OK " << command->name << " completed" << CRLF;
    answersData_.append(oss.str());
    return crlfPos + 1;
}

void ImapSession::rejectUnknownCommand(ImapCommand* command) {
    ostringstream oss;
    oss << command->tag << " BAD Unknown command \"" << command->name << "\"" << CRLF;
    answersData_.append(oss.str());
}

/**
 * Switches the state of session
 * @param newState
 */
void ImapSession::switchState(ImapSessionState newState) {
    if (newState == state_)
        return; // Same state, nothing to do.

    if (newState == ImapSessionState::CONNECTED)
        answersData_.append(greetingString());

    state_ = newState;
}

} /* namespace imap */
} /* namespace nestor */
