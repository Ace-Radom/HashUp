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
        CPERR << rich::FColor::RED << "Open target log file error." << rich::style_reset << std::endl;
        return RENALOGSTATUS::INITERR;
    }

    return RENALOGSTATUS::OK;
}

void rena::renalog::dump_logline_begin( rena::renalog::RENALOGSEVERITY severity , std::string host ){
    auto time_now = std::chrono::system_clock::now();
    time_t time_t_now = std::chrono::system_clock::to_time_t( time_now );
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>( time_now.time_since_epoch() ) % 1000; // current time ms part
    char timestr[128];
    strftime( timestr , sizeof( timestr ) , "%Y-%m-%d %H:%M:%S" , localtime( &time_t_now ) );
    this -> _rwF << "[" << timestr << "." << std::setw( 3 ) << std::setfill( '0' ) << ms.count() << "] " << host << ": " << severity << "\t";
    return;
}

std::ostream& rena::operator<<( std::ostream& os , rena::renalog::RENALOGSEVERITY severity ){
    switch ( severity )
    {
        case renalog::RENALOGSEVERITY::INFO:    os << "INFO";    break;
        case renalog::RENALOGSEVERITY::WARNING: os << "WARNING"; break;
        case renalog::RENALOGSEVERITY::ERROR:   os << "ERROR";   break;
        case renalog::RENALOGSEVERITY::FATAL:   os << "FATAL";   break;
        case renalog::RENALOGSEVERITY::UNKNOWN: os << "UNKNOWN"; break;
    }
    return os;
}