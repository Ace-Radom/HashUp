#ifndef _RICH_H_
#define _RICH_H_

#pragma once

#include<iostream>
#include<bitset>

#ifdef _WIN32
#include<Windows.h>
#endif

namespace rena {

#ifdef _WIN32
    namespace win32u {
        void _get_console_screen_buffer_info(); 
        void _get_stdout_handle();
        extern CONSOLE_SCREEN_BUFFER_INFO _csbufinfo;
        extern HANDLE _WIN_STDOUT_HANDLE;
    } // namespace win32u
#endif

    namespace rich {

#define RC_ANSI   0
#define RC_WINAPI 1
#ifdef _WIN32
#   ifdef RICH_USE_ANSI_COLOR_ONLY
#       define RICH_COLOR_TYPE RC_ANSI
#   else
#       define RICH_COLOR_TYPE RC_WINAPI
#   endif
#else
#   define RICH_COLOR_TYPE RC_ANSI
#endif

#if RICH_COLOR_TYPE == RC_WINAPI
        extern WORD _original_color_attr;
        extern WORD _fgc; // foreground color
        extern WORD _bgc; // background color
#endif

        /**
         * @brief enum class FColor (Foreground Color)
        */
        enum class FColor : int {
            BLACK         = 30,
            RED           = 31,
            GREEN         = 32,
            YELLOW        = 33,
            BLUE          = 34,
            MAGENTA       = 35,
            CYAN          = 36,
            WHITE         = 37,
            GRAY          = 90,
            BRIGHTRED     = 91,
            BRIGHTGREEN   = 92,
            BRIGHTYELLOW  = 93,
            BRIGHTBLUE    = 94,
            BRIGHTMAGENTA = 95,
            BRIGHTCYAN    = 96,
            BRIGHTWHITE   = 97,
        };

        /**
         * @brief enum class BColor (Background Color)
        */
        enum class BColor : int {
            BLACK         = 40,
            RED           = 41,
            GREEN         = 42,
            YELLOW        = 43,
            BLUE          = 44,
            MAGENTA       = 45,
            CYAN          = 46,
            WHITE         = 47,
            GRAY          = 100,
            BRIGHTRED     = 101,
            BRIGHTGREEN   = 102,
            BRIGHTYELLOW  = 103,
            BRIGHTBLUE    = 104,
            BRIGHTMAGENTA = 105,
            BRIGHTCYAN    = 106,
            BRIGHTWHITE   = 107,
        };

        /**
         * @brief push one fontground color to ostream
        */
        template <class _Elem , class _Traits>
        std::basic_ostream<_Elem,_Traits>& operator<<( std::basic_ostream<_Elem,_Traits>& __os , FColor __fc );

        /**
         * @brief push one background color to otream
        */
        template <class _Elem , class _Traits>
        std::basic_ostream<_Elem,_Traits>& operator<<( std::basic_ostream<_Elem,_Traits>& __os , BColor __bc );

        /**
         * @brief reset all color style to default
         * 
         * @note Under Windows (and using WIN API) it will reset console text attr to the original attr got when calling renash_toolkit_global_init.
         *       Under Linux (or using ANSI under Windows) it pushes '\033[0m' to ostream.
        */
        template <class _Elem , class _Traits>
        std::basic_ostream<_Elem,_Traits>& style_reset( std::basic_ostream<_Elem,_Traits>& __os );

        void rich_global_init();

    }; // namespace rich

}; // namespace rena

