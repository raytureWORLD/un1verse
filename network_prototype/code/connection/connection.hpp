#ifndef INCLUDED_SERVER_CONNECTION_HPP
#define INCLUDED_SERVER_CONNECTION_HPP

#include"protocol/types.hpp"
#include"boost/asio.hpp"
#include<memory>
#include<string>
#include<deque>
#include<atomic>
#include<functional>
#include"protocol/inbound_packet.hpp"
#include"protocol/outbound_packet.hpp"

namespace Network {
    class Connection: public std::enable_shared_from_this<Connection> {
    public:
        typedef unsigned Id;

        Id const id;

        boost::asio::ip::address const local_address;
        boost::asio::ip::port_type const local_port;
        boost::asio::ip::address const remote_address;
        boost::asio::ip::port_type const remote_port;

        explicit Connection(Id _id, boost::asio::ip::tcp::socket&& _socket);

        /* These functions do not introduce a data race with the io_context thread */
        std::vector<Protocol::Inbound_packet> get_and_clear_received_packets();
        void send_packet(std::shared_ptr<Protocol::Outbound_packet> _packet);

        bool is_dead() const;
        /* This prevents move-semantics, but whatever */
        /* Must only be called after is_dead() returns true, otherwise race condition */
        std::string const& get_dead_reason() const;

        ~Connection();
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

        std::once_flag init_async_read_once_flag;

        std::deque<std::shared_ptr<Protocol::Outbound_packet>> outbound_packets;
        mutable std::mutex outbound_packets_mx;

        std::atomic<bool> is_dead_;
        /* This is protected by is_dead_ if is_dead_ is written to second */
        std::string dead_reason; 

        void async_read();
        void async_read_callback(
            std::shared_ptr<Connection>,
            boost::system::error_code const& _error,
            std::size_t _bytes_transferred
        );

        void async_write();
        void async_write_callback(
            std::shared_ptr<Connection>,
            boost::system::error_code const& _error,
            std::size_t _bytes_transferred
        );

    };
}



#endif
