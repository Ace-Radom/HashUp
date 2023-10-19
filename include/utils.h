#ifndef _UTILS_H_
#define _UTILS_H_

#include<iostream>
#include<future>

#ifdef _MSC_VER
#include<codecvt>
#define CPOUT std::wcout
#define CPERR std::wcerr
#define CPSTR std::wstring
#define CPATOWCONV( str )   std::wstring_convert<std::codecvt_utf8<wchar_t>>{}.from_bytes( str )    // cp str to wstr convert
#define CPWTOACONV( str )   std::wstring_convert<std::codecvt_utf8<wchar_t>>{}.to_bytes( str )      // cp wstr to str convert
#define CPPATHTOSTR( path ) ( path ).wstring()                                                      // cp path to str
#else
#define CPOUT std::cout
#define CPERR std::cerr
#define CPSTR std::string
#define CPATOWCONV( str ) ( str )
#define CPWTOACONV( str ) ( str )
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
#endif
    } HASHMODE;

    typedef enum {
        CREATE,
        CHECK
    } HASHPURPOSE;

    bool confirm_interrupt( const CPSTR& msg , char y , char n );

}; // namespace rena

#endif