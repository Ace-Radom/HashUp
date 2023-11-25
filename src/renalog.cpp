#include"renalog.h"

rena::renalog* rena::__global_logger__ = nullptr;

rena::renalog::RENALOGSTATUS rena::renalog::init(){
    if ( this -> _nametag == CPTEXT( "" ) )
    {
        this -> _nametag = CPTEXT( "renalog" );
    }
    if ( this -> _old_log_file_max_num == static_cast<unsigned short>( -1 ) )
    {
        this -> _old_log_file_max_num = 16;
    }
    // nametag or old_log_file_max_num not set
    // nametag default "renalog", old_log_file_max_num default 16

    for ( int i = this -> _old_log_file_max_num - 1 ; i >= 0 ; i-- )
    {
        std::filesystem::path this_logfile = this -> _logdir / std::filesystem::path( this -> _nametag + CPTEXT( "." ) + CPTOSTR( i ) + CPTEXT( ".log" ) );
        if ( std::filesystem::exists( this_logfile ) )
        {
            std::filesystem::path older_logfile = this -> _logdir / std::filesystem::path( this -> _nametag + CPTEXT( "." ) + CPTOSTR( i + 1 ) + CPTEXT( ".log" ) );
            try {
                std::filesystem::copy_file( this_logfile , older_logfile , std::filesystem::copy_options::overwrite_existing );
            }
            catch ( const std::exception& e )
            {
                CPERR << rich::FColor::RED << "Init logger error: " << rich::style_reset << e.what() << std::endl;
                return RENALOGSTATUS::INITERR;
            }
        } // this old log file still exists
    } // copy old log files

    std::filesystem::path logfile_path = this -> _logdir / std::filesystem::path( this -> _nametag + CPTEXT( ".0.log" ) );
    // log file path in this run
    this -> _rwF.open( logfile_path , std::ios::in | std::ios::out | std::ios::trunc );
    if ( !( this -> _rwF.is_open() ) )
    {
        CPERR << rich::FColor::RED << "Open target log file error: " << rich::style_reset << strerror( errno ) << std::endl;
        return RENALOGSTATUS::INITERR;
    }

    return RENALOGSTATUS::OK;
}

void rena::renalog::dump_logline_begin( rena::renalog::RENALOGSEVERITY severity , std::string host ){
    this -> dump_logline_begin( severity , host , std::chrono::system_clock::now() );
    return;
}

void rena::renalog::dump_logline_begin( rena::renalog::RENALOGSEVERITY severity , std::string host , const std::chrono::system_clock::time_point& tp ){
    time_t time_t_now = std::chrono::system_clock::to_time_t( tp );
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>( tp.time_since_epoch() ) % 1000; // current time ms part
    char timestr[128];
    strftime( timestr , sizeof( timestr ) , "%Y-%m-%d %H:%M:%S" , localtime( &time_t_now ) );
    this -> _rwF << "[" << timestr << "." << std::setw( 3 ) << std::setfill( '0' ) << std::right << ms.count() << "] " << std::setw( 10 ) << std::setfill( ' ' ) << std::left << host + ":" << severity << "\t";
    return;
}

rena::renalog::RENALOGSTATUS rena::renalog::copy_current_log_for_fatal(){
    this -> flush();
    time_t time_t_now = std::chrono::system_clock::to_time_t( std::chrono::system_clock::now() );
    char timestr[128];
    strftime( timestr , sizeof( timestr ) , "%Y-%m-%d.%H.%M.%S" , localtime( &time_t_now ) );

    std::filesystem::path logfile_copy_path = this -> _logdir / std::filesystem::path( this -> _nametag + CPTEXT( ".FATAL." ) + CPATOWCONV( timestr ) + CPTEXT( ".log" ) );
    std::ofstream logfile_copy( logfile_copy_path );
    if ( !logfile_copy.is_open() )
    {
        CPERR << rich::FColor::RED << "Open target log file copy error: " << rich::style_reset << strerror( errno ) << std::endl;
        return RENALOGSTATUS::FCFIERR;
    }

    logfile_copy << "/**\n"
                 << " * This is a copy of the original log file, because fatal error occured during the last run.\n"
                 << " * In order to make sure the log datas will not be overwritten, renalog created this copy automatically.\n"
                 << " * All log datas before ending-sign from loghost will be copied.\n"
                 << "*/\n\n"
                 << "// Begin of copy\n\n";

    auto orippos = this -> _rwF.tellp(); // original put pos
    this -> _rwF.seekg( 0 , std::ios::beg );
    std::string buf;
    while ( std::getline( this -> _rwF , buf ) )
    {
        logfile_copy << buf << "\n";
    }
    logfile_copy << "\n// End of copy\n";
    logfile_copy.close();
    CPERR << "Details have been wriiten into log file and a copy \"" << CPPATHTOSTR( logfile_copy_path ) << "\"" << std::endl;
    this -> _rwF.clear();
    this -> _rwF.seekp( orippos , std::ios::beg );
    // reset _rwF write status
    return RENALOGSTATUS::OK;
}

void rena::renalog::push( rena::renalog::RENALOGSEVERITY severity , const char* host , std::chrono::system_clock::time_point log_tp , std::string msg ){
    this -> _tp.enqueue( [this,severity,host,log_tp,msg](){
        this -> dump_logline_begin( severity , host , log_tp );
        this -> _rwF << msg << "\n";
    } );
    return;
}

rena::renalog::RENALOGSEVERITY rena::parse_str_to_severity( const std::string& str ){
    if      ( str == "DEBUG" )   return renalog::RENALOGSEVERITY::DEBUG;
    else if ( str == "INFO" )    return renalog::RENALOGSEVERITY::INFO;
    else if ( str == "WARNING" ) return renalog::RENALOGSEVERITY::WARNING;
    else if ( str == "ERROR" )   return renalog::RENALOGSEVERITY::ERROR;
    else if ( str == "FATAL" )   return renalog::RENALOGSEVERITY::FATAL;
    else if ( str == "UNKNOWN" ) return renalog::RENALOGSEVERITY::UNKNOWN;
    else                         return renalog::RENALOGSEVERITY::INFO;
    // cannot recognize, return default INFO
}
