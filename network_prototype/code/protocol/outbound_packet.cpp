#include"protocol/outbound_packet.hpp"

Network::Protocol::Outbound_packet::Outbound_packet(Packet_id _packet_id) {
    write_next((Packet_length)0); /* placeholder */
    write_next(_packet_id);
}

std::span<std::byte const> Network::Protocol::Outbound_packet::get_data() const {
    return { data.data(), data.size() };
}
