#ifndef _UTILS_H_
#define _UTILS_H_

#include<iostream>
#include<future>

#ifdef _MSC_VER
#define CPOUT std::wcout
#define CPERR std::wcerr
#define CPSTR std::wstring
#else
#define CPOUT std::cout
#define CPERR std::cerr
#define CPSTR std::string
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

namespace rena {

    typedef enum {
        MD5,
        SHA1,
        SHA256,
        SHA512
    } HASHMODE;

    typedef enum {
        CREATE,
        CHECK
    } HASHPURPOSE;

    bool confirm_interrupt( const CPSTR& msg , char y , char n );

}; // namespace rena

#endif