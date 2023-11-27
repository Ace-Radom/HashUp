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
#include<csignal>
#ifdef WIN32
#include<Windows.h>
#include<DbgHelp.h>
#include<process.h>
#include<tchar.h>
#define getpid _getpid
#pragma comment( lib , "dbghelp.lib" )
#pragma warning( push )
#pragma warning( disable: 4251 )
#else
#include<execinfo.h>
#include<cxxabi.h>
#include<unistd.h>
#if defined( __APPLE__ ) || defined( __clang__ )
#include<sys/ucontext.h>
#else
#include<ucontext.h>
#endif
#endif

#include"ThreadPool.h"
// #include"rich.h"
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

#ifdef WIN32
#   ifdef RENALOGAPI_EXPORT
#       define RENALOGAPI __declspec( dllexport )
#   else
#       define RENALOGAPI __declspec( dllimport )
#   endif
#else
#   define RENALOGAPI
#endif

#if defined( _MSC_VER ) && defined( RENALOG_USING_WINDOWS_FUNCSIG )
#define RENALOG_PRETTY_FUNCTION __FUNCSIG__
#elif defined( __GNUC__ ) && defined( RENALOG_USING_PRETTY_FUNCTION )
#define RENALOG_PRETTY_FUNCTION __PRETTY_FUNCTION__
#else 
#define RENALOG_PRETTY_FUNCTION __FUNCTION__
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
        rena::renalog_capture( rena::renalog::RENALOGSEVERITY::severity ,                                           \
                               #host , std::chrono::system_clock::now() ,                                           \
                               __FILE__ ,                                                                           \
                               __LINE__ ,                                                                           \
                               static_cast<const char*>( RENALOG_PRETTY_FUNCTION )\
                             ).stream() << data;                                                                    \
    }
#define RENALOG_ISREADY() ( ( rena::__global_logger__ != nullptr ) ? true : false )

namespace rena {

////////////////////////////////////////////////////////////
//                       renalog.cpp                      //
////////////////////////////////////////////////////////////

    class RENALOGAPI renalog {

        public:
            typedef enum {
                OK,
                INITERR,
                FCFIERR,  // fatal copy file init error
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
            inline ~renalog(){ this -> stop(); return; }
            RENALOGSTATUS init();
            void dump_logline_begin( RENALOGSEVERITY severity , std::string host );
            void dump_logline_begin( RENALOGSEVERITY severity , std::string host , const std::chrono::system_clock::time_point& tp );
            inline void flush(){
                while ( !( this -> _tp.is_terminated() ) ){ std::this_thread::sleep_for( std::chrono::milliseconds( 10 ) ); }
                this -> _rwF.flush();
                return;
            };
            inline void stop(){
                this -> flush();
                if ( this -> _rwF.is_open() )
                {
                    this -> dump_logline_begin( RENALOGSEVERITY::INFO , "loghost" );
                    this -> _rwF << "Logger stoped." << "\n";
                    this -> _rwF.close();
                }
            }
            RENALOGSTATUS copy_current_log_for_fatal();
            
            void push( rena::renalog::RENALOGSEVERITY severity , const char* host , std::chrono::system_clock::time_point log_tp , std::string msg );

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

    template <class _Elem , class _Traits>
    std::basic_ostream<_Elem,_Traits>& operator<<( std::basic_ostream<_Elem,_Traits>& os , renalog::RENALOGSEVERITY severity ){
        switch ( severity )
        {
            case renalog::RENALOGSEVERITY::DEBUG:   os << "DEBUG";   break;
            case renalog::RENALOGSEVERITY::INFO:    os << "INFO";    break;
            case renalog::RENALOGSEVERITY::WARNING: os << "WARNING"; break;
            case renalog::RENALOGSEVERITY::ERROR:   os << "ERROR";   break;
            case renalog::RENALOGSEVERITY::FATAL:   os << "FATAL";   break;
            case renalog::RENALOGSEVERITY::UNKNOWN: os << "UNKNOWN"; break;
        }
        return os;
    }

    template <typename T>
    inline renalog& operator<<( renalog& rg , const T& data ){
        rg._rwF << data;
        return rg;
    }

    RENALOGAPI
    renalog::RENALOGSEVERITY parse_str_to_severity( const std::string& str );

    RENALOGAPI
    extern renalog* __global_logger__;

////////////////////////////////////////////////////////////
//                     renalog_cap.cpp                    //
////////////////////////////////////////////////////////////

    class RENALOGAPI renalog_capture {
        public:
            renalog_capture( renalog::RENALOGSEVERITY severity ,
                             const char* host ,
                             std::chrono::system_clock::time_point log_tp ,
                             const char* file ,
                             size_t line ,
                             const char* function )
                : _oss() ,
                  _severity( severity ) ,
                  _host( host ) ,
                  _tp( log_tp ) ,
                  _file( file ) ,
                  _line( line ) ,
                  _function( function ){};
            ~renalog_capture();

            inline std::ostringstream& stream(){ return this -> _oss; };

        private:
            std::ostringstream _oss;
            renalog::RENALOGSEVERITY _severity;
            const char* _host;
            std::chrono::system_clock::time_point _tp;
            const char* _file;
            size_t _line;
            const char* _function;
    }; // class renalog_capture

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

#ifdef WIN32
    typedef DWORD EXPSIGID;
#else
    typedef int   EXPSIGID;
#endif

    class RENALOGAPI crash_dumper {
        public:
            crash_dumper(){};
            ~crash_dumper(){};
            void start();

        private:
#ifdef WIN32
            static LONG WINAPI ExceptionFilter( LPEXCEPTION_POINTERS ExpInfo );
#else
            static void sigHandler( int signum , siginfo_t* info , void* ctx );
#endif
            static std::string get_exception_name( EXPSIGID ExpID );
    };

    RENALOGAPI
    void start_global_crash_dumper();

}; // namespace rena

#pragma warning( pop )

#endif