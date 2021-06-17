#include<iostream>
#include"io/console.hpp"
#include"server/server.hpp"


int main(int _argc, char** _argv) {
    using namespace Network;
    (void)_argc; (void)_argv;

    if(_argc > 1) {
        /* run in client mode - this is only for testing and to be removed */

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