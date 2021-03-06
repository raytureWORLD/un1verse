#ifndef INCLUDED_CLIENT_EVENTS_HPP
#define INCLUDED_CLIENT_EVENTS_HPP

#include"connection/connection.hpp"

namespace Network {
    namespace Client_impl {
        namespace Events {
            struct Connect_result {
                bool const success;
                std::string error_message;
            };

            struct Packet_received {
                Protocol::Inbound_packet packet;
            };

            struct Connection_lost {
                std::string error_message;
            };
        }
    }
}

#endif