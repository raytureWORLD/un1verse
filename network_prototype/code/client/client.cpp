#include"client/client.hpp"

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
}

void Network::Client::connect(std::string_view const& _host, std::string_view const& _service) {
    connection_manager.async_connect(_host, _service);
}


void Network::Client::tick() {
    connection_manager.tick();
}