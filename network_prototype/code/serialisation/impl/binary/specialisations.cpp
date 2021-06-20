#include"serialisation/impl/binary/specialisations.hpp"
#include"serialisation/binary.hpp"

std::size_t Network::Serialisation::Binary::read(
        std::string& _value, std::byte const* _memory_begin, std::byte const* _memory_end
) {
    //TODO: Add some asserts here, this is risky
    std::size_t offset = 0;

    uint32_t length;
    offset += read(length, _memory_begin, _memory_end);

    if(length > 1024) std::runtime_error("Remove this!!");

    /* char and std::byte have the same memory representation - strict aliasing is not violated */
    _value = std::string(reinterpret_cast<char const*>(_memory_begin + offset), length);

    return offset + length;
}


std::size_t Network::Serialisation::Binary::write(
        std::string const& _value, std::byte* _memory_begin, std::byte* _memory_end
) {
    /* if range is null, no serialisation is done, only size is returned */
    std::size_t offset = 0;
    offset += write(
        (uint32_t)_value.length(),
        _memory_begin, _memory_end
    );

    offset += write(
        std::span<std::byte const>(reinterpret_cast<std::byte const*>(_value.c_str()), _value.length()),
        _memory_begin, _memory_end
    );

    return offset;
}