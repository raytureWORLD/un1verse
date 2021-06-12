#ifndef INCLUDED_PREPROCESSOR_DEBUG_HPP
#define INCLUDED_PREPROCESSOR_DEBUG_HPP

#define BUILD_DEBUG

#if defined BUILD_DEBUG
    #define DEBUG(...) __VA_ARGS__
    #define ELSE(...)
#else
    #define DEBUG(...)
    #define ELSE(...) __VA_ARGS__
#endif


#endif
