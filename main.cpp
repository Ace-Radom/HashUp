#include<iostream>
#include<filesystem>
#ifdef _MSC_VER
#include<io.h>
#include<fcntl.h>
#endif

#include"cmdline.h"
#include"hashup.h"
#include"build_config.h"

void print_hufo_err( rena::HUFO::HUFOSTATUS tag );

int main( int argc , char** argv ){

#ifdef _MSC_VER
    _setmode( _fileno( stdout ) , _O_WTEXT );
    _setmode( _fileno( stderr ) , _O_WTEXT );
#endif

#pragma region create_cmd_parser

    cmdline::parser cmdparser;
    cmdparser.add                ( "help"    , '?'  , "Show this help page" );
    cmdparser.add                ( "create"  , 'w'  , "Create a hash list for a directory" );
    cmdparser.add                ( "check"   , 'r'  , "Do hash check for a directory" );
    cmdparser.add<std::string>   ( "file"    , 'f'  , "The path of the hash list"                            , true  , "" );
    cmdparser.add                ( "single"  , 's'  , "Use single file mode" );
    cmdparser.add<std::string>   ( "hash"    , '\0' , "File hash (only available by single file check)"      , false );
    cmdparser.add<std::string>   ( "mode"    , 'm'  , "Set hash mode (md5, sha1, sha256, sha512)"            , false , "md5" , cmdline::oneof<std::string>( "md5" , "sha1" , "sha256" , "sha512" ) );
    cmdparser.add<unsigned short>( "thread"  , 'j'  , "Set the thread-number of multithreading acceleration" , false , 8     , cmdline::range<unsigned short>( 1 , 128 ) );
    cmdparser.add                ( "version" , 'v'  , "Show HashUp version" );
    cmdparser.set_program_name( "hashup" );

#pragma endregion create_cmd_parser

    bool pret = cmdparser.parse( argc , argv );
    if ( !pret )
    {
        if ( !cmdparser.exist( "help" ) && !cmdparser.exist( "version" ) )
        {
            CPOUT << CPATOWCONV( cmdparser.error() ) << std::endl << CPATOWCONV( cmdparser.usage() ) << std::endl;
            return 128;
        } // show help
        else if ( cmdparser.exist( "help" ) )
        {
            CPOUT << CPATOWCONV( cmdparser.usage() ) << std::endl;
            return 0;
        } // show help page
        else
        {
            CPOUT << "HashUp " << HASHUP_VERSION << " (branch/" << BUILD_GIT_BRANCH << ":" << BUILD_GIT_COMMIT << ", " << BUILD_TIME << ") [" << CXX_COMPILER_ID << " " << CXX_COMPILER_VERSION << "] on " << BUILD_SYS_NAME << std::endl;
            return 0;
        } // show version
    } // arg error

    rena::HASHPURPOSE p;
    if ( cmdparser.exist( "create" ) && cmdparser.exist( "check" ) )
    {
        CPERR << "Cannot create hash list and do hash check at the same time, exit." << std::endl;
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
            return 127;
        }

        std::filesystem::path fp( cmdparser.get<std::string>( "file" ) );
        CPSTR hash;
        std::string mode = cmdparser.get<std::string>( "mode" );
        DEBUG_MSG( "Set Hash Mode: " << CPATOWCONV( mode ) );
        try {
            if ( mode == "md5" )
            {
                hash = rena::calc_file_md5( fp );
            }
            else if ( mode == "sha1" )
            {
                hash = rena::calc_file_sha1( fp );
            }
            else if ( mode == "sha256" )
            {
                hash = rena::calc_file_sha256( fp );
            }
            else if ( mode == "sha512" )
            {
                hash = rena::calc_file_sha512( fp );
            }
        }
        catch ( const std::exception& e )
        {
            CPERR << "Operate file \"" << CPPATHTOSTR( fp ) << "\" failed: " << e.what() << std::endl
                  << "Exit." << std::endl;
            return 127;
        }
        
        if ( p == rena::HASHPURPOSE::CREATE )
        {
            CPOUT << hash << std::endl;
        }
        else if ( p == rena::HASHPURPOSE::CHECK )
        {
            if ( hash == CPATOWCONV( cmdparser.get<std::string>( "hash" ) ) )
            {
                CPOUT << "Passed." << std::endl;
            }
            else
            {
                CPOUT << "Failed: got " << CPATOWCONV( cmdparser.get<std::string>( "hash" ) ) << ", should be " << hash << "." << std::endl;
            }
        }
        return 0;
    } // single file mode

    rena::HUFO hufo;

    std::filesystem::path huf_path_get = cmdparser.get<std::string>( "file" );
    if ( huf_path_get.is_relative() )
    {
        huf_path_get = std::filesystem::current_path() / huf_path_get;
    } // relative huf path

    rena::HUFO::HUFOSTATUS open_status = hufo.open( huf_path_get , p );
    if ( open_status != rena::HUFO::HUFOSTATUS::OK )
    {
        print_hufo_err( open_status );
        return open_status;
    } // open failed

    if ( cmdparser.exist( "mode" ) )
    {
        std::string mode = cmdparser.get<std::string>( "mode" );
        DEBUG_MSG( "Set Hash Mode: " << CPATOWCONV( mode ) );
        if ( mode == "md5" )
        {
            hufo.set_mode( rena::HASHMODE::MD5 );
        }
        else if ( mode == "sha1" )
        {
            hufo.set_mode( rena::HASHMODE::SHA1 );
        }
        else if ( mode == "sha256" )
        {
            hufo.set_mode( rena::HASHMODE::SHA256 );
        }
        else if ( mode == "sha512" )
        {
            hufo.set_mode( rena::HASHMODE::SHA512 );
        }
    } // set hash mode

    rena::HUFO::HUFOSTATUS do_operate_status = hufo.start( cmdparser.get<unsigned short>( "thread" ) );
    if ( do_operate_status != rena::HUFO::HUFOSTATUS::OK && do_operate_status != rena::HUFO::HUFOSTATUS::HASCHECKFAILEDF )
    {
        print_hufo_err( do_operate_status );
        return do_operate_status;
    }
    else
    {
        CPOUT << "Work done and success." << std::endl;
    }

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