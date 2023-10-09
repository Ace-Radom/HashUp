#ifndef _HASHUP_H_
#define _HASHUP_H_

#include<iostream>
#include<string>
#include<cstdio>
#include<cstdlib>
#include<fstream>
#include<filesystem>
#include<vector>
#include<cerrno>
#include<codecvt>

#include"openssl/md5.h"
#include"openssl/sha.h"
#include"utils.h"
#include"ThreadPool.h"

namespace rena {

////////////////////////////////////////////////////////////
//                      calchash.cpp                      //
////////////////////////////////////////////////////////////

#ifndef RFILE_BLOCK_SIZE
#define RFILE_BLOCK_SIZE 1024
#endif

    CPSTR calc_file_md5( std::filesystem::path path );
    CPSTR calc_file_sha1( std::filesystem::path path );
    CPSTR calc_file_sha256( std::filesystem::path path );
    CPSTR calc_file_sha512( std::filesystem::path path );

////////////////////////////////////////////////////////////
//                        hufo.cpp                        //
////////////////////////////////////////////////////////////

    class HUFO {

        public:
            typedef enum {
                OK,             // success
                OPENFILEERR,    // open file error
                FILENOTEXIST,   // file doesn't exist (mainly when doing hash check)
                INTERRUPT,      // function interrupted
                NOWORKINGDIR,   // no parent dir path
                HUFNOTOPEN      // HashUp File isn't opened and ready to read/write
            } HUFOSTATUS;

        public:
            HUFO(){};
            ~HUFO();
            HUFOSTATUS open( const CPSTR& path , HASHPURPOSE p );
            void set_mode( HASHMODE mode );
            void set_purpose( HASHPURPOSE purpose );
            HUFOSTATUS do_create( unsigned short threads );
            void test();

        private:
            typedef struct {
                std::filesystem::path       fp;
                std::shared_future<CPSTR>   hash_future;
                CPSTR                       hash;

            } HASHOBJ;
            typedef std::vector<HASHOBJ> HASHLIST;
            typedef CPSTR ( *HASHFUNCTIONHOOK )( std::filesystem::path path );

        private:
            void _traversal_dir_write_to_hlist( const std::filesystem::path& dir );
            HUFOSTATUS _do_hashcalc( unsigned short threads );

        private:
            std::filesystem::path   _pdpath;        // parent dir path
            std::filesystem::path   _hufopath;      // HUFO path
            std::fstream            _rwF;           // read write file
            HASHMODE                _hmode;         // hash mode
            HASHFUNCTIONHOOK        _hf = nullptr;  // hash function 
            HASHPURPOSE             _hpurpose;      // hash purpose
            HASHLIST                _hlist;         // hash list

    }; // class HUFO (HashUp File Object)

}; // namespace rena

#endif