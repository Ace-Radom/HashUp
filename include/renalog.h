#ifndef _RENALOG_H_
#define _RENALOG_H_

#include<iostream>
#include<string>
#include<fstream>
#include<filesystem>
#include<mutex>
#include<chrono>
#include<ctime>

#include"rich.h"
#include"utils.h"

#define RENALOG_INIT( logdir , nametag , olfmaxnum , min_severity )                                         \
    rena::__global_logger__ = new rena::renalog( logdir ,                                                   \
                                                 nametag ,                                                  \
                                                 olfmaxnum ,                                                \
                                                 rena::renalog::RENALOGSEVERITY::min_severity               \
                                                );                                                          \
    rena::__global_logger__ -> init();
#define RENALOG_FREE()                                                                                      \
    delete rena::__global_logger__
#define LOG( severity , host , data )                                                                       \
    if ( rena::__global_logger__ -> is_severity_need_to_log( rena::renalog::RENALOGSEVERITY::severity ) )   \
    {                                                                                                       \
        rena::__global_logger__ -> lock();                                                                  \
        rena::__global_logger__ -> dump_logline_begin( rena::renalog::RENALOGSEVERITY::severity , #host );  \
        *rena::__global_logger__ << data << "\n";                                                           \
        rena::__global_logger__ -> release();                                                               \
    }


namespace rena {

    class renalog {

        public:
            typedef enum {
                OK,
                INITERR,
            } RENALOGSTATUS;

            typedef enum {
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
                  _min_severity( severity ){};
            inline ~renalog(){};
            RENALOGSTATUS init();
            void dump_logline_begin( RENALOGSEVERITY severity , std::string host );

            template <typename T>
            friend inline renalog& operator<<( renalog& rg , const T& data );

            inline void lock(){
                this -> _global_mutex.lock();
                return;
            }
            inline void release(){
                this -> _global_mutex.unlock();
                return;
            }

            inline void set_logdir( std::filesystem::path logdir) noexcept {
                this -> _logdir = logdir;
                return;
            }
            inline void set_nametag( CPSTR nametag ) noexcept {
                this -> _nametag = nametag;
                return;
            }
            inline void set_old_log_file_max_num( unsigned short old_log_file_max_num ) noexcept {
                this -> _old_log_file_max_num = old_log_file_max_num;
                return;
            }
            inline void set_min_severity( RENALOGSEVERITY severity ) noexcept {
                this -> _min_severity = severity;
                return;
            }
            inline bool is_severity_need_to_log( RENALOGSEVERITY severity ) const noexcept {
                if ( severity >= this -> _min_severity )
                {
                    return true;
                }
                return false;
            }


        private:
            std::filesystem::path _logdir;
            CPSTR _nametag = CPTEXT( "" );
            std::fstream _rwF;
            unsigned short _old_log_file_max_num = static_cast<unsigned short>( -1 );
            RENALOGSEVERITY _min_severity;
            std::mutex _global_mutex;

    }; // class renalog

    std::ostream& operator<<( std::ostream& os , renalog::RENALOGSEVERITY severity );

    template <typename T>
    inline renalog& operator<<( renalog& rg , const T& data ){
        rg._rwF << data;
        return rg;
    }

    extern renalog* __global_logger__;

}; // namespace rena

#endif