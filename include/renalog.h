#ifndef _RENALOG_H_
#define _RENALOG_H_

#include<iostream>
#include<string>
#include<fstream>
#include<filesystem>
#include<mutex>
#include<chrono>
#include<ctime>
#include<map>
#include<sstream>
#include<atomic>
#include<thread>
#if defined( WIN32 )
#include<Windows.h>
#include<DbgHelp.h>
#include<tchar.h>
#pragma comment( lib , "dbghelp.lib" )
#else
#include<execinfo.h>
#include<cxxabi.h>
#include<signal.h>
#include<unistd.h>
#if defined( __APPLE__ ) || defined( __clang__ )
#include<sys/ucontext.h>
#else
#include<ucontext.h>
#endif
#endif

#include"ThreadPool.h"
#include"rich.h"
#include"utils.h"

#ifdef DEBUG
#undef DEBUG
#endif
#ifdef INFO
#undef INFO
#endif
#ifdef WARNING
#undef WARNING
#endif
#ifdef ERROR
#undef ERROR
#endif
#ifdef FATAL
#undef FATAL
#endif
#ifdef UNKNOWN
#undef UNKNOWN
#endif

#define RENALOG_INIT( logdir , nametag , olfmaxnum , min_severity )                                                 \
    rena::__global_logger__ = new rena::renalog( logdir ,                                                           \
                                                 nametag ,                                                          \
                                                 olfmaxnum ,                                                        \
                                                 min_severity                                                       \
                                                );                                                                  \
    if ( rena::__global_logger__ -> init() != rena::renalog::RENALOGSTATUS::OK )                                    \
    {                                                                                                               \
        throw std::runtime_error( "Failed to init global logger" );                                                 \
    }                                                                                                               \
    rena::__global_logger__ -> dump_logline_begin( rena::renalog::RENALOGSEVERITY::INFO , "loghost" );              \
    *rena::__global_logger__ << "Logger inited and start." << "\n";

#define RENALOG_FREE()                                                                                              \
    delete rena::__global_logger__
#define LOG( severity , host , data )                                                                               \
    if ( rena::__global_logger__ -> is_severity_need_to_log( rena::renalog::RENALOGSEVERITY::severity ) )           \
    {                                                                                                               \
        auto tp = std::chrono::system_clock::now();                                                                 \
        rena::__global_logger__ -> push( [&,tp](){                                                                  \
            rena::__global_logger__ -> dump_logline_begin( rena::renalog::RENALOGSEVERITY::severity , #host , tp ); \
            *rena::__global_logger__ << data                                                                        \
                                     << " [" << std::filesystem::relative( __FILE__ , SOURCE_ROOT_DIR ).string()    \
                                     << ": " << __LINE__ << "]\n";                                                  \
        } );                                                                                                        \
    }
#define RENALOG_ISREADY() ( rena::__global_logger__ != nullptr )

namespace rena {

////////////////////////////////////////////////////////////
//                       renalog.cpp                      //
////////////////////////////////////////////////////////////

    class renalog {

        public:
            typedef enum {
                OK,
                INITERR,
            } RENALOGSTATUS;

            typedef enum {
                DEBUG,
                INFO,
                WARNING,
                ERROR,
                FATAL,
                UNKNOWN
            } RENALOGSEVERITY;

        public:
            inline renalog( std::filesystem::path logdir , CPSTR nametag , unsigned short old_log_file_max_num , RENALOGSEVERITY severity )
                : _logdir( logdir ) ,
                  _nametag( nametag ) ,
                  _old_log_file_max_num( old_log_file_max_num ) ,
                  _min_severity( severity ) ,
                  _tp( 1 ){};
            inline ~renalog(){
                this -> flush();
                // wait all log to be finished
                if ( this -> _rwF.is_open() )
                {
                    this -> dump_logline_begin( RENALOGSEVERITY::INFO , "loghost" );
                    this -> _rwF << "Logger stoped." << "\n";
                    this -> _rwF.close();
                }
            };
            RENALOGSTATUS init();
            void dump_logline_begin( RENALOGSEVERITY severity , std::string host );
            void dump_logline_begin( RENALOGSEVERITY severity , std::string host , const std::chrono::system_clock::time_point& tp );
            inline void flush(){
                while ( !( this -> _tp.is_terminated() ) ){ std::this_thread::sleep_for( std::chrono::milliseconds( 10 ) ); }
                this -> _rwF.flush();
                return;
            };
            
            template <class F , class... Args>
            void push( F&& f , Args&&... args ){
                this -> _tp.enqueue( f , args... );
                return;
            }

            template <typename T>
            friend inline renalog& operator<<( renalog& rg , const T& data );

            inline void lock(){ this -> _global_mutex.lock(); return; }
            inline void release(){ this -> _global_mutex.unlock(); return; }

            void set_logdir( const std::filesystem::path& logdir ) noexcept;
            void set_nametag( CPSTR nametag ) noexcept;
            void set_old_log_file_max_num( unsigned short old_log_file_max_num ) noexcept;
            void set_min_severity( RENALOGSEVERITY severity ) noexcept;
            bool is_severity_need_to_log( RENALOGSEVERITY severity ) const noexcept;


        private:
            std::filesystem::path _logdir;
            CPSTR _nametag = CPTEXT( "" );
            std::fstream _rwF;
            unsigned short _old_log_file_max_num = static_cast<unsigned short>( -1 );
            RENALOGSEVERITY _min_severity;
            std::mutex _global_mutex;
            ThreadPool _tp;

    }; // class renalog

    std::ostream& operator<<( std::ostream& os , renalog::RENALOGSEVERITY severity );

    template <typename T>
    inline renalog& operator<<( renalog& rg , const T& data ){
        rg._rwF << data;
        return rg;
    }

    renalog::RENALOGSEVERITY parse_str_to_severity( const std::string& str );

    extern renalog* __global_logger__;

////////////////////////////////////////////////////////////
//                     crashdumper.cpp                    //
////////////////////////////////////////////////////////////

/**
 * The implementation of stacktrace in class crash_dumper references the one in project g3log <https://github.com/KjellKod/g3log>
 * The files are:
 * * /src/crashhandler_windows.cpp
 * * /src/crashhandler_unix.cpp
 * * /src/stacktrace_windows.cpp
*/

    class crash_dumper {
        public:
            crash_dumper();
            ~crash_dumper();

            static bool _placeholder();

        private:
#ifdef WIN32
            LPTOP_LEVEL_EXCEPTION_FILTER m_OriFilter;
            static LONG WINAPI ExceptionFilter( LPEXCEPTION_POINTERS ExpInfo );
#else
            static void sigHandler( int signum , siginfo_t* info , void* ctx );
#endif
    };

    const bool __bplaceholder__ = crash_dumper::_placeholder();
    // call crash_dumper::_placeholder by hand here, make sure it will still work full-background even with static-linking

}; // namespace rena

#endif