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

#include"openssl/md5.h"
#include"openssl/sha.h"
#ifdef USE_OPENSSL_EVP
#include"openssl/evp.h"
#endif
#include"utils.h"
#include"ThreadPool.h"
#include"cppfignore.h"

namespace rena {

////////////////////////////////////////////////////////////
//                      calchash.cpp                      //
////////////////////////////////////////////////////////////

#ifndef RFILE_BLOCK_SIZE
#define RFILE_BLOCK_SIZE 1024
#endif

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

        private:
            typedef struct {
                std::filesystem::path       fp;
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
            HASHPURPOSE             _hpurpose;          // hash purpose
            HASHLIST                _hlist;             // hash list
            size_t                  _ori_hlist_len = 0; // hash list length at very first step
            HASHLIST                _errhlist;          // error hash list
            cppfignore              _figobj;            // file ignore object
            bool                    _using_fig = false; // using file ignore

    }; // class HUFO (HashUp File Object)

#ifdef SHOW_PROGRESS_DETAIL

////////////////////////////////////////////////////////////
//                     speedwatch.cpp                     //
////////////////////////////////////////////////////////////

    class speedwatcher {
        public:
            speedwatcher( std::chrono::steady_clock::time_point start_time_point ) 
                : start_time( start_time_point ) , total_size( 0 ){};
            ~speedwatcher(){};

            void add( size_t size );
            size_t get_speed();

        private:
            std::chrono::steady_clock::time_point start_time;
            size_t total_size;
            std::mutex global_mutex;
    }; // class speedwatcher

    extern speedwatcher* global_speed_watcher;

#endif

}; // namespace rena

#endif