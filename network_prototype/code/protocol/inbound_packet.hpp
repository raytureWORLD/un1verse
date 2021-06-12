#ifndef INCLUDED_PROTOCOL_INBOUND_PACKET_HPP
#define INCLUDED_PROTOCOL_INBOUND_PACKET_HPP 

/*
    Every packet starts with a header consisting of the following fields:
        length (of the entire packet, including the header)
        id
*/


#include"protocol/types.hpp"
#include<span>
#include<cstddef>
#include<memory>

namespace Network {
    namespace Protocol {
        class Inbound_packet_reader {
        public:
            /* Packet data without the header */
            explicit Inbound_packet_reader(std::span<std::byte const> const& _data); 

            /* This does not include the header data */
            void read_next(auto& _value) {
                //TODO
            }

            ~Inbound_packet_reader() = default;
            Inbound_packet_reader(Inbound_packet_reader const&) = delete;
            Inbound_packet_reader(Inbound_packet_reader&&) = default;
            Inbound_packet_reader& operator=(Inbound_packet_reader const&) = delete;
            Inbound_packet_reader& operator=(Inbound_packet_reader&&) = default;

        private:
            std::span<std::byte> packet_data;
            std::size_t current_offset;

        };


        class Inbound_packet {
        public:
            /* Full packet data, including the header */
            explicit Inbound_packet(std::unique_ptr<std::byte const[]>&& _data, std::size_t _data_length);

            Inbound_packet_reader get_reader() const;
            Protocol::Packet_length get_length() const;
            Protocol::Packet_id get_id() const;

            ~Inbound_packet() = default;

            Inbound_packet(Inbound_packet const&) = delete;
            Inbound_packet(Inbound_packet&&) = default;
            Inbound_packet& operator=(Inbound_packet const&) = delete;
            Inbound_packet& operator=(Inbound_packet&&) = default;

        private:
            std::unique_ptr<std::byte const[]> data;
            std::size_t data_length;

            Protocol::Packet_length packet_length;
            Protocol::Packet_id packet_id;

        };

    }
}

#endif
