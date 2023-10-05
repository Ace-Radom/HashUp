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
                DIRASFILE,      // directory gave in as file
                FILENOTEXIST    // file doesn't exist (mainly when doing hash check)
            } HUFOSTATUS;

        public:
            HUFO(){};
            ~HUFO();
            HUFOSTATUS open( const std::string& path , HASHPURPOSE p );
            void set_mode( HASHMODE mode );
            void set_purpose( HASHPURPOSE purpose );

        private:
            typedef struct {
                std::string fp;
                std::string hash;
            } HASHOBJ;
            typedef std::vector<HASHOBJ> HASHLIST;

        private:
            std::string  _path;
            std::fstream _rwF;
            HASHMODE     _hmode;
            HASHPURPOSE  _hpurpose;
            HASHLIST     _hlist;

    }; // class HUFO (HashUp File Object)

}; // namespace rena

#endif