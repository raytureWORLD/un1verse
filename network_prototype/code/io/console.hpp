#ifndef INCLUDED_IO_CONSOLE_HPP
#define INCLUDED_IO_CONSOLE_HPP

#include"text/formatting.hpp"

namespace Network {
    namespace Console {
        void write(auto const&... _arguments) {
            std::cout << Text::concatenate(_arguments...) << std::flush;
        }

        void write_line(auto const&... _arguments) {
            write(_arguments..., '\n');
        }
    }
}

#endif
