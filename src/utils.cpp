#include"utils.h"

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