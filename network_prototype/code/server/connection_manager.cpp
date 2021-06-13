#include"server/connection_manager.hpp"
#include"io/console.hpp"
#include<functional>

Network::Server::Connection_manager::Connection_manager(uint16_t _port_number, unsigned _packet_queue_capacity):
    io_context_thread(
        std::bind_front(
            &Connection_manager::io_context_thread_function,
            this
        )
    ),
    /* This overload sets reuse_address by default */
    acceptor(
        io_context,
        boost::asio::ip::tcp::endpoint(
            boost::asio::ip::tcp::v4(),
            _port_number
        )
    ),
    next_connection_id(0),
    packet_queue_capacity(_packet_queue_capacity)
{
    acceptor.async_accept(std::bind_front(&Connection_manager::async_accept_callback, this));
}


void Network::Server::Connection_manager::dispatch_all_sync_events() {
    /* Copy to avoid holding the mutex for too long */
    std::vector<boost::asio::ip::tcp::socket> accepted_sockets_copy;
    
    {
        std::scoped_lock lock(accepted_sockets_mx);
        accepted_sockets_copy = std::move(accepted_sockets);
        accepted_sockets.clear();
    }

    for(auto& socket : accepted_sockets_copy) {
        auto [iterator, success] = connections.emplace(
            std::piecewise_construct,
            std::forward_as_tuple(next_connection_id),
            std::forward_as_tuple(std::move(socket), next_connection_id) 
        );

        auto const& connection = iterator->second;

        Events::Connection_established event(
            connection.id,
            connection.socket.local_endpoint().address(),
            connection.socket.local_endpoint().port(),
            connection.socket.remote_endpoint().address(),
            connection.socket.remote_endpoint().port()
        );

        post_event(event);
        
        ++next_connection_id;
    }
}

Network::Server::Connection_manager::~Connection_manager() {
    io_context.stop();
}

void Network::Server::Connection_manager::io_context_thread_function([[maybe_unused]] std::stop_token _stop_token) {
    auto work = boost::asio::require(io_context.get_executor(), boost::asio::execution::outstanding_work.tracked);

    /* Exceptions thrown from handlers will be caught here */
    /* When an exception is thrown, run() must be called again */
    for(;;) {
        try {
            io_context.run();
            if(io_context.stopped()) break;
        } catch(std::exception const&) {
            throw;
        }
    }
}

void Network::Server::Connection_manager::async_accept_callback(
    boost::system::error_code const& _error,
    boost::asio::ip::tcp::socket _peer_socket
) {
    if(!_error) {
        {
            std::scoped_lock lock(accepted_sockets_mx);
            accepted_sockets.emplace_back(std::move(_peer_socket));
        }
    } else {
        throw std::runtime_error(_error.message());
    }

    acceptor.async_accept(std::bind_front(&Connection_manager::async_accept_callback, this));
}
