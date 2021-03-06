#include"server/server.hpp"
#include"io/console.hpp"
#include"protocol/packet_ids.hpp"

Network::Server::Server(unsigned short _port):
    connection_manager(_port)
{ 
    connection_manager.add_event_callback(
        [this](Server_impl::Events::Connection_established& _event) -> void {
            on_player_join(_event);
        }
    );

    connection_manager.add_event_callback(
        [this](Server_impl::Events::Packet_received& _event) -> void {
            on_packet_received(_event);
        }
    );

    connection_manager.add_event_callback(
        [this](Server_impl::Events::Connection_killed& _event) -> void {
            on_connection_killed(_event);
        }
    );
}


void Network::Server::tick() {
    connection_manager.tick();
}


void Network::Server::on_player_join(Server_impl::Events::Connection_established& _event) {
    players.emplace(
        std::piecewise_construct,
        std::forward_as_tuple(_event.id),
        std::forward_as_tuple(
            _event.id
        )
    );

    Console::write_line(
        "Connection from ", _event.remote_endpoint_address, ":", _event.remote_endpoint_port,
        " accepted on ", _event.local_endpoint_address, ":", _event.local_endpoint_port,
        " with id ", _event.id
    );
}


void Network::Server::on_packet_received(Server_impl::Events::Packet_received& _event) {
    auto packet_reader = _event.packet.get_reader();

    switch(_event.packet.get_id()) {
        case Protocol::Packet_ids::debug_message: {
            std::string message;
            packet_reader.read_next(message);
            
            std::string message_reply = message + "-reply";

            auto reply_packet = std::make_shared<Protocol::Outbound_packet>(Protocol::Packet_ids::debug_message);
            reply_packet->write_next(message_reply);

            connection_manager.send_packet(
                _event.from,
                std::move(reply_packet)
            );

            Console::write_line(
                "Debug message received from ", _event.from, ": \"", message, "\", ",
                "replying with: \"", message_reply, "\""
            );
            break;
        }

        default: {
            Console::write_line(
                "Packet with invalid id (", _event.packet.get_id(), ") received from ", _event.from
            );

            break;
        }
    }
}


void Network::Server::on_connection_killed(Server_impl::Events::Connection_killed& _event) {
    players.erase(_event.id);

    Console::write_line(
        "Connection with id ", _event.id, " terminated, reason: ", _event.reason
    );
}