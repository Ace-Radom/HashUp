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
    return CPATOWCONV( out );
}

#ifdef USE_OPENSSL_EVP

CPSTR rena::calc_file_hash( const std::filesystem::path& path , const EVP_MD* algo ){
    try {
        std::ifstream rFile( path , std::ios::binary );
        if ( !rFile.is_open() )
        {
            throw std::runtime_error( "Open file failed" );
        }

        EVP_MD_CTX* ctx = EVP_MD_CTX_create();
        EVP_DigestInit_ex( ctx , algo , NULL );
        size_t ret;
        char buf[RFILE_BLOCK_SIZE];
        unsigned char out[EVP_MAX_MD_SIZE];
        unsigned int out_len;
        while ( !rFile.eof() )
        {
            rFile.read( buf , sizeof( buf ) );
            ret = rFile.gcount();
#ifdef SHOW_PROGRESS_DETAIL
            if ( global_speed_watcher != nullptr )
            {
                global_speed_watcher -> add( ret );
            }
#endif
            EVP_DigestUpdate( ctx , ( char* ) buf , ret );
        }
        EVP_DigestFinal_ex( ctx , out , &out_len );
        rFile.close();
        EVP_MD_CTX_destroy( ctx );
#ifdef SHOW_PROGRESS_DETAIL
        if ( global_speed_watcher != nullptr )
        {
            global_speed_watcher -> finished_one();
        }
#endif
        return dump_CHAR_to_HEX( out , out_len );
    }
    catch ( const std::exception& e )
    {
#ifdef SHOW_PROGRESS_DETAIL
        if ( global_speed_watcher != nullptr )
        {
            global_speed_watcher -> finished_one();
        }
#endif
        throw e;
    } // call finish_one and rethrow exception
}

CPSTR rena::calc_file_md5( const std::filesystem::path& path ){
    return calc_file_hash( path , EVP_md5() );
}

CPSTR rena::calc_file_sha1( const std::filesystem::path& path ){
    return calc_file_hash( path , EVP_sha1() );
}

CPSTR rena::calc_file_sha224( const std::filesystem::path& path ){
    return calc_file_hash( path , EVP_sha224() );
}

CPSTR rena::calc_file_sha256( const std::filesystem::path& path ){
    return calc_file_hash( path , EVP_sha256() );
}

CPSTR rena::calc_file_sha384( const std::filesystem::path& path ){
    return calc_file_hash( path , EVP_sha384() );
}

CPSTR rena::calc_file_sha512( const std::filesystem::path& path ){
    return calc_file_hash( path , EVP_sha512() );
}

CPSTR rena::calc_file_sha3_224( const std::filesystem::path& path ){
    return calc_file_hash( path , EVP_sha3_224() );
}

CPSTR rena::calc_file_sha3_256( const std::filesystem::path& path ){
    return calc_file_hash( path , EVP_sha3_256() );
}

CPSTR rena::calc_file_sha3_384( const std::filesystem::path& path ){
    return calc_file_hash( path , EVP_sha3_384() );
}

CPSTR rena::calc_file_sha3_512( const std::filesystem::path& path ){
    return calc_file_hash( path , EVP_sha3_512() );
}

CPSTR rena::calc_file_shake128( const std::filesystem::path& path ){
    return calc_file_hash( path , EVP_shake128() );
}

CPSTR rena::calc_file_shake256( const std::filesystem::path& path ){
    return calc_file_hash( path , EVP_shake256() );
}

#else

CPSTR rena::calc_file_md5( const std::filesystem::path& path ){
    std::ifstream rFile( path , std::ios::binary );
    if ( !rFile.is_open() )
    {
        throw std::runtime_error( "Open file failed" );
    }

    MD5_CTX ctx;
    MD5_Init( &ctx );
    size_t ret;
    char buf[RFILE_BLOCK_SIZE];
    unsigned char out[MD5_DIGEST_LENGTH];
    while ( !rFile.eof() )
    {
        rFile.read( buf , sizeof( buf ) );
        ret = rFile.gcount();
#ifdef SHOW_PROGRESS_DETAIL
        if ( global_speed_watcher != nullptr )
        {
            global_speed_watcher -> add( ret );
        }
#endif
        MD5_Update( &ctx , buf , ret );
    }
    MD5_Final( out , &ctx );
    rFile.close();
    return dump_CHAR_to_HEX( out , MD5_DIGEST_LENGTH );
}

