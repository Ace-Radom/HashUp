#ifndef _UTILS_H_
#define _UTILS_H_

#include<iostream>
#include<vector>
#include<cstring>
#include<chrono>
#include<ctime>
#include<sstream>
#ifdef _WIN32
#include<Windows.h>
#include<ShlObj_core.h>
#elif defined( __linux__ )
#include<unistd.h>
#include<termios.h>
#elif defined( __APPLE__ )
#include<unistd.h>
#include<termios.h>
#include<mach-o/dyld.h>
#endif
#include"rich.h"

#ifdef WIN32
#include<codecvt>
#define CPOUT  std::wcout
#define CPERR  std::wcerr
#define CPSTR  std::wstring
#define CPCHAR wchar_t
#define CPTEXT( str )       L##str
#define CPTOSTR( tostr )    std::to_wstring( tostr )
#define CPATOWCONV( str )   std::wstring_convert<std::codecvt_utf8<wchar_t>>{}.from_bytes( str )    // cp str to wstr convert
#define CPWTOACONV( str )   std::wstring_convert<std::codecvt_utf8<wchar_t>>{}.to_bytes( str )      // cp wstr to str convert
#define CPPATHTOSTR( path ) ( path ).wstring()                                                      // cp path to str
#else
#define CPOUT  std::cout
#define CPERR  std::cerr
#define CPSTR  std::string
#define CPCHAR char
#define CPTEXT( str )       ( str )
#define CPTOSTR( tostr )    std::to_string( tostr )
#define CPATOWCONV( str )   ( str )
#define CPWTOACONV( str )   ( str )
#define CPPATHTOSTR( path ) ( path ).string()
#endif

#ifdef NDEBUG
#define DEBUG_MSG( msg )                    \
        do {                                \
        } while ( 0 )
#else
#define DEBUG_MSG( msg )                    \
        do {                                \
            CPOUT << msg << std::endl;      \
        } while ( 0 )
#endif

#define in_range( x , min , max ) ( x >= min && x <= max )

namespace rena {

    typedef enum {
        MD5,
        SHA1,
        SHA224,
        SHA256,
        SHA384,
        SHA512,
#ifdef USE_OPENSSL_EVP
        SHA3_224,
        SHA3_256,
        SHA3_384,
        SHA3_512,
        SHAKE128,
        SHAKE256
#endif
    } HASHMODE;

    typedef enum {
        NOSET,
        CREATE,
        CHECK
    } HASHPURPOSE;

    bool confirm_interrupt( const CPSTR& msg , char y , char n );
    std::string get_time_str_now();

    void noecho();
    void echo();
    void nocursor();
    void showcursor();

    // utils for main function

    bool is_supported_hash_mode( std::string mode );
    CPSTR get_current_exe_path();
    CPSTR get_home_path();

    extern std::string    CFG_MODE;
    extern unsigned short CFG_THREAD;
    extern std::string    CFG_MIN_LOG_SEVERITY;
    extern unsigned short CFG_MAX_OLD_LOG_FILES;
    extern std::vector<std::string> support_hash_modes; // all supported hash modes

}; // namespace rena

#endif