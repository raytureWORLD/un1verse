#include<iostream>
#include"io/console.hpp"
#include"server/server.hpp"
#include"client/client.hpp"


int main(int _argc, char** _argv) {
    using namespace Network;
    (void)_argc; (void)_argv;

    if(_argc > 1) {
        /* run in client mode - this is only for testing and to be removed */

        Console::write_line("Initialising");
        Console::write_line("Running in client mode");

        std::string host = "127.0.0.1", service = "2345";        

        Client client;
        client.add_event_callback(
            [](Events::Connect_result& _event) {
                if(_event.success) {
                    Console::write_line("Connection established");
                } else {
                    Console::write_line("Connection could not be established: ", _event.error_message);
                }
            }
        ); 

        Console::write_line("Attempting to connect to ", host, ":", service);

        client.connect(host, service);

        for(;;) {
            client.tick();
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }

    } else {
        /* run in server mode */

        Console::write_line("Initialising");
        
        {
            unsigned short port = 2345;
            Server server(port);

            Console::write_line("Listening on 0.0.0.0:", port);

            for(;;) {
                server.tick();
                std::this_thread::sleep_for(std::chrono::milliseconds(500));
            }

            Console::write_line("Terminating");
        }

        Console::write_line("Exiting");
    }

}