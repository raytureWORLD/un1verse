#ifndef INCLUDED_SERVER_ACCEPTOR_HPP
#define INCLUDED_SERVER_ACCEPTOR_HPP

#include"boost/asio.hpp"
#include<vector>
#include<mutex>

namespace Network {
    namespace Server {
        class Acceptor {
        public:
            explicit Acceptor(boost::asio::io_context& _io_context, unsigned short _port);

            /* This does not introduce a data race with the io_context thread */
            std::vector<boost::asio::ip::tcp::socket> get_and_clear_accepted_sockets();

            ~Acceptor() = default;
            Acceptor(Acceptor const&) = delete;
            Acceptor& operator=(Acceptor const&) = delete;
            Acceptor(Acceptor&&) = delete;
            Acceptor& operator=(Acceptor&&) = delete;

        private:
            boost::asio::ip::tcp::acceptor acceptor;

            std::vector<boost::asio::ip::tcp::socket> accepted_sockets;
            mutable std::mutex accepted_sockets_mx;

            void async_accept();
            void async_accept_callback(
                boost::system::error_code const& _error,
                boost::asio::ip::tcp::socket _peer_socket
            );

        };
    }
}

#endif