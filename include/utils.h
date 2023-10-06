#ifndef _UTILS_H_
#define _UTILS_H_

#include<iostream>
#include<future>

#ifdef NDEBUG
#define DEBUG_MSG( msg )                    \
        do {                                \
        } while ( 0 )
#else
#define DEBUG_MSG( msg )                    \
        do {                                \
            std::cout << msg << std::endl;  \
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

    bool confirm_interrupt( const std::string& msg , char y , char n );

}; // namespace rena

#endif