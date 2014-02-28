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
#include "imap_session.h"
#include "utils/string.h"

using namespace std;

namespace nestor {
namespace imap {

ImapSession::ImapSession()
        : state_(ImapSessionState::CONNECTED), ready_(true) {
}

ImapSession::~ImapSession() {
    // TODO Auto-generated destructor stub
}

void ImapSession::proccessData(const std::string& data) {

}

std::string ImapSession::getAnswers() {
    lock_guard<mutex> lock(sessionLock_);

    switch(state_) {
    case ImapSessionState::CONNECTED:
        state_ = ImapSessionState::NON_AUTH;
        return greetingString();
        break;
    default:
        break;
    }

    return "";
}

bool ImapSession::answersReady() {
    return ready_;
}

std::string ImapSession::greetingString() const {
    return "* OK IMAP4revl server ready" CRLF;
}

} /* namespace imap */
} /* namespace nestor */
