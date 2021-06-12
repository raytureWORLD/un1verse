#ifndef INCLUDED_SERIALISATION_BINARY_IMPL_INTERNAL_HPP
#define INCLUDED_SERIALISATION_BINARY_IMPL_INTERNAL_HPP

#include<type_traits>
#include<cstddef>
#include<concepts>

namespace Network {
    namespace Serialisation {
        namespace Binary {
            namespace Concepts {
                template<typename _T>
                concept Serialisable_scalar = 
                    std::is_same<_T, typename std::decay<_T>::type>::value && 
                    std::is_scalar<_T>::value;
                

                template<typename _T>
                concept Serialisable_class = 
                    std::is_same<_T, typename std::decay<_T>::type>::value && 
                    std::is_class<_T>::value && (
                        requires(_T _t, _T const _tc, std::byte* _b, std::byte const* _cb) { 
                            { _tc.serialise(_b, _b) } -> std::same_as<std::size_t>; 
                            { _t.deserialise(_cb, _cb) } -> std::same_as<std::size_t>;
                        } ||
                        requires(_T _t, _T const _tc, std::byte* _b, std::byte const* _cb) { 
                            { serialise(_tc, _b, _b) } -> std::same_as<std::size_t>; 
                            { deserialise(_t, _cb, _cb) } -> std::same_as<std::size_t>;
                        }
                    );

            }
        }
    }
}

#endif