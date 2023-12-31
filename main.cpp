#include<iostream>
#include<filesystem>
#ifdef WIN32
#include<io.h>
#include<fcntl.h>
#endif

#include"cmdline.h"
#include"mini.h"
#include"hashup.h"
#include"renalog.h"
#include"utils.h"
#include"build_config.h"

#ifdef WIN32
#define FREE_ARGV delete[] argv;
#else
#define FREE_ARGV ;
#endif

void print_hufo_err( rena::HUFO::HUFOSTATUS tag );

#ifdef WIN32
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

    rena::rich::rich_global_init();

    signal( SIGINT , rena::handle_syssigint );

    std::filesystem::path hashup_exe_path( rena::get_current_exe_path() );
    std::filesystem::path cfg_path = hashup_exe_path.parent_path() / "hashup.ini";
    if ( std::filesystem::exists( cfg_path ) )
    {
        mINI::INIFile rINI( cfg_path );
        mINI::INIStructure iniobj;
        if ( !rINI.read( iniobj ) )
        {
            CPERR << rena::rich::FColor::RED << "Load config file error." << rena::rich::style_reset << std::endl
                  << "Continue with default settings." << std::endl;
        }
        else
        {
            if ( iniobj.has( "default" ) && iniobj["default"].has( "mode" ) && iniobj["default"].has( "thread" )
                 &&
                 iniobj.has( "renalog" ) && iniobj["renalog"].has( "min_log_severity" ) && iniobj["renalog"].has( "max_old_log_files" ) )
            {
                try {
                    rena::CFG_MODE = iniobj["default"]["mode"];
                    rena::CFG_THREAD = std::stoi( iniobj["default"]["thread"] );
                    rena::CFG_MIN_LOG_SEVERITY = iniobj["renalog"]["min_log_severity"];
                    rena::CFG_MAX_OLD_LOG_FILES = std::stoi( iniobj["renalog"]["max_old_log_files"] );
                }
                catch ( const std::exception& e )
                {
                    CPERR << rena::rich::FColor::RED << "Parse config value error: " << rena::rich::style_reset << e.what() << std::endl
                          << "Continue with default settings." << std::endl;
                }
            }
            else
            {
                CPERR << rena::rich::FColor::RED << "Config file format error." << rena::rich::style_reset << std::endl
                      << "Continue with default settings." << std::endl;
            }
        }
    } // config file exists, parse and get settings

    std::filesystem::path log_dir_path( rena::get_home_path() );
    log_dir_path /= ".hashup";
    if ( !std::filesystem::exists( log_dir_path ) )
    {
        std::filesystem::create_directories( log_dir_path );
    }

    RENALOG_INIT(
        log_dir_path ,
        CPTEXT( "hashup" ) ,
        rena::CFG_MAX_OLD_LOG_FILES ,
        rena::parse_str_to_severity( rena::CFG_MIN_LOG_SEVERITY )
    );

    LOG( INFO , master ,
        "Version -> " << "HashUp " << HASHUP_VERSION
                      << " (branch/" << BUILD_GIT_BRANCH
                      << ":" << BUILD_GIT_COMMIT
                      << ", " << BUILD_TIME
                      << ") [" << CXX_COMPILER_ID
                      << " " << CXX_COMPILER_VERSION
                      << "] on " << BUILD_SYS_NAME
    );

    if ( !( rena::is_supported_hash_mode( rena::CFG_MODE ) && in_range( rena::CFG_THREAD , 1 , 128 ) ) )
    {
        LOG( WARNING , master ,
            "Found illegal config values under key \"default\": mode -> \"" << rena::CFG_MODE << "\" thread -> " << rena::CFG_THREAD
        );
        CPERR << rena::rich::FColor::RED << "Illegal config values." << rena::rich::style_reset << std::endl
              << "Continue with default settings." << std::endl;
        rena::CFG_MODE = "md5";
        rena::CFG_THREAD = 8;
    } // illegal configs

    // if there's no config file, just skip and use default settings

    LOG( INFO , master , 
        "CFG -> MODE: " << rena::CFG_MODE << " THREAD: " << rena::CFG_THREAD << " MIN_LOG_SEVERITY: " << rena::CFG_MIN_LOG_SEVERITY << " MAX_OLD_LOG_FILES: " << rena::CFG_MAX_OLD_LOG_FILES
    );

#pragma region create_cmd_parser

    cmdline::parser cmdparser;
//               ------Type------ ----arg---- -abbr- ----------------------------------------------------describe----------------------------------------------------- --nes-- -----default------ ------------------------possible option------------------------
    cmdparser.add                ( "help"    , '?'  , "Show this help page" );
    cmdparser.add                ( "create"  , 'w'  , "Create a hash list for a directory" );
    cmdparser.add                ( "overlay" , '\0' , "Overlay old hash list without asking" );
    cmdparser.add                ( "check"   , 'r'  , "Do hash check for a directory" );
    cmdparser.add<std::string>   ( "file"    , 'f'  , "Set the path of the hash list"                                                                                 , true  , "" );
    cmdparser.add                ( "single"  , 's'  , "Use single file mode" );
    cmdparser.add<std::string>   ( "hash"    , '\0' , "File hash (only available by single file check)"                                                               , false );
