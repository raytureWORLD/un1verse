#ifndef INCLUDED_DEBUG_RUNTIME_ASSERT_HPP
#define INCLUDED_DEBUG_RUNTIME_ASSERT_HPP

#include<string_view>
#include<source_location>

namespace Network {
    namespace Debug {
        void runtime_assert(
            bool _condition, 
            std::string_view const& _message = "",
            std::source_location const& _source_location = std::source_location::current()
        );
    }
}

#endif