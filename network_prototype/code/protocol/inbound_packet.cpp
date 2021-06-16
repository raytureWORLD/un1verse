#include"protocol/inbound_packet.hpp"


Network::Protocol::Inbound_packet_reader::Inbound_packet_reader(std::span<std::byte const> const& _data):
    packet_data(_data), current_offset(0)
{ }



Network::Protocol::Inbound_packet::Inbound_packet(std::unique_ptr<std::byte const[]>&& _data, std::size_t _data_length):
    data(std::move(_data)), data_length(_data_length)
{
    std::byte const* data_ptr = data.get();
    std::byte const* current_data_ptr = data.get();
    current_data_ptr += Serialisation::Binary::read(packet_length, current_data_ptr, data_ptr + data_length);
    current_data_ptr += Serialisation::Binary::read(packet_id, current_data_ptr, data_ptr + data_length);
}

Network::Protocol::Inbound_packet_reader Network::Protocol::Inbound_packet::get_reader() const {
    return {{
        data.get() + sizeof(Packet_length) + sizeof(Protocol::Packet_id),
        data_length - sizeof(Packet_length) + sizeof(Protocol::Packet_id)
    }};
}

Network::Protocol::Packet_length Network::Protocol::Inbound_packet::get_length() const noexcept {
    return packet_length;
}

Network::Protocol::Packet_id Network::Protocol::Inbound_packet::get_id() const noexcept {
    return packet_id;
}
