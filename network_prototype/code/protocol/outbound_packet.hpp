#ifndef INCLUDED_PROTOCOL_OUTBOUND_PACKET_HPP
#define INCLUDED_PROTOCOL_OUTBOUND_PACKET_HPP 

#include"protocol/types.hpp"
#include"serialisation/binary.hpp"
#include<span>
#include<cstddef>
#include<memory>
#include<vector>

namespace Network {
    namespace Protocol {
        class Outbound_packet {
        public:
            explicit Outbound_packet(Packet_id _packet_id);

            /* This does not modify the header data */
            void write_next(auto const& _value) {
                std::size_t prev_data_size = data.size();

                data.resize(
                    prev_data_size + Serialisation::Binary::write(_value, nullptr, nullptr)
                );

                Serialisation::Binary::write(
                    _value, data.data() + prev_data_size, data.data() + data.size()
                );
                
                Serialisation::Binary::write(
                    (Packet_length)data.size(), data.data() + prev_data_size, data.data() + data.size()
                );
            }

            /* Full data, including the header */
            /* Any calls to write_next() invalidate this range */
            std::span<std::byte const> get_data() const;

            ~Outbound_packet() = default;
            Outbound_packet(Outbound_packet const&) = delete;
            Outbound_packet(Outbound_packet&&) = default;
            Outbound_packet& operator=(Outbound_packet const&) = delete;
            Outbound_packet& operator=(Outbound_packet&&) = default;

        private:
            /* Full data, including the header */
            std::vector<std::byte> data;

        };

    }
}

#endif
