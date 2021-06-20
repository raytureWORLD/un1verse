#ifndef INCLUDED_CLIENT_CLIENT_HPP
#define INCLUDED_CLIENT_CLIENT_HPP

#include"connection/connection.hpp"
#include"client/connection_manager.hpp"
#include"client/user_events.hpp"
#include<future>
#include<functional>
#include<string_view>


namespace Network {
    class Client: 
        public Sync_event_emitter<
            Events::Connection_lost,
            Events::Connect_result,
            Events::Auth_result
        > 
    {
    public:
        explicit Client();

        void tick();

        void connect(std::string_view const& _host, std::string_view const& _service);
        void authenticate(std::string_view const& _username);

        ~Client() = default;
        Client(Client const&) = delete;
        Client& operator=(Client const&) = delete;
        Client(Client&&) = delete;
        Client& operator=(Client&&) = delete;

    private:
        Client_impl::Connection_manager connection_manager;

    };
}

#endif