#ifndef INCLUDED_SERVER_CONNECTION_HPP
#define INCLUDED_SERVER_CONNECTION_HPP

#include"protocol/types.hpp"
#include"boost/asio.hpp"
#include<memory>
#include<string>
#include<deque>
#include<condition_variable>
#include<functional>
#include"protocol/inbound_packet.hpp"
#include"protocol/outbound_packet.hpp"

namespace Network {
    namespace Server_impl {
        class Connection {
        public:
            typedef unsigned Id;

            Id const id;

            boost::asio::ip::address const local_address;
            boost::asio::ip::port_type const local_port;
            boost::asio::ip::address const remote_address;
            boost::asio::ip::port_type const remote_port;


            explicit Connection(
                Id _id, boost::asio::ip::tcp::socket&& _socket, boost::asio::io_context& _socket_io_context
            );

            /* This does not introduce a data race with the io_context thread */
            std::vector<Protocol::Inbound_packet> get_and_clear_received_packets();
            void send_packet(std::shared_ptr<Protocol::Outbound_packet> _packet);

            ~Connection() = default;
            Connection(Connection const&) = delete;
            Connection& operator=(Connection const&) = delete;
            Connection(Connection&&) = delete;
            Connection& operator=(Connection&&) = delete;

        private:
            boost::asio::ip::tcp::socket socket;
            mutable std::mutex socket_mx;

            Protocol::Packet_length next_inbound_packet_length;
            bool next_inbound_packet_length_valid; /* Whether length has already been written */
            /* Full data, including all fields of the header - the length too */
            std::unique_ptr<std::byte[]> next_inbound_packet_data;

            std::vector<Protocol::Inbound_packet> received_packets;
            mutable std::mutex received_packets_mx;

            std::deque<std::shared_ptr<Protocol::Outbound_packet>> outbound_packets;
            mutable std::mutex outbound_packets_mx;

            void async_read();
            void async_read_callback(
                boost::system::error_code const& _error,
                std::size_t _bytes_transferred
            );

            void async_write();
            void async_write_callback(
                boost::system::error_code const& _error,
                std::size_t _bytes_transferred
            );

        };
    }
}

// socket.shutdown(boost::asio::ip::tcp::socket::shutdown_both);
// socket.close(); /* This cancels all outstanding async operations */


#endif
