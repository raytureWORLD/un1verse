#ifndef INCLUDED_SERVER_CONNECTION_MANAGER_HPP
#define INCLUDED_SERVER_CONNECTION_MANAGER_HPP

#include<thread>
#include<cstddef>
#include<queue>
#include<memory>
#include<vector>
#include<mutex>
#include<unordered_map>
#include<string>
#include"protocol/inbound_packet.hpp"
#include"protocol/outbound_packet.hpp"
#include"events/sync_event_emitter.hpp"
#include"server/events.hpp"
#include"server/acceptor.hpp"

namespace Network {
    namespace Server_impl {
        class Connection_manager: 
            public Sync_event_emitter<
                Events::Connection_established,
                Events::Connection_killed,
                Events::Packet_received
            > 
        {
        public:
            explicit Connection_manager(unsigned short _port_number, unsigned _packet_queue_capacity = 32);

            void tick();
            void send_packet(
                Connection::Id _connection,
                std::shared_ptr<Protocol::Outbound_packet> _packet
            );

            ~Connection_manager();
            Connection_manager(Connection_manager const&) = delete;
            Connection_manager& operator=(Connection_manager const&) = delete;
            Connection_manager(Connection_manager&&) = delete;
            Connection_manager& operator=(Connection_manager&&) = delete;

        private:
            /* Max size of send and receive packet queues for connections */
            unsigned const packet_queue_capacity;

            boost::asio::io_context io_context; /* This is thread safe */
            std::jthread io_context_thread;
            void io_context_thread_function(std::stop_token _stop_token);

            Connection::Id next_connection_id;
            std::unordered_map<Connection::Id, std::shared_ptr<Connection>> connections;

            Acceptor acceptor;

            void process_accepted_sockets();
            void process_received_packets();
            void process_dead_connections();

        };
    }
}

#endif