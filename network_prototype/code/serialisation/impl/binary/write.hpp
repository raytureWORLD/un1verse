#ifndef INCLUDED_SERIALISATION_BINARY_IMPL_WRITE_HPP
#define INCLUDED_SERIALISATION_BINARY_IMPL_WRITE_HPP

#include<type_traits>
#include<cstddef>
#include<cstring>
#include"text/type_name.hpp"
#include"debug/assert.hpp"
#include"serialisation/impl/binary/concepts.hpp"
#include"preprocessor/debug.hpp"
#include"text/formatting.hpp"
#include<span>


namespace Network {
    namespace Serialisation {
        namespace Binary {
            template<Concepts::Serialisable_scalar _Type>
            std::size_t write(_Type const& _value, std::byte* _memory_begin, std::byte* _memory_end) {
                if(_memory_begin && _memory_end) {
                    DEBUG(
                        Debug::runtime_assert(
                            _memory_end - _memory_begin >= (std::ptrdiff_t)sizeof(_Type), 
                            Text::concatenate(
                                "Serialisation::Binary::write(): invalid buffer for T = ", Text::type_name<_Type>(),
                                ", length = ", sizeof(_Type), ", buffer size = ", _memory_end - _memory_begin
                            )
                        );
                    )

                    std::memcpy(_memory_begin, &_value, sizeof(_Type));
                }

                return sizeof(_Type);
            }


            template<Concepts::Serialisable_scalar _Type>
            std::size_t write(std::span<_Type const> const& _array, std::byte* _memory_begin, std::byte* _memory_end) {
                if(_memory_begin && _memory_end) {
                    DEBUG(
                        Debug::runtime_assert(
                            _memory_end - _memory_begin >= (std::ptrdiff_t)_array.size_bytes(), 
                            Text::concatenate(
                                "Serialisation::Binary::write(): invalid buffer for array of T = ", Text::type_name<_Type>(), 
                                ", length = ", _array.size_bytes(), ", buffer size = ", _memory_end - _memory_begin
                            )
                        );
                    )

                    std::memcpy(_memory_begin, _array.data(), _array.size_bytes());
                }

                return _array.size_bytes();
            }

            template<Concepts::Serialisable_class _Type>
            std::size_t write(_Type const& _value, std::byte* _memory_begin, std::byte* _memory_end) {
                if constexpr(requires { _value.serialise(_memory_begin, _memory_end); }) {
                    return _value.serialise(_memory_begin, _memory_end);
                } else {
                    return serialise(_value, _memory_begin, _memory_end);
                }
            }
        }
    }
}


#endif