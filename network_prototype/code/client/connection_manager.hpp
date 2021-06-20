#ifndef INCLUDED_CLIENT_CONNECTION_MANAGER_HPP
#define INCLUDED_CLIENT_CONNECTION_MANAGER_HPP

#include"connection/connection.hpp"
#include"events/sync_event_emitter.hpp"
#include"client/events.hpp"
#include<thread>
#include<optional>

namespace Network {
    namespace Client_impl {
        class Connection_manager:
            public Sync_event_emitter<
                Events::Connect_result
            > 
        {
        public:
            explicit Connection_manager();

            void async_connect(std::string_view const& _host, std::string_view const& _service);

            void tick();

            ~Connection_manager();
            Connection_manager(Connection_manager const&) = delete;
            Connection_manager& operator=(Connection_manager const&) = delete;
            Connection_manager(Connection_manager&&) = delete;
            Connection_manager& operator=(Connection_manager&&) = delete;

        private:
            boost::asio::io_context io_context;
            std::jthread io_context_thread;
            void io_context_thread_function(std::stop_token _stop_token);

            boost::asio::ip::tcp::resolver resolver;
            mutable std::mutex connect_result_mx;
            std::optional<Events::Connect_result> connect_result;
            bool connect_in_progress;

            std::unique_ptr<Connection> connection;
        };
    }
}

#endif
