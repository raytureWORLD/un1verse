#include"server/connection_manager.hpp"
#include"io/console.hpp"
#include<functional>

Network::Server::Connection_manager::Connection_manager(uint16_t _port_number, unsigned _packet_queue_capacity):
    packet_queue_capacity(_packet_queue_capacity),
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
    next_connection_id(0)
{
    async_accept();
}

Network::Server::Connection_manager::~Connection_manager() {
    io_context.stop();
}


void Network::Server::Connection_manager::tick() {
    process_accepted_sockets();
    process_killed_connections();
    process_received_packets();
}

void Network::Server::Connection_manager::io_context_thread_function(std::stop_token _stop_token) {
    (void)_stop_token;

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

void Network::Server::Connection_manager::async_accept() {
    acceptor.async_accept(std::bind_front(&Connection_manager::async_accept_callback, this));
}

void Network::Server::Connection_manager::async_accept_callback(
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

    acceptor.async_accept(std::bind_front(&Connection_manager::async_accept_callback, this));
}


void Network::Server::Connection_manager::process_accepted_sockets() {
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
            std::forward_as_tuple(
                std::make_shared<Connection>(
                    next_connection_id,
                    std::move(socket)
                )
            )
        );
        ++next_connection_id;

        /* Initiate receiving packets */
        async_read(iterator->second, Async_read_operation_type::length);

        Events::Connection_established event(
            iterator->second->id,
            iterator->second->socket.local_endpoint().address(),
            iterator->second->socket.local_endpoint().port(),
            iterator->second->socket.remote_endpoint().address(),
            iterator->second->socket.remote_endpoint().port()
        );

        post_event(event);
    }
}

void Network::Server::Connection_manager::process_killed_connections() {
    std::vector<Events::Connection_killed> connection_killed_events_copy;
    {
        std::scoped_lock lock(connection_killed_events_mx);
        connection_killed_events_copy = std::move(connection_killed_events);
        connection_killed_events.clear();
    }

    for(auto& event : connection_killed_events_copy) {
        connections.erase(event.id);
        post_event(event);
    }
}

void Network::Server::Connection_manager::process_received_packets() {
    std::vector<Events::Packet_received> packet_received_events_copy;
    {
        std::scoped_lock lock(packet_received_events_mx);
        packet_received_events_copy = std::move(packet_received_events);
        packet_received_events.clear();
    }

    for(auto& event : packet_received_events_copy) {
        post_event(event);
    }
}

void Network::Server::Connection_manager::async_read(
    std::shared_ptr<Connection> const& _connection, 
    Async_read_operation_type _operation
) {
    switch(_operation){
        case Async_read_operation_type::length: {
            boost::asio::async_read(
                _connection->socket,
                boost::asio::buffer(
                    reinterpret_cast<std::byte*>(&_connection->next_inbound_packet_length),
                    sizeof(_connection->next_inbound_packet_length)
                ),
                std::bind_front(
                    &Connection_manager::async_read_callback,
                    this,
                    _connection,
                    Async_read_operation_type::length
                )
            );
            break;
        }

        case Async_read_operation_type::rest: {
            boost::asio::async_read(
                _connection->socket,
                boost::asio::buffer(
                    _connection->next_inbound_packet_data.get() + sizeof(_connection->next_inbound_packet_length),
                    _connection->next_inbound_packet_length - sizeof(_connection->next_inbound_packet_length)
                ),
                std::bind_front(
                    &Connection_manager::async_read_callback,
                    this,
                    _connection,
                    Async_read_operation_type::rest
                )
            );
            break;
        }
    }
}


void Network::Server::Connection_manager::async_read_callback(
    std::shared_ptr<Connection> _connection,
    Async_read_operation_type _operation, /* the operation that finished with this callback */
    boost::system::error_code const& _error,
    std::size_t _bytes_transferred
) {
    (void)_bytes_transferred;

    if(!_connection->is_dead) {
        if(!_error) {
            switch(_operation) {
                case Async_read_operation_type::length: {
                    _connection->next_inbound_packet_data = std::make_unique_for_overwrite<std::byte[]>(
                        _connection->next_inbound_packet_length
                    );

                    async_read(_connection, Async_read_operation_type::rest);

                    break;
                }

                case Async_read_operation_type::rest: {
                    {
                        std::scoped_lock lock(packet_received_events_mx);
                        packet_received_events.emplace_back(
                            _connection->id,
                            std::make_unique<Protocol::Inbound_packet const>(
                                std::move(_connection->next_inbound_packet_data),
                                _connection->next_inbound_packet_length
                            )
                        );
                    }

                    async_read(_connection, Async_read_operation_type::length);

                    break;
                }
            }
        } else {
            _connection->is_dead = true;
            _connection->socket.shutdown(boost::asio::ip::tcp::socket::shutdown_both);
            _connection->socket.close(); /* This cancels all outstanding async operations */

            {
                std::scoped_lock lock(connection_killed_events_mx);
                connection_killed_events.emplace_back(
                    _connection->id,
                    _error.message()
                );
            }
        }
    }
}
