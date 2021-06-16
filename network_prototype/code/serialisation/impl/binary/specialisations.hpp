#ifndef INCLUDED_SERIALISATION_BINARY_SPECIALISATIONS_HPP
#define INCLUDED_SERIALISATION_BINARY_SPECIALISATIONS_HPP

#include<string>

namespace Network {
    namespace Serialisation {
        namespace Binary {
            std::size_t read(std::string& _value, std::byte const* _memory_begin, std::byte const* _memory_end);
            std::size_t write(std::string const& _value, std::byte* _memory_begin, std::byte* _memory_end);
        }
    }
}

#endif