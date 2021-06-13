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
            Id const id;
            Protocol::Packet_length next_inbound_packet_length;
            /* Full data, including all fields of the header - the length too */
            std::unique_ptr<std::byte[]> next_inbound_packet_data;

        };

        /* TODO: This namespace should be directly inside Network */
        namespace Events {
            struct Connection_established {
                Connection::Id const id;
                boost::asio::ip::address const local_endpoint_address;
                boost::asio::ip::port_type const local_endpoint_port;
                boost::asio::ip::address const remote_endpoint_address;
                boost::asio::ip::port_type const remote_endpoint_port;
            };

            struct Packet_received {
                Connection::Id const from;
                Protocol::Inbound_packet const* const packet;
            };
        };


        class Connection_manager: 
            public Sync_event_emitter<
                Events::Connection_established,
                Events::Packet_received
            > 
        {
        public:
            explicit Connection_manager(uint16_t _port_number, unsigned _packet_queue_capacity = 32);

            /* This is called synchronously, from the main thread */
            void tick();

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
            std::vector<std::unique_ptr<Protocol::Inbound_packet>> inbound_packets_queue;
            mutable std::mutex inbound_packets_queue_mx;
            std::vector<std::shared_ptr<Protocol::Outbound_packet>> outbound_packets_queue;
            mutable std::mutex outbound_packets_queue_mx;

            void io_context_thread_function(std::stop_token _stop_token);

            void async_accept_callback(
                boost::system::error_code const& _error,
                boost::asio::ip::tcp::socket _peer_socket
            );

            enum struct Async_read_operation_type { length, rest };
            void async_read_callback(
                Connection::Id _connection_id,
                Async_read_operation_type _operation_type,
                boost::system::error_code const& _error,
                std::size_t _bytes_transferred
            );

        };
    }
}

#endif