template <class _Elem , class _Traits>
std::basic_ostream<_Elem,_Traits>& rena::rich::operator<<( std::basic_ostream<_Elem,_Traits>& __os , rena::rich::FColor __fc ){

#if RICH_COLOR_TYPE == RC_WINAPI

    // when using win api to set color, foreground text style will not be available

    switch ( __fc ) {
        case FColor::BLACK:         _fgc = 0x0000; break;
        case FColor::RED:           _fgc = 0x0004; break;
        case FColor::GREEN:         _fgc = 0x0002; break;
        case FColor::YELLOW:        _fgc = 0x0006; break;
        case FColor::BLUE:          _fgc = 0x0001; break;
        case FColor::MAGENTA:       _fgc = 0x0005; break;
        case FColor::CYAN:          _fgc = 0x0003; break;
        case FColor::WHITE:         _fgc = 0x0007; break;
        case FColor::GRAY:          _fgc = 0x0008; break;
        case FColor::BRIGHTRED:     _fgc = 0x000C; break;
        case FColor::BRIGHTGREEN:   _fgc = 0x000A; break;
        case FColor::BRIGHTYELLOW:  _fgc = 0x000E; break;
        case FColor::BRIGHTBLUE:    _fgc = 0x0009; break;
        case FColor::BRIGHTMAGENTA: _fgc = 0x000D; break;
        case FColor::BRIGHTCYAN:    _fgc = 0x000B; break;
        case FColor::BRIGHTWHITE:   _fgc = 0x000F; break;
        default: return __os;
    };

    SetConsoleTextAttribute( win32u::_WIN_STDOUT_HANDLE , _fgc | _bgc );

#else // using WIN API

    unsigned int chandle = static_cast<unsigned int>( __fc );
    unsigned int cpart = std::bitset<8>( chandle ).to_ulong(); // color part
    __os << "\033[" << cpart << "m" << std::flush;

#endif // using ANSI

    return __os;
}

template <class _Elem , class _Traits>
std::basic_ostream<_Elem,_Traits>& rena::rich::operator<<( std::basic_ostream<_Elem,_Traits>& __os , BColor __bc ){

#if RICH_COLOR_TYPE == RC_WINAPI

    switch ( __bc ) {
        case BColor::BLACK:         _bgc = 0x0000; break;
        case BColor::RED:           _bgc = 0x0040; break;
        case BColor::GREEN:         _bgc = 0x0020; break;
        case BColor::YELLOW:        _bgc = 0x0060; break;
        case BColor::BLUE:          _bgc = 0x0010; break;
        case BColor::MAGENTA:       _bgc = 0x0050; break;
        case BColor::CYAN:          _bgc = 0x0030; break;
        case BColor::WHITE:         _bgc = 0x0070; break;
        case BColor::GRAY:          _bgc = 0x0080; break;
        case BColor::BRIGHTRED:     _bgc = 0x00C0; break;
        case BColor::BRIGHTGREEN:   _bgc = 0x00A0; break;
        case BColor::BRIGHTYELLOW:  _bgc = 0x00E0; break;
        case BColor::BRIGHTBLUE:    _bgc = 0x0090; break;
        case BColor::BRIGHTMAGENTA: _bgc = 0x00D0; break;
        case BColor::BRIGHTCYAN:    _bgc = 0x00B0; break;
        case BColor::BRIGHTWHITE:   _bgc = 0x00F0; break;
        default: return __os;
    };

    SetConsoleTextAttribute( win32u::_WIN_STDOUT_HANDLE , _fgc | _bgc );

#else // using WIN API

    unsigned int chandle = static_cast<unsigned int>( __bc );
    unsigned int cpart = std::bitset<8>( chandle ).to_ulong(); // color part
    __os << "\033[" << cpart << "m" << std::flush;

#endif // using ANSI

    return __os;
}

template <class _Elem , class _Traits>
std::basic_ostream<_Elem,_Traits>& rena::rich::style_reset( std::basic_ostream<_Elem,_Traits>& __os ){

#if RICH_COLOR_TYPE == RC_WINAPI
    _fgc = 0;
    _bgc = 0;
    SetConsoleTextAttribute( win32u::_WIN_STDOUT_HANDLE , _original_color_attr );
#else // using WIN API
    __os << "\033[0m" << std::flush;
#endif // using ANSI

    return __os;
}

#endif