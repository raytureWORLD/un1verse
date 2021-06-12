#include"protocol/inbound_packet.hpp"

Network::Protocol::Inbound_packet::Inbound_packet(std::unique_ptr<std::byte const[]>&& _data, std::size_t _data_length):
    data(std::move(_data)), data_length(_data_length)
{

}