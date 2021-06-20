#ifndef INCLUDED_CLIENT_USER_EVENTS_HPP
#define INCLUDED_CLIENT_USER_EVENTS_HPP

/* TODO: Yes, this is a terrible name for a file and the majority of this program needs a refactor.
   The events in this file are emitted by Client, whereas those in events.hpp are emitted by
   Connection_manager and are used only internally */

#include<string>
#include"connection/connection.hpp"

namespace Network {
    namespace Events {
        struct Connect_result {
            bool const success;
            std::string error_message;
        };

        struct Auth_result {
            bool const success;
            std::string error_message;
        };

        struct Connection_lost {
            std::string error_message;
        };

        struct Player_status_change {
            Connection::Id const id;
            enum struct Status { connected, disconnected } const status;
        };

    }
}

#endif
