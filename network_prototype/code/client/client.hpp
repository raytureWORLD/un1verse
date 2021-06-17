#ifndef INCLUDED_CLIENT_CLIENT_HPP
#define INCLUDED_CLIENT_CLIENT_HPP

#include"connection/connection.hpp"
#include"client/connection_manager.hpp"
#include<future>
#include<functional>
#include<string_view>


namespace Network {
    class Client {
    public:
        explicit Client() = default;


        // void tick();

        ~Client();
        Client(Client const&) = delete;
        Client& operator=(Client const&) = delete;
        Client(Client&&) = delete;
        Client& operator=(Client&&) = delete;

    private:
        Client_impl::Connection_manager connection_manager;

    };
}

#endif