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

    std::string calc_file_md5( FILE* rFile );
    std::string calc_file_sha1( FILE* rFile );
    std::string calc_file_sha256( FILE* rFile );
    std::string calc_file_sha512( FILE* rFile );

////////////////////////////////////////////////////////////
//                        hufo.cpp                        //
////////////////////////////////////////////////////////////

    class HUFO {

        public:
            typedef enum {
                OK,             // success
                OPENFILEERR,    // open file error
                FILENOTEXIST,   // file doesn't exist (mainly when doing hash check)
                INTERRUPT       // function interrupted
            } HUFOSTATUS;

        public:
            HUFO(){};
            ~HUFO();
            HUFOSTATUS open( const std::string& path , HASHPURPOSE p );
            void set_mode( HASHMODE mode );
            void set_purpose( HASHPURPOSE purpose );
            HUFOSTATUS do_hashcalc( unsigned short threads );

        private:
            typedef struct {
                std::filesystem::path           fp;
                std::shared_future<std::string> hash_future;
                std::string                     hash;

            } HASHOBJ;
            typedef std::vector<HASHOBJ> HASHLIST;
            typedef std::string ( *HASHFUNCTIONHOOK )( FILE* rFile );

        private:
            void _traversal_dir_write_to_hlist( const std::filesystem::path& dir );

        private:
            std::filesystem::path   _pdpath;        // parent dir path
            std::fstream            _rwF;           // read write file
            HASHMODE                _hmode;         // hash mode
            HASHFUNCTIONHOOK        _hf = nullptr;  // hash function 
            HASHPURPOSE             _hpurpose;      // hash purpose
            HASHLIST                _hlist;         // hash list

    }; // class HUFO (HashUp File Object)

}; // namespace rena

#endif