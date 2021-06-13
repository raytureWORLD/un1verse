#include<iostream>
#include"io/console.hpp"
#include"server/connection_manager.hpp"

int main(int _argc, char** _argv) {
    using namespace Network;
    (void)_argc; (void)_argv;


    Console::write_line("Running");
    Server::Connection_manager cm(2345);
    int x;
    std::cin >> x; /* This is just for testing, so the program won't exit immediately */
    Console::write_line("Done");


}