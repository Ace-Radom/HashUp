#include"hash.h"

std::string dump_CHAR_to_HEX( const unsigned char* hash , int len ){
    std::string out;
    for ( int i = 0 ; i < len ; i++ )
    {
        char dump_HEX_temp[3];
        memset( dump_HEX_temp , '\0' , sizeof( dump_HEX_temp ) );
        sprintf( dump_HEX_temp , "%02x" ,  hash[i] );
        out += dump_HEX_temp;
    }
    return out;
}

/**
 * FILE objects will NOT be opened or closed in the following functions
*/

std::string rena::calc_file_md5( FILE* rFile ){
    MD5_CTX ctx;
    MD5_Init( &ctx );
    errno_t ret;
    unsigned char buf[RFILE_BLOCK_SIZE];
    unsigned char out[MD5_DIGEST_LENGTH];
    while ( 1 )
    {
        if ( ( ret = fread( buf , sizeof( unsigned char ) , sizeof( buf ) , rFile ) ) == -1 )
        {
            return "\0";
        }
        MD5_Update( &ctx , ( char* ) buf , ret );
        if ( ret == 0 || ret < RFILE_BLOCK_SIZE )
        {
            break;
        }
    }
    MD5_Final( out , &ctx );
    return dump_CHAR_to_HEX( out , MD5_DIGEST_LENGTH );
}

std::string rena::calc_file_sha1( FILE* rFile ){
    SHA_CTX ctx;
    SHA1_Init( &ctx );
    errno_t ret;
    unsigned char buf[RFILE_BLOCK_SIZE];
    unsigned char out[SHA_DIGEST_LENGTH];
    while ( 1 )
    {
        if ( ( ret = fread( buf , sizeof( unsigned char ) , sizeof( buf ) , rFile ) ) == -1 )
        {
            return "\0";
        }
        SHA1_Update( &ctx , ( char* ) buf , ret );
        if ( ret == 0 || ret < RFILE_BLOCK_SIZE )
        {
            break;
        }
    }
    SHA1_Final( out , &ctx );
    return dump_CHAR_to_HEX( out , SHA_DIGEST_LENGTH );
}

std::string rena::calc_file_sha256( FILE* rFile ){
    SHA256_CTX ctx;
    SHA256_Init( &ctx );
    errno_t ret;
    unsigned char buf[RFILE_BLOCK_SIZE];
    unsigned char out[SHA256_DIGEST_LENGTH];
    while ( 1 )
    {
        if ( ( ret = fread( buf , sizeof( unsigned char ) , sizeof( buf ) , rFile ) ) == -1 )
        {
            return "\0";
        }
        SHA256_Update( &ctx , ( char* ) buf , ret );
        if ( ret == 0 || ret < RFILE_BLOCK_SIZE )
        {
            break;
        }
    }
    SHA256_Final( out , &ctx );
    return dump_CHAR_to_HEX( out , SHA256_DIGEST_LENGTH );
}

std::string rena::calc_file_sha512( FILE* rFile ){
    SHA512_CTX ctx;
    SHA512_Init( &ctx );
    errno_t ret;
    unsigned char buf[RFILE_BLOCK_SIZE];
    unsigned char out[SHA512_DIGEST_LENGTH];
    while ( 1 )
    {
        if ( ( ret = fread( buf , sizeof( unsigned char ) , sizeof( buf ) , rFile ) ) == -1 )
        {
            return "\0";
        }
        SHA512_Update( &ctx , ( char* ) buf , ret );
        if ( ret == 0 || ret < RFILE_BLOCK_SIZE )
        {
            break;
        }
    }
    SHA512_Final( out , &ctx );
    return dump_CHAR_to_HEX( out , SHA512_DIGEST_LENGTH );
}