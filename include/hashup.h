#ifndef _HASHUP_H_
#define _HASHUP_H_

#include<iostream>
#include<iomanip>
#include<string>
#include<cstdio>
#include<cstdlib>
#include<fstream>
#include<filesystem>
#include<vector>
#include<codecvt>
#include<cstring>
#include<chrono>
#include<mutex>
#include<cerrno>
#include<thread>
#include<unordered_map>
#include<atomic>
#include<sstream>
#include<cmath>
#ifdef SHOW_PROGRESS_DETAIL
#ifdef WIN32
#include<conio.h>
#define kbhit _kbhit
#define getch _getch
#elif defined( __linux__ )
#include<termios.h>
#include<unistd.h>
#include<sys/select.h>
#include<sys/ioctl.h>
#define kbhit rena::_kbhit
#define getch rena::_getch
#else
#define kbhit
#define getch
#endif
#endif

#include"openssl/md5.h"
#include"openssl/sha.h"
#ifdef USE_OPENSSL_EVP
#include"openssl/evp.h"
#endif
#include"utils.h"
#include"ThreadPool.h"
#include"cppfignore.h"
#include"rich.h"

namespace rena {

////////////////////////////////////////////////////////////
//                      calchash.cpp                      //
////////////////////////////////////////////////////////////

#ifndef RFILE_BLOCK_SIZE
#define RFILE_BLOCK_SIZE 1024
#endif

    extern std::atomic<bool> pause_signal;

    CPSTR calc_file_md5( const std::filesystem::path& path );
    CPSTR calc_file_sha1( const std::filesystem::path& path );
    CPSTR calc_file_sha224( const std::filesystem::path& path );
    CPSTR calc_file_sha256( const std::filesystem::path& path );
    CPSTR calc_file_sha384( const std::filesystem::path& path );
    CPSTR calc_file_sha512( const std::filesystem::path& path );
#ifdef USE_OPENSSL_EVP
    CPSTR calc_file_hash( const std::filesystem::path& path , const EVP_MD* algo );
    CPSTR calc_file_sha3_224( const std::filesystem::path& path );
    CPSTR calc_file_sha3_256( const std::filesystem::path& path );
    CPSTR calc_file_sha3_384( const std::filesystem::path& path );
    CPSTR calc_file_sha3_512( const std::filesystem::path& path );
    CPSTR calc_file_shake128( const std::filesystem::path& path );
    CPSTR calc_file_shake256( const std::filesystem::path& path );
#endif

////////////////////////////////////////////////////////////
//                        hufo.cpp                        //
////////////////////////////////////////////////////////////

    class HUFO {

        public:
            typedef enum {
                OK,             // success
                OPENFILEERR,    // open file error
                OPENIGFERR,     // open ignore file error
                PARSEIGFERR,    // parse ignore file error
                FILENOTEXIST,   // file doesn't exist (mainly when doing hash check)
                INTERRUPT,      // function interrupted
                NOWORKINGDIR,   // no parent dir path
                HUFNOTOPEN,     // HashUp File isn't opened and ready to read/write
                HMODENOTSET,    // hash mode not set (by hash check)
                HASCHECKFAILEDF // hash hash check failed files
            } HUFOSTATUS;

        public:
            HUFO(){};
            ~HUFO();
            HUFOSTATUS open( const std::filesystem::path& path , HASHPURPOSE p , bool _ol_no_ask );
            HUFOSTATUS open( const std::filesystem::path& path , HASHPURPOSE p , bool _ol_no_ask , const std::filesystem::path& ignore_file_path );
            void set_mode( HASHMODE mode );
            HUFOSTATUS start( unsigned short threads );
            HUFOSTATUS do_create( unsigned short threads );
            HUFOSTATUS do_check( unsigned short threads );
            friend void watch_kb_signal( const HUFO* hufoobj );
            friend class speedwatcher;

        private:
            typedef struct {
                std::filesystem::path       fp;
                size_t                      fsize;         // file size
                std::shared_future<CPSTR>   hash_future;
                CPSTR                       hash;
                CPSTR                       hash_readin;   // read in hash (only be used when checking)

            } HASHOBJ;
            typedef std::vector<HASHOBJ> HASHLIST;
            typedef CPSTR ( *HASHFUNCTIONHOOK )( const std::filesystem::path& path );

        private:
            void _traversal_dir_write_to_hlist( const std::filesystem::path& dir );
            void _read_huf_write_to_hlist();
            HUFOSTATUS _do_hashcalc( unsigned short threads );

        private:
            std::filesystem::path   _pdpath;            // parent dir path
            std::filesystem::path   _hufopath;          // HUFO path
            std::fstream            _rwF;               // read write file
            HASHMODE                _hmode;             // hash mode
            HASHFUNCTIONHOOK        _hf = nullptr;      // hash function
            unsigned short          _hlen;              // hash length
            size_t                  _tfsize = 0;        // total file size
            HASHPURPOSE             _hpurpose;          // hash purpose
            HASHLIST                _hlist;             // hash list
            size_t                  _ori_hlist_len = 0; // hash list length at very first step
            HASHLIST                _errhlist;          // error hash list
            cppfignore              _figobj;            // file ignore object
            bool                    _using_fig = false; // using file ignore

    }; // class HUFO (HashUp File Object)

#ifdef SHOW_PROGRESS_DETAIL

////////////////////////////////////////////////////////////
//                        kbs.cpp                         //
////////////////////////////////////////////////////////////

#ifdef __linux__
    int _kbhit();
    char _getch();
#endif
    void watch_kb_signal( const HUFO* hufoobj );

////////////////////////////////////////////////////////////
//                     speedwatch.cpp                     //
////////////////////////////////////////////////////////////

    class speedwatcher {
        public:
            speedwatcher( std::chrono::steady_clock::time_point start_time_point ) 
                : start_time( start_time_point ) , total_duration( 0 ) , total_size( 0 ) , finished_num( 0 ){};
            ~speedwatcher(){};

            void add( size_t size );
            void start_one( std::thread::id thread_id , std::filesystem::path path );
            void finished_one( std::thread::id thread_id );
            size_t get_speed();
            size_t get_finished();
            CPSTR get_expected_time_left( const HUFO* hufoobj );
            double get_duration_s();
            std::vector<std::filesystem::path> get_files_in_process();
            void pause_watch();
            void resume_watch();

        private:
            std::chrono::steady_clock::time_point start_time; // start time this timekeeping period
            std::chrono::microseconds total_duration;
            size_t total_size;      // total size processed
            size_t finished_num;
            std::mutex global_mutex;
            std::unordered_map<std::thread::id,std::filesystem::path> file_in_process;
            void push_timekeeping_period();
    }; // class speedwatcher

    extern speedwatcher* global_speed_watcher;

#endif

}; // namespace rena

#endif