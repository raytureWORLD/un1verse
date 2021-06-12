#include"debug/stack_trace.hpp"
#include<boost/stacktrace.hpp>

std::string Network::Debug::generate_stack_trace() {
    return boost::stacktrace::to_string(boost::stacktrace::stacktrace());
}