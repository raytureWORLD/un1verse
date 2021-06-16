#include<iostream>
#include"io/console.hpp"
#include"server/server.hpp"


int main(int _argc, char** _argv) {
    using namespace Network;
    (void)_argc; (void)_argv;

    Console::write_line("Starting");
    
    Server server(2345);

    for(;;) {
        server.tick();
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }

    Console::write_line("Done");


}