#include"renalog.h"

void rena::renalog::set_logdir( const std::filesystem::path& logdir ) noexcept {
    this -> _logdir = logdir;
    return;
}

void rena::renalog::set_nametag( CPSTR nametag ) noexcept {
    this -> _nametag = nametag;
    return;
}

void rena::renalog::set_old_log_file_max_num( unsigned short old_log_file_max_num ) noexcept {
    this -> _old_log_file_max_num = old_log_file_max_num;
    return;
}

void rena::renalog::set_min_severity( rena::renalog::RENALOGSEVERITY severity ) noexcept {
    this -> _min_severity = severity;
    return;
}

bool rena::renalog::is_severity_need_to_log( rena::renalog::RENALOGSEVERITY severity ) const noexcept {
    if ( severity >= this -> _min_severity )
    {
        return true;
    }
    return false;
}