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
    this -> _rwF.open( logfile_path , std::ios::out | std::ios::trunc );
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

void rena::renalog::dump_logline_begin( RENALOGSEVERITY severity , std::string host , const std::chrono::system_clock::time_point& tp ){
    time_t time_t_now = std::chrono::system_clock::to_time_t( tp );
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>( tp.time_since_epoch() ) % 1000; // current time ms part
    char timestr[128];
    strftime( timestr , sizeof( timestr ) , "%Y-%m-%d %H:%M:%S" , localtime( &time_t_now ) );
    this -> _rwF << this -> _tp.get_task_in_queue_num() << "[" << timestr << "." << std::setw( 3 ) << std::setfill( '0' ) << std::right << ms.count() << "] " << std::setw( 10 ) << std::setfill( ' ' ) << std::left << host + ":" << severity << "\t";
    return;
}

std::ostream& rena::operator<<( std::ostream& os , rena::renalog::RENALOGSEVERITY severity ){
    switch ( severity )
    {
        case renalog::RENALOGSEVERITY::DEBUG:   os << "DEBUG";   break;
        case renalog::RENALOGSEVERITY::INFO:    os << "INFO";    break;
        case renalog::RENALOGSEVERITY::WARNING: os << "WARNING"; break;
        case renalog::RENALOGSEVERITY::ERROR:   os << "ERROR";   break;
        case renalog::RENALOGSEVERITY::FATAL:   os << "FATAL";   break;
        case renalog::RENALOGSEVERITY::UNKNOWN: os << "UNKNOWN"; break;
    }
    return os;
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