#ifdef USE_OPENSSL_EVP
    cmdparser.add<std::string>   ( "mode"    , 'm'  , "Set hash mode (md5, sha1, sha224, sha256, sha512, sha3-224, sha3-256, sha3-384, sha3-512, shake128, shake256)" , false , rena::CFG_MODE   , cmdline::oneof<std::string>( rena::support_hash_modes ) );
#else
    cmdparser.add<std::string>   ( "mode"    , 'm'  , "Set hash mode (md5, sha1, sha224, sha256, sha512)"                                                             , false , rena::CFG_MODE   , cmdline::oneof<std::string>( rena::support_hash_modes ) );
#endif
    cmdparser.add<std::string>   ( "ignore"  , 'i'  , "Set the path of the ignore file"                                                                               , false );
    cmdparser.add<unsigned short>( "thread"  , 'j'  , "Set the thread-number of multithreading acceleration"                                                          , false , rena::CFG_THREAD , cmdline::range<unsigned short>( 1 , 128 ) );
    cmdparser.add                ( "version" , 'v'  , "Show HashUp version" );
    cmdparser.set_program_name( "hashup" );

#pragma endregion create_cmd_parser

    bool pret = cmdparser.parse( argc , argv );
    if ( !pret )
    {
        if ( !cmdparser.exist( "help" ) && !cmdparser.exist( "version" ) )
        {
            LOG( FATAL , master ,
                "Argument error -> " << cmdparser.error()
            );
            CPOUT << rena::rich::FColor::RED << CPATOWCONV( cmdparser.error() ) << rena::rich::style_reset << std::endl
                  << CPATOWCONV( cmdparser.usage() ) << std::endl;
            FREE_ARGV;
            RENALOG_FREE();
            return 128;
        } // show help
        else if ( cmdparser.exist( "help" ) )
        {
            CPOUT << CPATOWCONV( cmdparser.usage() ) << std::endl;
            FREE_ARGV;
            RENALOG_FREE();
            return 0;
        } // show help page
        else
        {
            CPOUT << "HashUp " << HASHUP_VERSION << " (branch/" << BUILD_GIT_BRANCH << ":" << BUILD_GIT_COMMIT << ", " << BUILD_TIME << ") [" << CXX_COMPILER_ID << " " << CXX_COMPILER_VERSION << "] on " << BUILD_SYS_NAME << std::endl;
            FREE_ARGV;
            RENALOG_FREE();
            return 0;
        } // show version
    } // arg error

    rena::HASHPURPOSE p = rena::HASHPURPOSE::NOSET;
    if ( cmdparser.exist( "create" ) && cmdparser.exist( "check" ) )
    {
        LOG( FATAL , master ,
            "Argument error -> do hash check and create at the same time"
        );
        CPERR << rena::rich::FColor::RED << "Cannot create hash list and do hash check at the same time, exit." << rena::rich::style_reset << std::endl;
        FREE_ARGV;
        RENALOG_FREE();
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
        LOG( INFO , master ,
            "Mode -> single file"
        );
        if ( p != rena::HASHPURPOSE::NOSET )
        {
            CPOUT << rena::rich::FColor::YELLOW << "Using '-r, --check' or '-w, --create' args by single file mode is not necessary." << rena::rich::style_reset << std::endl
                  << "Continue." << std::endl;
        }
        if ( cmdparser.exist( "hash" ) )
        {
            LOG( INFO , master ,
                "Mode -> hash check"
            );
            p = rena::HASHPURPOSE::CHECK;
        }
        else
        {
            LOG( INFO , master ,
                "Mode -> hash create"
            );
            p = rena::HASHPURPOSE::CREATE;
        }

        std::filesystem::path fp( CPATOWCONV( cmdparser.get<std::string>( "file" ) ) );
        CPSTR hash;
        std::string mode = cmdparser.get<std::string>( "mode" );
        LOG( INFO , master ,
            "Hash Mode -> " << mode
        );
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
            LOG( ERROR , master ,
                "Result -> error. file: \"" << CPWTOACONV( CPPATHTOSTR( fp ) ) << "\" what: " << e.what()
            );
            CPERR << rena::rich::FColor::RED << "Operate file \"" << CPPATHTOSTR( fp ) << "\" failed: " << rena::rich::style_reset << e.what() << std::endl
                  << "Exit." << std::endl;
            FREE_ARGV;
            RENALOG_FREE();
            return 128;
        }

        if ( rena::quit_signal.load() )
        {
            CPOUT << "Stop." << std::endl;
            FREE_ARGV;
            RENALOG_FREE();
            return 0;
        } // called quit
        
        if ( p == rena::HASHPURPOSE::CREATE )
        {
            LOG( INFO , master ,
                "Result -> passed. file: \"" << CPWTOACONV( CPPATHTOSTR( fp ) ) << "\" hash calced: " << CPWTOACONV( hash )
            );
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
                LOG( INFO , master ,
                    "Result -> passed. file: \"" << CPWTOACONV( CPPATHTOSTR( fp ) ) << "\" hash got: " << CPWTOACONV( hash_get ) << " hash calced: " << CPWTOACONV( hash )
                )
                CPOUT << rena::rich::FColor::GREEN << "Passed." << rena::rich::style_reset << std::endl;
            }
            else
            {
                LOG( INFO , master ,
                    "Result -> failed. file: \"" << CPWTOACONV( CPPATHTOSTR( fp ) ) << "\" hash got: " << CPWTOACONV( hash_get ) << " hash calced: " << CPWTOACONV( hash )
                )
                CPOUT << rena::rich::FColor::RED << "Failed: " << rena::rich::style_reset << "got " << hash_get << ", should be " << hash << "." << std::endl;
            }
        }
        FREE_ARGV;
        RENALOG_FREE();
        return 0;
    } // single file mode

    LOG( INFO , master ,
        "Mode -> basic hash list"
    );

    rena::HUFO hufo;
    rena::HUFO::HUFOSTATUS open_status;

    std::filesystem::path huf_path_get = CPATOWCONV( cmdparser.get<std::string>( "file" ) );
    if ( huf_path_get.is_relative() )
    {
        huf_path_get = std::filesystem::current_path() / huf_path_get;
    } // relative huf path
    LOG( INFO , master ,
        "HUF -> path: \"" << CPWTOACONV( CPPATHTOSTR( huf_path_get ) ) << "\""
    );

    if ( cmdparser.exist( "ignore" ) )
    {
        LOG( INFO , master ,
            "Using file ignore function"
        );
        std::filesystem::path fig_path_get = CPATOWCONV( cmdparser.get<std::string>( "ignore" ) );
        if ( fig_path_get.is_relative() )
        {
            fig_path_get = std::filesystem::current_path() / fig_path_get;
        }
        LOG( INFO , master ,
            "IGF -> path: \"" << CPWTOACONV( CPPATHTOSTR( huf_path_get ) ) << "\""
        );
        open_status = hufo.open( huf_path_get , p , cmdparser.exist( "overlay" ) ? true : false , fig_path_get );
    }
    else
    {
        open_status = hufo.open( huf_path_get , p , cmdparser.exist( "overlay" ) ? true : false );
    }
    if ( open_status != rena::HUFO::HUFOSTATUS::OK )
    {
        LOG( ERROR , master ,
            "Result -> open HUF failed: HUFO.open() exit with " << static_cast<int>( open_status )
        );
        print_hufo_err( open_status );
        FREE_ARGV;
        RENALOG_FREE();
        return open_status;
    } // open failed

    if ( cmdparser.exist( "mode" ) || p == rena::HASHPURPOSE::CREATE )
    {
        std::string mode = cmdparser.get<std::string>( "mode" );
        LOG( INFO , master ,
            "Hash Mode -> " << mode
        );
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

    LOG( INFO , master ,
        "Start hash calculation process"
    );
    rena::HUFO::HUFOSTATUS do_operate_status = hufo.start( cmdparser.get<unsigned short>( "thread" ) );
    if ( do_operate_status != rena::HUFO::HUFOSTATUS::OK &&
         do_operate_status != rena::HUFO::HUFOSTATUS::HASCHECKFAILEDF &&
         do_operate_status != rena::HUFO::HUFOSTATUS::CALLQUIT )
    {
        LOG( ERROR , master ,
            "Result -> do operate failed: HUFO.start() exit with " << static_cast<int>( do_operate_status )
        );
        print_hufo_err( do_operate_status );
        FREE_ARGV;
        RENALOG_FREE();
        return do_operate_status;
    }
    else if ( do_operate_status == rena::HUFO::HUFOSTATUS::CALLQUIT )
    {
        LOG( INFO , master ,
            "Result -> global stop called (SIGINT / KBS_Q)"
        );
        CPOUT << "Stop." << std::endl;
    }
    else
    {
        LOG( INFO , master ,
            "Result -> work done and success"
        );
        CPOUT << "Work done and success." << std::endl;
    }

    FREE_ARGV;
    RENALOG_FREE();
    return 0;
}

void print_hufo_err( rena::HUFO::HUFOSTATUS tag ){
    CPERR << rena::rich::FColor::RED;
    switch ( tag ){
        case rena::HUFO::HUFOSTATUS::OPENFILEERR:
            CPERR << "Open target hash list failed, exit." << std::endl; break;
        case rena::HUFO::HUFOSTATUS::OPENIGFERR:
            CPERR << "Open ignore file failed, exit." << std::endl; break;
        case rena::HUFO::HUFOSTATUS::PARSEIGFERR:
            CPERR << "Failed to parse ignore file, exit." << std::endl; break;
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
    CPERR << rena::rich::style_reset;
    return;
}