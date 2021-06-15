#include"server/acceptor.hpp"
#include"text/formatting.hpp"

Network::Server::Acceptor::Acceptor(boost::asio::io_context& _io_context, unsigned short _port):
    /* This overload sets reuse_address by default */
    acceptor(_io_context, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), _port))
{ 
    async_accept();
} 


std::vector<boost::asio::ip::tcp::socket> Network::Server::Acceptor::get_and_clear_accepted_sockets() {
    decltype(accepted_sockets) result;

    {
        std::scoped_lock lock(accepted_sockets_mx);

        result = std::move(accepted_sockets);
        accepted_sockets.clear();
    }

    return result;
}


void Network::Server::Acceptor::async_accept() {
    acceptor.async_accept(std::bind_front(&Acceptor::async_accept_callback, this));
}


void Network::Server::Acceptor::async_accept_callback(
    boost::system::error_code const& _error,
    boost::asio::ip::tcp::socket _peer_socket
) {
    if(!_error) {
        std::scoped_lock lock(accepted_sockets_mx);
        accepted_sockets.emplace_back(std::move(_peer_socket));
    } else {
        throw std::runtime_error(
            Text::concatenate(
                "Network::Server::Connection_manager::async_accept_callback() error: ",
                _error.message()
            )
        );
    }

    async_accept();
}

