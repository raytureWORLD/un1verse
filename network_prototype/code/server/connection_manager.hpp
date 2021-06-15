#ifndef INCLUDED_SERVER_CONNECTION_MANAGER_HPP
#define INCLUDED_SERVER_CONNECTION_MANAGER_HPP

#include<thread>
#include<cstddef>
#include<queue>
#include<memory>
#include<vector>
#include<mutex>
#include<unordered_map>
#include<string>
#include"protocol/inbound_packet.hpp"
#include"protocol/outbound_packet.hpp"
#include"events/sync_event_emitter.hpp"
#include"server/events.hpp"
#include"server/acceptor.hpp"

namespace Network {
    namespace Server {
        class Connection_manager: 
            public Sync_event_emitter<
                Events::Connection_established,
                Events::Connection_killed,
                Events::Packet_received
            > 
        {
        public:
            explicit Connection_manager(unsigned short _port_number, unsigned _packet_queue_capacity = 32);

            /* This is called synchronously, from the main thread */
            void tick();

            ~Connection_manager();
            Connection_manager(Connection_manager const&) = delete;
            Connection_manager& operator=(Connection_manager const&) = delete;
            Connection_manager(Connection_manager&&) = delete;
            Connection_manager& operator=(Connection_manager&&) = delete;

        private:
            /* Max size of send and receive packet queues for connections */
            unsigned const packet_queue_capacity;

            boost::asio::io_context io_context; /* This is thread safe */
            std::jthread io_context_thread;
            void io_context_thread_function(std::stop_token _stop_token);

            Acceptor acceptor;
            void process_accepted_sockets();

            std::vector<Events::Connection_killed> connection_killed_events;
            mutable std::mutex connection_killed_events_mx;
            void process_killed_connections();

            std::vector<Events::Packet_received> packet_received_events;
            mutable std::mutex packet_received_events_mx;
            void process_received_packets();
            enum struct Async_read_operation_type { length, rest };
            /* This does not allocate anything, length must be valid before the call */
            void async_read(std::shared_ptr<Connection> const& _connection, Async_read_operation_type _operation);
            void async_read_callback(
                std::shared_ptr<Connection> _connection, /* copy */
                Async_read_operation_type _operation, /* the operation that finished with this callback */
                boost::system::error_code const& _error,
                std::size_t _bytes_transferred
            );

            std::unordered_map<Connection::Id, std::shared_ptr<Connection>> connections;
            Connection::Id next_connection_id;
        };
    }
}

#endif