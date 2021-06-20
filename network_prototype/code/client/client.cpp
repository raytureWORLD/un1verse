#include"client/client.hpp"
#include"protocol/packet_ids.hpp"
#include"io/console.hpp"

Network::Client::Client() {
    connection_manager.add_event_callback(
        [this](Client_impl::Events::Connect_result& _event) -> void {
            post_event(
                Events::Connect_result(
                    _event.success,
                    std::move(_event.error_message)
                )
            );
        }
    );

    connection_manager.add_event_callback(
        [this](Client_impl::Events::Packet_received& _event) -> void {
            process_received_packet(_event.packet);
        }
    );
}

void Network::Client::connect(std::string_view const& _host, std::string_view const& _service) {
    connection_manager.async_connect(_host, _service);
}


void Network::Client::tick() {
    connection_manager.tick();
}


void Network::Client::process_received_packet(Protocol::Inbound_packet& _packet) {
    auto id = _packet.get_id();
    auto packet_reader = _packet.get_reader();

    switch(id) {
        case Protocol::Packet_ids::debug_message: {
            std::string message;
            packet_reader.read_next(message);

            Console::write_line("Debug message from the server: ", message);
            
            break;
        }

        case Protocol::Packet_ids::connection_state_change: {
            uint8_t status;
            Connection::Id connection_id;
            packet_reader.read_next(status);
            packet_reader.read_next(connection_id);

            Events::Player_status_change event(
                connection_id,
                status == 0 ? Events::Player_status_change::Status::connected :
                status == 1 ? Events::Player_status_change::Status::disconnected :
                throw std::logic_error("Invalid player status")
            );

            post_event(event);

            break;
        }
    }
}