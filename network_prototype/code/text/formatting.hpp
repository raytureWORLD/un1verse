#ifndef INCLUDED_TEXT_FORMATTING_HPP
#define INCLUDED_TEXT_FORMATTING_HPP

#include<sstream>
#include<string>

namespace Network {
    namespace Text {
        /* This is slow but good enough for now, we can use <charconv> later */
        std::string to_text(auto&& _argument) {
            std::stringstream result;
            return (result << _argument), result.str();
        }

        template<typename... _Args>
        std::string concatenate(_Args const&... _arguments) {
            if constexpr(sizeof...(_Args) == 0) return "";
            else return (... + to_text(_arguments));
        }
    }
}


#endif