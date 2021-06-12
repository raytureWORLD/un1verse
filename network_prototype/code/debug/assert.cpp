#include"debug/assert.hpp"
#include"text/formatting.hpp"
#include"debug/stack_trace.hpp"

void Network::Debug::runtime_assert(
    bool _condition, 
    std::string_view const& _message,
    std::source_location const& _source_location
) {
    if(!_condition) {
        throw std::logic_error(
            Text::concatenate(
                "Assertion failed", _message.empty() ? "" : ": ", _message, "\n",
                "Location: ", _source_location.file_name(), ":", _source_location.line(), "\n",
                "Stack trace:\n", generate_stack_trace()
            )
        );        
    }
}


