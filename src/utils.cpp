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

CPSTR rena::get_hashup_exe_path(){
    CPSTR hashup_exe_path;
    CPCHAR buf[1024];
    memset( buf , 0 , sizeof( buf ) );

#ifdef _WIN32
    GetModuleFileName( NULL , buf , sizeof( buf ) );
    hashup_exe_path = buf;
#elif defined( __linux__ )
    
#elif defined( __APPLE__ )

#endif

    return hashup_exe_path;
}

std::string rena::CFG_MODE = "md5";
unsigned short rena::CFG_THREAD = 8;
