#include"serialisation/impl/binary/specialisations.hpp"
#include"serialisation/binary.hpp"

std::size_t Network::Serialisation::Binary::read(
        std::string& _value, std::byte const* _memory_begin, std::byte const* _memory_end
) {
    //TODO: Add some asserts here, this is risky
    (void)_memory_end;

    /* char and std::byte have the same memory representation - strict aliasing is not violated */
    _value = std::string(reinterpret_cast<char const*>(_memory_begin));

    return _value.length() + 1;
}


std::size_t Network::Serialisation::Binary::write(
        std::string const& _value, std::byte* _memory_begin, std::byte* _memory_end
) {
    return write(
        std::span<std::byte const>(reinterpret_cast<std::byte const*>(_value.c_str()), _value.length() + 1),
        _memory_begin, _memory_end
    );
}