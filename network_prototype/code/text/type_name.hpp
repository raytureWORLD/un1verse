#ifndef INCLUDED_TEXT_TYPE_NAME_HPP
#define INCLUDED_TEXT_TYPE_NAME_HPP

#include<string>

namespace Network {
    namespace Text {
        template <typename T>
        constexpr std::string_view type_name() {
            std::string_view name, prefix, suffix;

            #if defined(__clang__)
                name = __PRETTY_FUNCTION__;
                prefix = "std::string_view Network::Text::type_name() [T = ";
                suffix = "]";
            #elif defined(__GNUC__)
                name = __PRETTY_FUNCTION__;
                prefix = "constexpr std::string_view Network::Text::type_name() [with T = ";
                suffix = "; std::string_view = std::basic_string_view<char>]";
            #elif defined(_MSC_VER)
                name = __FUNCSIG__;
                prefix = "class std::basic_string_view<char,struct std::char_traits<char> > __cdecl Network::Text::type_name<";
                suffix = ">(void)";
            #endif

            name.remove_prefix(prefix.size());
            name.remove_suffix(suffix.size());

            return name;
        }    
    }
}

#endif