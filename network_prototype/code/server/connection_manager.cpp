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


void Network::Server::Connection_manager::tick() {
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

        auto& connection = iterator->second;

        /* Initiate receiving packets */
        boost::asio::async_read(
            connection.socket,
            boost::asio::buffer(
                reinterpret_cast<std::byte*>(&connection.next_inbound_packet_length),
                sizeof(connection.next_inbound_packet_length)
            ),
            std::bind_front(
                &Connection_manager::async_read_callback,
                this,
                connection.id,
                Async_read_operation_type::length
            )
        );

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


    std::vector<std::unique_ptr<Protocol::Inbound_packet>> inbound_packets_queue_copy;
    {
        std::scoped_lock lock(inbound_packets_queue_mx);
        inbound_packets_queue_copy = std::move(inbound_packets_queue);
        inbound_packets_queue.clear();
    }

    for(auto const& packet : inbound_packets_queue_copy) {
        Events::Packet_received event(
            0, //TODO: Change this
            packet.get()
        );

        post_event(event);
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


void Network::Server::Connection_manager::async_read_callback(
    Connection::Id _connection_id,
    Async_read_operation_type _operation_type,
    boost::system::error_code const& _error,
    [[maybe_unused]] std::size_t _bytes_transferred
) {
    if(!_error) {
        auto connection_iterator = connections.find(_connection_id);
        if(connection_iterator == std::end(connections)) {
            throw std::logic_error("Async_read: no connection with the specified id");
        }

        auto& connection = connection_iterator->second;

        switch(_operation_type) {
            case Async_read_operation_type::length: {
                connection.next_inbound_packet_data = std::make_unique_for_overwrite<std::byte[]>(
                    connection.next_inbound_packet_length
                );

                std::size_t offset = Serialisation::Binary::write(
                    connection.next_inbound_packet_length,
                    connection.next_inbound_packet_data.get(),
                    connection.next_inbound_packet_data.get() + connection.next_inbound_packet_length
                );

                boost::asio::async_read(
                    connection.socket,
                    boost::asio::buffer(
                        connection.next_inbound_packet_data.get() + offset,
                        connection.next_inbound_packet_length - offset
                    ),
                    std::bind_front(
                        &Connection_manager::async_read_callback,
                        this,
                        _connection_id,
                        Async_read_operation_type::rest
                    )
                );
                break;
            }

            case Async_read_operation_type::rest: {
                boost::asio::async_read(
                    connection.socket,
                    boost::asio::buffer(
                        reinterpret_cast<std::byte*>(&connection.next_inbound_packet_length),
                        sizeof(connection.next_inbound_packet_length)
                    ),
                    std::bind_front(
                        &Connection_manager::async_read_callback,
                        this,
                        _connection_id,
                        Async_read_operation_type::length
                    )
                );

                {
                    std::scoped_lock lock(inbound_packets_queue_mx);
                    inbound_packets_queue.emplace_back(
                        std::make_unique<Protocol::Inbound_packet>(
                            std::move(connection.next_inbound_packet_data),
                            connection.next_inbound_packet_length
                        )
                    );
                }

                break;
            }
        }
    } else {
        throw std::runtime_error(_error.message());
    }
}