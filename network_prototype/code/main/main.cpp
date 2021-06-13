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

    for(;;) {
        Console::write_line("Tick");
        cm.dispatch_all_sync_events();
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
    Console::write_line("Done");


}