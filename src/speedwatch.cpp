#include"hashup.h"

#ifdef SHOW_PROGRESS_DETAIL

rena::speedwatcher* rena::global_speed_watcher = nullptr;

void rena::speedwatcher::add( size_t size ){
    std::lock_guard<std::mutex> lock( this -> global_mutex );
    this -> total_size += size;
    return;
}

void rena::speedwatcher::start_one( std::thread::id thread_id , std::filesystem::path path ){
    std::lock_guard<std::mutex> lock( this -> global_mutex );
    this -> file_in_process[thread_id] = path;
    return;
}

void rena::speedwatcher::finished_one( std::thread::id thread_id ){
    std::lock_guard<std::mutex> lock( this -> global_mutex );
    this -> file_in_process.erase( thread_id );
    this -> finished_num++;
    return;
}

/**
 * @return how many bytes is processed per secounds
*/
size_t rena::speedwatcher::get_speed(){
    std::lock_guard<std::mutex> lock( this -> global_mutex );
    this -> push_timekeeping_period();
    if ( this -> total_duration.count() == 0 )
    {
        return 0;
    }
    return this -> total_size / this -> total_duration.count() * 1000000;
}

size_t rena::speedwatcher::get_finished(){
    std::lock_guard<std::mutex> lock( this -> global_mutex );
    return this -> finished_num;
}

CPSTR rena::speedwatcher::get_expected_time_left( const HUFO* hufoobj ){
    std::lock_guard<std::mutex> lock( this -> global_mutex );
    this -> push_timekeeping_period();
    if ( this -> total_duration.count() == 0 )
    {
        return CPTEXT( "unlimited" );
    }
    size_t speed_per_second = this -> total_size / this -> total_duration.count() * 1000000;
    double excepted_time_left_second = ( double ) ( hufoobj -> _tfsize - this -> total_size ) / speed_per_second;
    std::ostringstream oss;
    if ( excepted_time_left_second > 60 )
    {
        oss << ( int ) excepted_time_left_second / 60 << "min ";
    }
    oss << std::fixed << std::setprecision( 2 ) << fmod( excepted_time_left_second , 60.0 ) << "s";
    return CPATOWCONV( oss.str() );
}

double rena::speedwatcher::get_duration_s(){
    std::lock_guard<std::mutex> lock( this -> global_mutex );
    return this -> total_duration.count() / 1000000.0;
}

std::vector<std::filesystem::path> rena::speedwatcher::get_files_in_process(){
    std::lock_guard<std::mutex> lock( this -> global_mutex );
    std::vector<std::filesystem::path> ret;
    for ( const auto& it : this -> file_in_process )
    {
        ret.push_back( it.second );
    }
    return ret;
}

void rena::speedwatcher::pause_watch(){
    std::lock_guard<std::mutex> lock( this -> global_mutex );
    this -> push_timekeeping_period();
    return;
}

void rena::speedwatcher::resume_watch(){
    std::lock_guard<std::mutex> lock( this -> global_mutex );
    this -> start_time = std::chrono::steady_clock::now();
    return;
}

void rena::speedwatcher::push_timekeeping_period(){
    // someone calls this function means mutex has already been locked
    auto time_now = std::chrono::steady_clock::now();
    this -> total_duration += std::chrono::duration_cast<std::chrono::microseconds>( time_now - this -> start_time );
    this -> start_time = std::chrono::steady_clock::now();
    return;
}

#endif