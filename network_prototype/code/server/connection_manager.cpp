#include"server/connection_manager.hpp"
#include"io/console.hpp"
#include<functional>

Network::Server_impl::Connection_manager::Connection_manager(unsigned short _port_number, unsigned _packet_queue_capacity):
    packet_queue_capacity(_packet_queue_capacity),
    io_context_thread(
        std::bind_front(
            &Connection_manager::io_context_thread_function,
            this
        )
    ),
    next_connection_id(0),
    acceptor(io_context, _port_number)
{ }


Network::Server_impl::Connection_manager::~Connection_manager() {
    io_context.stop();
}


void Network::Server_impl::Connection_manager::tick() {
    process_dead_connections();
    process_accepted_sockets();
    process_received_packets();
}

void Network::Server_impl::Connection_manager::send_packet(
    Connection::Id _connection,
    std::shared_ptr<Protocol::Outbound_packet> _packet
) {
    auto connection = connections.find(_connection);
    if(connection != std::end(connections)) {
        connection->second->send_packet(std::move(_packet));
    } else {
        throw std::runtime_error(Text::concatenate(
            "Network::Server_impl::Connection_manager::send_packet() error: ",
            "no connection with id: ", _connection
        ));
    }
}

void Network::Server_impl::Connection_manager::io_context_thread_function(std::stop_token _stop_token) {
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


void Network::Server_impl::Connection_manager::process_accepted_sockets() {
    auto accepted_sockets = acceptor.get_and_clear_accepted_sockets();

    for(auto& socket : accepted_sockets) {
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

        Events::Connection_established event(
            iterator->second->id,
            iterator->second->local_address,
            iterator->second->local_port,
            iterator->second->remote_address,
            iterator->second->remote_port
        );

        post_event(event);
    }
}


void Network::Server_impl::Connection_manager::process_received_packets() {
    /* This is extremely slow... But good enough for now */

    for(auto& [id, connection] : connections) {
        auto packets = connection->get_and_clear_received_packets();
        for(auto& packet : packets) {
            Events::Packet_received event(id, std::move(packet));
            post_event(event);
        }
    }
}


void Network::Server_impl::Connection_manager::process_dead_connections() {
    /* Extremely slow... */
    for(auto it = std::begin(connections); it != std::end(connections);) {
        auto& [id, connection] = *it;
        if(connection->is_dead()) {
            Events::Connection_killed event(
                id,
                connection->get_dead_reason()
            );

            post_event(event);

            it = connections.erase(it);
        } else {
            ++it;
        }
    }
}