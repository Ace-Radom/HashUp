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
                HUFNOTOPEN,     // HashUp File isn't opened and ready to read/write
                HMODENOTSET,    // hash mode not set (by hash check)
                HASCHECKFAILEDF // hash hash check failed files
            } HUFOSTATUS;

        public:
            HUFO(){};
            ~HUFO();
            HUFOSTATUS open( const std::filesystem::path& path , HASHPURPOSE p );
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
            typedef CPSTR ( *HASHFUNCTIONHOOK )( std::filesystem::path path );

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

    }; // class HUFO (HashUp File Object)

}; // namespace rena

#endif