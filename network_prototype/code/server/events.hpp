#ifndef INCLUDED_SERVER_EVENTS_HPP
#define INCLUDED_SERVER_EVENTS_HPP

#include"server/connection.hpp"

namespace Network {
    namespace Server {
        namespace Events {
            struct Connection_established {
                Connection::Id const id;
                boost::asio::ip::address const local_endpoint_address;
                boost::asio::ip::port_type const local_endpoint_port;
                boost::asio::ip::address const remote_endpoint_address;
                boost::asio::ip::port_type const remote_endpoint_port;
            };

            struct Connection_killed {
                Connection::Id const id;
                std::string const reason;
            };

            struct Packet_received {
                Connection::Id const from;
                std::unique_ptr<Protocol::Inbound_packet const> packet;
            };
        }
    }
}

#endif