CPSTR rena::calc_file_sha1( const std::filesystem::path& path ){
    std::ifstream rFile( path , std::ios::binary );
    if ( !rFile.is_open() )
    {
        throw std::runtime_error( "Open file failed" );
    }

    SHA_CTX ctx;
    SHA1_Init( &ctx );
    size_t ret;
    char buf[RFILE_BLOCK_SIZE];
    unsigned char out[SHA_DIGEST_LENGTH];
    while ( !rFile.eof() )
    {
        rFile.read( buf , sizeof( buf ) );
        ret = rFile.gcount();
#ifdef SHOW_PROGRESS_DETAIL
        if ( global_speed_watcher != nullptr )
        {
            global_speed_watcher -> add( ret );
        }
#endif
        SHA1_Update( &ctx , ( char* ) buf , ret );
    }
    SHA1_Final( out , &ctx );
    rFile.close();
    return dump_CHAR_to_HEX( out , SHA_DIGEST_LENGTH );
}

CPSTR rena::calc_file_sha224( const std::filesystem::path& path ){
    std::ifstream rFile( path , std::ios::binary );
    if ( !rFile.is_open() )
    {
        throw std::runtime_error( "Open file failed" );
    }

    SHA256_CTX ctx;
    SHA224_Init( &ctx );
    size_t ret;
    char buf[RFILE_BLOCK_SIZE];
    unsigned char out[SHA224_DIGEST_LENGTH];
    while ( !rFile.eof() )
    {
        rFile.read( buf , sizeof( buf ) );
        ret = rFile.gcount();
#ifdef SHOW_PROGRESS_DETAIL
        if ( global_speed_watcher != nullptr )
        {
            global_speed_watcher -> add( ret );
        }
#endif
        SHA224_Update( &ctx , ( char* ) buf , ret );
    }
    SHA224_Final( out , &ctx );
    rFile.close();
    return dump_CHAR_to_HEX( out , SHA224_DIGEST_LENGTH );
}

CPSTR rena::calc_file_sha256( const std::filesystem::path& path ){
    std::ifstream rFile( path , std::ios::binary );
    if ( !rFile.is_open() )
    {
        throw std::runtime_error( "Open file failed" );
    }

    SHA256_CTX ctx;
    SHA256_Init( &ctx );
    size_t ret;
    char buf[RFILE_BLOCK_SIZE];
    unsigned char out[SHA256_DIGEST_LENGTH];
    while ( !rFile.eof() )
    {
        rFile.read( buf , sizeof( buf ) );
        ret = rFile.gcount();
#ifdef SHOW_PROGRESS_DETAIL
        if ( global_speed_watcher != nullptr )
        {
            global_speed_watcher -> add( ret );
        }
#endif
        SHA256_Update( &ctx , ( char* ) buf , ret );
    }
    SHA256_Final( out , &ctx );
    rFile.close();
    return dump_CHAR_to_HEX( out , SHA256_DIGEST_LENGTH );
}

CPSTR rena::calc_file_sha384( const std::filesystem::path& path ){
    std::ifstream rFile( path , std::ios::binary );
    if ( !rFile.is_open() )
    {
        throw std::runtime_error( "Open file failed" );
    }

    SHA512_CTX ctx;
    SHA384_Init( &ctx );
    size_t ret;
    char buf[RFILE_BLOCK_SIZE];
    unsigned char out[SHA384_DIGEST_LENGTH];
    while ( !rFile.eof() )
    {
        rFile.read( buf , sizeof( buf ) );
        ret = rFile.gcount();
#ifdef SHOW_PROGRESS_DETAIL
        if ( global_speed_watcher != nullptr )
        {
            global_speed_watcher -> add( ret );
        }
#endif
        SHA384_Update( &ctx , ( char* ) buf , ret );
    }
    SHA384_Final( out , &ctx );
    rFile.close();
    return dump_CHAR_to_HEX( out , SHA384_DIGEST_LENGTH );
}

CPSTR rena::calc_file_sha512( const std::filesystem::path& path ){
    std::ifstream rFile( path , std::ios::binary );
    if ( !rFile.is_open() )
    {
        throw std::runtime_error( "Open file failed" );
    }

    SHA512_CTX ctx;
    SHA512_Init( &ctx );
    size_t ret;
    char buf[RFILE_BLOCK_SIZE];
    unsigned char out[SHA512_DIGEST_LENGTH];
    while ( !rFile.eof() )
    {
        rFile.read( buf , sizeof( buf ) );
        ret = rFile.gcount();
#ifdef SHOW_PROGRESS_DETAIL
        if ( global_speed_watcher != nullptr )
        {
            global_speed_watcher -> add( ret );
        }
#endif
        SHA512_Update( &ctx , ( char* ) buf , ret );
    }
    SHA512_Final( out , &ctx );
    rFile.close();
    return dump_CHAR_to_HEX( out , SHA512_DIGEST_LENGTH );
}

#endif
