#ifndef INCLUDED_SERVER_CONNECTION_MANAGER_HPP
#define INCLUDED_SERVER_CONNECTION_MANAGER_HPP

#include<boost/asio.hpp>
#include<thread>
#include<cstddef>
#include<queue>
#include<memory>
#include<vector>
#include<mutex>
#include<unordered_map>
#include"protocol/inbound_packet.hpp"
#include"protocol/outbound_packet.hpp"
#include"events/sync_event_emitter.hpp"

namespace Network {
    namespace Server {
        struct Connection {
            typedef unsigned Id;

            boost::asio::ip::tcp::socket socket;
            Id id;
        };

        /* TODO: This namespace should be directly inside Network */
        namespace Events {
            struct Connection_established {
                Connection::Id id;
                boost::asio::ip::address local_endpoint_address;
                boost::asio::ip::port_type local_endpoint_port;
                boost::asio::ip::address remote_endpoint_address;
                boost::asio::ip::port_type remote_endpoint_port;
            };
        };


        class Connection_manager: 
            public Sync_event_emitter<
                Events::Connection_established
            > 
        {
        public:
            explicit Connection_manager(uint16_t _port_number, unsigned _packet_queue_capacity = 32);

            /* This is called synchronously, from the main thread */
            void dispatch_all_sync_events();

            ~Connection_manager();
            Connection_manager(Connection_manager const&) = delete;
            Connection_manager& operator=(Connection_manager const&) = delete;
            Connection_manager(Connection_manager&&) = delete;
            Connection_manager& operator=(Connection_manager&&) = delete;

        private:
            boost::asio::io_context io_context; /* This is thread safe */
            std::jthread io_context_thread;

            boost::asio::ip::tcp::acceptor acceptor;

            /* These sockets should be synchronously moved into connections */
            std::vector<boost::asio::ip::tcp::socket> accepted_sockets;
            mutable std::mutex accepted_sockets_mx;

            std::unordered_map<Connection::Id, Connection> connections;
            Connection::Id next_connection_id;

            unsigned const packet_queue_capacity;
            std::queue<std::unique_ptr<Protocol::Inbound_packet>> inbound_packets_queue;
            std::queue<std::shared_ptr<Protocol::Outbound_packet>> outbound_packets_queue;

            void io_context_thread_function(std::stop_token _stop_token);

            void async_accept_callback(
                boost::system::error_code const& _error,
                boost::asio::ip::tcp::socket _peer_socket
            );

        };
    }
}

#endif