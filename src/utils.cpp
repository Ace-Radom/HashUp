#include"utils.h"

bool rena::confirm_interrupt( const CPSTR& msg , char y , char n ){

start_confirm:

    CPOUT << msg << " (" << y << "/" << n << "): ";
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