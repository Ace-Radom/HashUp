#include"hashup.h"

CPSTR dump_CHAR_to_HEX( const unsigned char* hash , int len ){
    std::string out;
    for ( int i = 0 ; i < len ; i++ )
    {
        char dump_HEX_temp[3];
        memset( dump_HEX_temp , '\0' , sizeof( dump_HEX_temp ) );
        sprintf( dump_HEX_temp , "%02x" ,  hash[i] );
        out += dump_HEX_temp;
    }
#ifdef _MSC_VER
    std::wstring_convert<std::codecvt_utf8<wchar_t>> conv;
    return conv.from_bytes( out );
#else
    return out;
#endif
}

/**
 * FILE objects will NOT be opened or closed in the following functions
*/

CPSTR rena::calc_file_md5( std::filesystem::path path ){
    std::ifstream rFile( path );
    if ( !rFile.is_open() )
    {
        throw std::exception( "Open file failed" );
    }

    MD5_CTX ctx;
    MD5_Init( &ctx );
    errno_t ret;
    char buf[RFILE_BLOCK_SIZE];
    unsigned char out[MD5_DIGEST_LENGTH];
    while ( 1 )
    {
        if ( rFile.eof() )
        {
            break;
        }
        rFile.read( buf , sizeof( buf ) );
        ret = rFile.gcount();
        MD5_Update( &ctx , buf , ret );
    }
    MD5_Final( out , &ctx );
    rFile.close();
    return dump_CHAR_to_HEX( out , MD5_DIGEST_LENGTH );
}

CPSTR rena::calc_file_sha1( std::filesystem::path path ){
    std::ifstream rFile( path );
    if ( !rFile.is_open() )
    {
        throw std::exception( "Open file failed" );
    }

    SHA_CTX ctx;
    SHA1_Init( &ctx );
    errno_t ret;
    char buf[RFILE_BLOCK_SIZE];
    unsigned char out[SHA_DIGEST_LENGTH];
    while ( 1 )
    {
        if ( rFile.eof() )
        {
            break;
        }
        rFile.read( buf , sizeof( buf ) );
        ret = rFile.gcount();
        SHA1_Update( &ctx , ( char* ) buf , ret );
    }
    SHA1_Final( out , &ctx );
    rFile.close();
    return dump_CHAR_to_HEX( out , SHA_DIGEST_LENGTH );
}

CPSTR rena::calc_file_sha256( std::filesystem::path path ){
    std::ifstream rFile( path );
    if ( !rFile.is_open() )
    {
        throw std::exception( "Open file failed" );
    }

    SHA256_CTX ctx;
    SHA256_Init( &ctx );
    errno_t ret;
    char buf[RFILE_BLOCK_SIZE];
    unsigned char out[SHA256_DIGEST_LENGTH];
    while ( 1 )
    {
        if ( rFile.eof() )
        {
            break;
        }
        rFile.read( buf , sizeof( buf ) );
        ret = rFile.gcount();
        SHA256_Update( &ctx , ( char* ) buf , ret );
    }
    SHA256_Final( out , &ctx );
    rFile.close();
    return dump_CHAR_to_HEX( out , SHA256_DIGEST_LENGTH );
}

CPSTR rena::calc_file_sha512( std::filesystem::path path ){
    std::ifstream rFile( path );
    if ( !rFile.is_open() )
    {
        throw std::exception( "Open file failed" );
    }

    SHA512_CTX ctx;
    SHA512_Init( &ctx );
    errno_t ret;
    char buf[RFILE_BLOCK_SIZE];
    unsigned char out[SHA512_DIGEST_LENGTH];
    while ( 1 )
    {
        if ( rFile.eof() )
        {
            break;
        }
        rFile.read( buf , sizeof( buf ) );
        ret = rFile.gcount();
        SHA512_Update( &ctx , ( char* ) buf , ret );
    }
    SHA512_Final( out , &ctx );
    rFile.close();
    return dump_CHAR_to_HEX( out , SHA512_DIGEST_LENGTH );
}