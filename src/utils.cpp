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