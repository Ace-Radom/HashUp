#include"rich.h"

#ifdef _WIN32

CONSOLE_SCREEN_BUFFER_INFO rena::win32u::_csbufinfo;
HANDLE rena::win32u::_WIN_STDOUT_HANDLE = nullptr;

void rena::win32u::_get_console_screen_buffer_info(){
    GetConsoleScreenBufferInfo( _WIN_STDOUT_HANDLE , &_csbufinfo );
    return;
}

void rena::win32u::_get_stdout_handle(){
    _WIN_STDOUT_HANDLE = GetStdHandle( STD_OUTPUT_HANDLE );
    return;
}

#endif // _WIN32

#if RICH_COLOR_TYPE == RC_WINAPI
    WORD rena::rich::_fgc = 0;
    WORD rena::rich::_bgc = 0;
    WORD rena::rich::_original_color_attr = 0;
#endif

void rena::rich::rich_global_init(){
#ifdef _WIN32
    win32u::_get_stdout_handle();
    win32u::_get_console_screen_buffer_info();
#if RICH_COLOR_TYPE == RC_WINAPI
    rich::_original_color_attr = win32u::_csbufinfo.wAttributes;
#endif
#endif
    return;
}
