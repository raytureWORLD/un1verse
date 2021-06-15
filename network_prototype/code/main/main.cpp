#include<iostream>
#include"io/console.hpp"
#include"server/connection_manager.hpp"


int main(int _argc, char** _argv) {
    using namespace Network;
    (void)_argc; (void)_argv;


    Console::write_line("Running");
    Server::Connection_manager cm(2345);

    cm.add_event_callback(
        [](Server::Events::Connection_established& _event) -> void {
            Console::write_line(
                "Connection from ", _event.remote_endpoint_address, ":", _event.remote_endpoint_port,
                " accepted on ", _event.local_endpoint_address, ":", _event.local_endpoint_port,
                " with id ", _event.id
            );
        }
    );

    cm.add_event_callback(
        [](Server::Events::Connection_killed& _event) -> void {
            Console::write_line(
                "Connection with id ", _event.id, 
                " killed, reason: ", _event.reason
            );
        }
    );

    cm.add_event_callback(
        [](Server::Events::Packet_received& _event) -> void {
            Console::write_line(
                "Packet received from: ", _event.from, 
                ". Packet length: ", _event.packet.get_length(),
                ", Packet id: ", _event.packet.get_id()
            );
        }
    );

    for(;;) {
        cm.tick();
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
    Console::write_line("Done");


}