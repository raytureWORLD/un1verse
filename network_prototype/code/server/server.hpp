#ifndef INCLUDED_SERVER_SERVER_HPP
#define INCLUDED_SERVER_SERVER_HPP

#include"server/connection_manager.hpp"

namespace Network {
    class Server {
    public:
        explicit Server(unsigned short _port);

        void tick();

        ~Server() = default;

        Server(Server const&) = delete;
        Server& operator=(Server const&) = delete;
        Server(Server&&) = delete;
        Server& operator=(Server&&) = delete;

    private:
        struct Player {
            Server_impl::Connection::Id connection_id;
        };

        Server_impl::Connection_manager connection_manager;
        std::unordered_map<Server_impl::Connection::Id, Player> players;

        void on_player_join(Server_impl::Events::Connection_established& _event);
        void on_packet_received(Server_impl::Events::Packet_received& _event);

    };
}

#endif