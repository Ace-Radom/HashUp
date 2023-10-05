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
            std::cerr << msg << std::endl;  \
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

    template <typename _T>
    bool is_future_ready( const std::future<_T>& F );

}; // namespace rena

template <typename _T>
bool rena::is_future_ready( const std::future<_T>& F ){
    return F.wait_for( std::chrono::seconds( 0 ) ) == std::future_status::ready;
}

#endif