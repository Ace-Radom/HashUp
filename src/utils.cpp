#include"utils.h"

template <typename _T>
bool rena::is_future_ready( const std::future<_T>& F){
    return F.wait_for( std::chrono::seconds( 0 ) ) == std::future_status::ready;
}

bool rena::confirm_interrupt( const std::string& msg , char y , char n ){

start_confirm:

    std::cout << msg << " (" << y << "/" << n << "): ";
    char c;
    std::cin >> c;
    if ( tolower( c ) == tolower( y ) )
    {
        return true;
    }
    else if ( tolower( c ) == tolower( n ) )
    {
        return false;
    }
    else
    {
        goto start_confirm;
    }
}