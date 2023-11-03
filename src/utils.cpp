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

std::string rena::get_time_str_now(){
    auto now = std::chrono::system_clock::now();
    time_t current_time = std::chrono::system_clock::to_time_t( now );
    tm time_info = *localtime( &current_time );
    char time_str[256];
    strftime( time_str , sizeof( time_str ) , "%Y-%m-%d %H:%M:%S" , &time_info );
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>( now.time_since_epoch() ) % 1000;

    std::ostringstream ostr;
    ostr << time_str << "." << ms.count();
    return ostr.str();
}

void rena::noecho(){
#ifdef WIN32
    HANDLE hConsole = GetStdHandle( STD_OUTPUT_HANDLE );
    DWORD hmode;
    GetConsoleMode( hConsole , &hmode );
    hmode &= ~ENABLE_ECHO_INPUT;
    SetConsoleMode( hConsole , hmode );
#else
    struct termios attr;
    tcgetattr( STDIN_FILENO , &attr );
    attr.c_lflag &= ~ECHO;
    tcsetattr( STDIN_FILENO , TCSANOW , &attr );
#endif
    return;
}

void rena::echo(){
#ifdef WIN32
    HANDLE hConsole = GetStdHandle( STD_OUTPUT_HANDLE );
    DWORD hmode;
    GetConsoleMode( hConsole , &hmode );
    hmode |= ENABLE_ECHO_INPUT;
    SetConsoleMode( hConsole , hmode );
#else
    struct termios attr;
    tcgetattr( STDIN_FILENO , &attr );
    attr.c_lflag |= ECHO;
    tcsetattr( STDIN_FILENO , TCSANOW , &attr );
#endif
    return;
}

void rena::nocursor(){
#ifdef WIN32
    HANDLE hConsole = GetStdHandle( STD_OUTPUT_HANDLE );
    CONSOLE_CURSOR_INFO cinfo; // cursor info
    GetConsoleCursorInfo( hConsole , &cinfo );
    cinfo.bVisible = false;
    SetConsoleCursorInfo( hConsole , &cinfo );
#else
    CPOUT << "\033[?25l" << std::flush;
#endif
    return;
}

void rena::showcursor(){
#ifdef WIN32
    HANDLE hConsole = GetStdHandle( STD_OUTPUT_HANDLE );
    CONSOLE_CURSOR_INFO cinfo; // cursor info
    GetConsoleCursorInfo( hConsole , &cinfo );
    cinfo.bVisible = true;
    SetConsoleCursorInfo( hConsole , &cinfo );
#else
    CPOUT << "\033[?25h" << std::flush;
#endif
    return;
}

bool rena::is_supported_hash_mode( std::string mode ){
    for ( const auto& it : support_hash_modes )
    {
        if ( mode == it )
        {
            return true;
        }
    }
    return false;
}

CPSTR rena::get_hashup_exe_path(){
    CPSTR hashup_exe_path;
    CPCHAR buf[1024];
    memset( buf , 0 , sizeof( buf ) );

#ifdef _WIN32
    GetModuleFileName( NULL , buf , sizeof( buf ) );
    hashup_exe_path = buf;
#elif defined( __linux__ )
    ssize_t count = readlink( "/proc/self/exe" , buf , sizeof( buf ) );
    if ( count != -1 )
    {
        hashup_exe_path = buf;
    }
#elif defined( __APPLE__ )
    uint32_t bufsize = sizeof( buf );
    _NSGetExecutablePath( buf , &bufsize );
    hashup_exe_path = buf;
#endif

    return hashup_exe_path;
}

std::string rena::CFG_MODE = "md5";
unsigned short rena::CFG_THREAD = 8;
std::vector<std::string> rena::support_hash_modes = {
        "md5",
        "sha1",
        "sha224",
        "sha256",
        "sha384",
        "sha512"
#ifdef USE_OPENSSL_EVP
        ,
        "sha3-224",
        "sha3-256",
        "sha3-384",
        "sha3-512",
        "shake128",
        "shake256"
#endif
};