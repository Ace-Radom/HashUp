#include"hashup.h"

#ifdef SHOW_PROGRESS_DETAIL

rena::speedwatcher* rena::global_speed_watcher = nullptr;

void rena::speedwatcher::add( size_t size ){
    std::lock_guard<std::mutex> lock( this -> global_mutex );
    this -> total_size += size;
    return;
}

/**
 * @return how many bytes is processed per secounds
*/
size_t rena::speedwatcher::get_speed(){
    std::lock_guard<std::mutex> lock( this -> global_mutex );
    auto time_now = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>( time_now - this -> start_time );
    if ( duration.count() == 0 )
    {
        return 0;
    }
    return this -> total_size / duration.count() * 1000000;
}

#endif