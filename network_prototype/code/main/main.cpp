#include<iostream>
#include"io/console.hpp"
#include"server/connection_manager.hpp"

int main(int _argc, char** _argv) {
    using namespace Network;
    (void)_argc; (void)_argv;


    Console::write_line("Running");
    for(;;) {
        Server::Connection_manager cm(2345);

        int x;
        std::cin >> x;
    }
    Console::write_line("Done");


}