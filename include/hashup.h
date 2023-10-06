#ifndef _HASHUP_H_
#define _HASHUP_H_

#include<iostream>
#include<string>
#include<cstdio>
#include<cstdlib>
#include<fstream>
#include<filesystem>
#include<vector>

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
            HUFOSTATUS do_create( unsigned short threads );

        private:
            typedef struct {
                std::string fp;
                std::string hash;
            } HASHOBJ;
            typedef std::vector<HASHOBJ> HASHLIST;

        private:
            void _traversal_dir_write_to_hlist( const std::string& dir );

        private:
            std::string  _dpath;        // parent dir path
            std::fstream _rwF;          // read write file
            HASHMODE     _hmode;        // hash mode
            HASHPURPOSE  _hpurpose;     // hash purpose
            HASHLIST     _hlist;        // hash list

    }; // class HUFO (HashUp File Object)

}; // namespace rena

#endif