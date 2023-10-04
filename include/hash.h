#ifndef _HASH_H_
#define _HASH_H_

#include<string>
#include<cstdio>
#include<cstdlib>
#include<fstream>

#include"openssl/md5.h"
#include"openssl/sha.h"

#ifndef RFILE_BLOCK_SIZE
#define RFILE_BLOCK_SIZE 1024
#endif

namespace rena {
    std::string calc_file_md5( FILE* rFile );
    std::string calc_file_sha1( FILE* rFile );
    std::string calc_file_sha256( FILE* rFile );
    std::string calc_file_sha512( FILE* rFile );
}; // namespace rena

#endif