#ifndef INCLUDED_SERVER_CONNECTION_MANAGER_HPP
#define INCLUDED_SERVER_CONNECTION_MANAGER_HPP

#include<boost/asio.hpp>
#include<thread>
#include<cstddef>
#include<queue>
#include<memory>
#include<vector>
#include<mutex>
#include"protocol/inbound_packet.hpp"
#include"protocol/outbound_packet.hpp"

namespace Network {
    namespace Server {
        class Connection_manager {
        public:
            explicit Connection_manager(uint16_t _port_number, unsigned _packet_queue_capacity = 32);

            ~Connection_manager();
            Connection_manager(Connection_manager const&) = delete;
            Connection_manager& operator=(Connection_manager const&) = delete;
            Connection_manager(Connection_manager&&) = delete;
            Connection_manager& operator=(Connection_manager&&) = delete;

        private:
            boost::asio::io_context io_context; /* This is thread safe */
            std::jthread io_context_thread;

            boost::asio::ip::tcp::acceptor acceptor;
            mutable std::mutex acceptor_mx; /* This is almost certainly not needed */

            std::vector<boost::asio::ip::tcp::socket> accepted_sockets;
            mutable std::mutex accepted_sockets_mx;

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