#include<iostream>
#include<filesystem>
#ifdef _MSC_VER
#include<io.h>
#include<fcntl.h>
#endif

#include"cmdline.h"
#include"hashup.h"
#include"build_config.h"

#ifdef _MSC_VER
#define FREE_ARGV delete[] argv;
#else
#define FREE_ARGV ;
#endif

void print_hufo_err( rena::HUFO::HUFOSTATUS tag );

#ifdef _MSC_VER
int wmain( int argc , wchar_t** wargv ){
    _setmode( _fileno( stdout ) , _O_WTEXT );
    _setmode( _fileno( stderr ) , _O_WTEXT );

    char** argv = new char*[ argc ];
    for ( int i = 0 ; i < argc ; i++ )
    {
        argv[i] = new char[ wcslen( wargv[i] ) * sizeof( wchar_t ) * 2 ];
        strcpy( argv[i] , CPWTOACONV( wargv[i] ).c_str() );
    }
#else
int main( int argc , char** argv ){
#endif

#pragma region create_cmd_parser

    cmdline::parser cmdparser;
    cmdparser.add                ( "help"    , '?'  , "Show this help page" );
    cmdparser.add                ( "create"  , 'w'  , "Create a hash list for a directory" );
    cmdparser.add                ( "check"   , 'r'  , "Do hash check for a directory" );
    cmdparser.add<std::string>   ( "file"    , 'f'  , "The path of the hash list"                                                                                     , true  , "" );
    cmdparser.add                ( "single"  , 's'  , "Use single file mode" );
    cmdparser.add<std::string>   ( "hash"    , '\0' , "File hash (only available by single file check)"                                                               , false );
#ifdef USE_OPENSSL_EVP
    cmdparser.add<std::string>   ( "mode"    , 'm'  , "Set hash mode (md5, sha1, sha224, sha256, sha512, sha3-224, sha3-256, sha3-384, sha3-512, shake128, shake256)" , false , "md5" , cmdline::oneof<std::string>( "md5" , "sha1" , "sha224" , "sha256" , "sha384" , "sha512" , "sha3-224" , "sha3-256" , "sha3-384" , "sha3-512" , "shake128" , "shake256" ) );
#else
    cmdparser.add<std::string>   ( "mode"    , 'm'  , "Set hash mode (md5, sha1, sha224, sha256, sha512)"                                                             , false , "md5" , cmdline::oneof<std::string>( "md5" , "sha1" , "sha224" , "sha256" , "sha384" , "sha512" ) );
#endif
    cmdparser.add<unsigned short>( "thread"  , 'j'  , "Set the thread-number of multithreading acceleration"                                                          , false , 8     , cmdline::range<unsigned short>( 1 , 128 ) );
    cmdparser.add                ( "version" , 'v'  , "Show HashUp version" );
    cmdparser.set_program_name( "hashup" );

#pragma endregion create_cmd_parser

    bool pret = cmdparser.parse( argc , argv );
    if ( !pret )
    {
        if ( !cmdparser.exist( "help" ) && !cmdparser.exist( "version" ) )
        {
            CPOUT << CPATOWCONV( cmdparser.error() ) << std::endl << CPATOWCONV( cmdparser.usage() ) << std::endl;
            FREE_ARGV;
            return 128;
        } // show help
        else if ( cmdparser.exist( "help" ) )
        {
            CPOUT << CPATOWCONV( cmdparser.usage() ) << std::endl;
            FREE_ARGV;
            return 0;
        } // show help page
        else
        {
            CPOUT << "HashUp " << HASHUP_VERSION << " (branch/" << BUILD_GIT_BRANCH << ":" << BUILD_GIT_COMMIT << ", " << BUILD_TIME << ") [" << CXX_COMPILER_ID << " " << CXX_COMPILER_VERSION << "] on " << BUILD_SYS_NAME << std::endl;
            FREE_ARGV;
            return 0;
        } // show version
    } // arg error

    rena::HASHPURPOSE p;
    if ( cmdparser.exist( "create" ) && cmdparser.exist( "check" ) )
    {
        CPERR << "Cannot create hash list and do hash check at the same time, exit." << std::endl;
        FREE_ARGV;
        return 128;
    } // do create and check at the same time
    else if ( cmdparser.exist( "create" ) )
    {
        DEBUG_MSG( "Doing Create" );
        p = rena::HASHPURPOSE::CREATE;
    }
    else if ( cmdparser.exist( "check" ) )
    {
        DEBUG_MSG( "Doing Check" );
        p = rena::HASHPURPOSE::CHECK;
    }
    // get hash purpose

    if ( cmdparser.exist( "single" ) )
    {
        if ( p == rena::HASHPURPOSE::CHECK && !cmdparser.exist( "hash" ) )
        {
            CPERR << "Doing single file hash check but file hash not given, exit." << std::endl;
            FREE_ARGV;
            return 128;
        }

        std::filesystem::path fp( CPATOWCONV( cmdparser.get<std::string>( "file" ) ) );
        CPSTR hash;
        std::string mode = cmdparser.get<std::string>( "mode" );
        DEBUG_MSG( "Set Hash Mode: " << CPATOWCONV( mode ) );
        try {
            if ( mode == "md5" )            hash = rena::calc_file_md5( fp );
            else if ( mode == "sha1" )      hash = rena::calc_file_sha1( fp );
            else if ( mode == "sha224" )    hash = rena::calc_file_sha224( fp );
            else if ( mode == "sha256" )    hash = rena::calc_file_sha256( fp );
            else if ( mode == "sha384" )    hash = rena::calc_file_sha384( fp );
            else if ( mode == "sha512" )    hash = rena::calc_file_sha512( fp );
#ifdef USE_OPENSSL_EVP
            else if ( mode == "sha3-224" )  hash = rena::calc_file_sha3_224( fp );
            else if ( mode == "sha3-256" )  hash = rena::calc_file_sha3_256( fp );
            else if ( mode == "sha3-384" )  hash = rena::calc_file_sha3_384( fp );
            else if ( mode == "sha3-512" )  hash = rena::calc_file_sha3_512( fp );
            else if ( mode == "shake128" )  hash = rena::calc_file_shake128( fp );
            else if ( mode == "shake256" )  hash = rena::calc_file_shake256( fp );
#endif
        }
        catch ( const std::exception& e )
        {
            CPERR << "Operate file \"" << CPPATHTOSTR( fp ) << "\" failed: " << e.what() << std::endl
                  << "Exit." << std::endl;
            FREE_ARGV;
            return 128;
        }
        
        if ( p == rena::HASHPURPOSE::CREATE )
        {
            CPOUT << hash << std::endl;
        }
        else if ( p == rena::HASHPURPOSE::CHECK )
        {
            CPSTR hash_get = CPATOWCONV( cmdparser.get<std::string>( "hash" ) );
            for ( auto& c : hash_get )
            {
                if ( std::isupper( c ) )
                {
                    c = std::tolower( c );
                }
            }

            if ( hash == hash_get )
            {
                CPOUT << "Passed." << std::endl;
            }
            else
            {
                CPOUT << "Failed: got " << hash_get << ", should be " << hash << "." << std::endl;
            }
        }
        FREE_ARGV;
        return 0;
    } // single file mode

    rena::HUFO hufo;

    std::filesystem::path huf_path_get = CPATOWCONV( cmdparser.get<std::string>( "file" ) );
    if ( huf_path_get.is_relative() )
    {
        huf_path_get = std::filesystem::current_path() / huf_path_get;
    } // relative huf path

    rena::HUFO::HUFOSTATUS open_status = hufo.open( huf_path_get , p );
    if ( open_status != rena::HUFO::HUFOSTATUS::OK )
    {
        print_hufo_err( open_status );
        FREE_ARGV;
        return open_status;
    } // open failed

    if ( cmdparser.exist( "mode" ) )
    {
        std::string mode = cmdparser.get<std::string>( "mode" );
        DEBUG_MSG( "Set Hash Mode: " << CPATOWCONV( mode ) );
        if ( mode == "md5" )            hufo.set_mode( rena::HASHMODE::MD5 );
        else if ( mode == "sha1" )      hufo.set_mode( rena::HASHMODE::SHA1 );
        else if ( mode == "sha224" )    hufo.set_mode( rena::HASHMODE::SHA224 );
        else if ( mode == "sha256" )    hufo.set_mode( rena::HASHMODE::SHA256 );
        else if ( mode == "sha384" )    hufo.set_mode( rena::HASHMODE::SHA384 );
        else if ( mode == "sha512" )    hufo.set_mode( rena::HASHMODE::SHA512 );
#ifdef USE_OPENSSL_EVP
        else if ( mode == "sha3-224" )  hufo.set_mode( rena::HASHMODE::SHA3_224 );
        else if ( mode == "sha3-256" )  hufo.set_mode( rena::HASHMODE::SHA3_256 );
        else if ( mode == "sha3-384" )  hufo.set_mode( rena::HASHMODE::SHA3_384 );
        else if ( mode == "sha3-512" )  hufo.set_mode( rena::HASHMODE::SHA3_512 );
        else if ( mode == "shake128" )  hufo.set_mode( rena::HASHMODE::SHAKE128 );
        else if ( mode == "shake256" )  hufo.set_mode( rena::HASHMODE::SHAKE256 );
#endif
    } // set hash mode

    rena::HUFO::HUFOSTATUS do_operate_status = hufo.start( cmdparser.get<unsigned short>( "thread" ) );
    if ( do_operate_status != rena::HUFO::HUFOSTATUS::OK && do_operate_status != rena::HUFO::HUFOSTATUS::HASCHECKFAILEDF )
    {
        print_hufo_err( do_operate_status );
        FREE_ARGV;
        return do_operate_status;
    }
    else
    {
        CPOUT << "Work done and success." << std::endl;
    }

    FREE_ARGV;
    return 0;
}

void print_hufo_err( rena::HUFO::HUFOSTATUS tag ){
    switch ( tag ){
        case rena::HUFO::HUFOSTATUS::OPENFILEERR:
            CPERR << "Open target hash list failed, exit." << std::endl; break;
        case rena::HUFO::HUFOSTATUS::FILENOTEXIST:
            CPERR << "Doing hash check but hash list doesn't exist, exit." << std::endl; break;
        case rena::HUFO::HUFOSTATUS::INTERRUPT:
            CPERR << "Interrupted." << std::endl; break;
        case rena::HUFO::HUFOSTATUS::NOWORKINGDIR:
            CPERR << "Cannot set root work direcotry, exit." << std::endl; break;
        case rena::HUFO::HUFOSTATUS::HUFNOTOPEN:
            CPERR << "Cannot open target hash list file, exit." << std::endl; break;
        case rena::HUFO::HUFOSTATUS::HMODENOTSET:
            CPERR << "Doing hash check but hash mode not set, exit." << std::endl; break;
        case rena::HUFO::HUFOSTATUS::HASCHECKFAILEDF:
            break;
    };
    return;
}