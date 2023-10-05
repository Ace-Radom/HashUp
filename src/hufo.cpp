#include"hashup.h"

rena::HUFO::~HUFO(){
    if ( this -> _rwF.is_open() )
    {
        this -> _rwF.close();
    }
    return;
}

rena::HUFO::HUFOSTATUS rena::HUFO::open( const std::string& path , rena::HASHPURPOSE p  ){
    this -> _path = path;
    this -> _hpurpose = p;
    if ( std::filesystem::exists( this -> _path ) && std::filesystem::is_regular_file( this -> _path ) )
    {
        this -> _rwF.open( this -> _path , std::ios::in | std::ios::out );
        if ( !( this -> _rwF.is_open() ) )
        {
            return HUFOSTATUS::OPENFILEERR;
        } // open failed
    } // file exists, and also a regular file, open it without caring about purpose
    else
    {
        if ( std::filesystem::is_directory( this -> _path ) )
        {
            return HUFOSTATUS::DIRASFILE;
        } // a dir given
        else if ( this -> _hpurpose == HASHPURPOSE::CHECK )
        {
            return HUFOSTATUS::FILENOTEXIST;
        } // doing hash check, but file doesn't exist
        else
        {
            this -> _rwF.open( this -> _path , std::ios::in | std::ios::out | std::ios::trunc );
            if ( !( this -> _rwF.is_open() ) )
            {
                return HUFOSTATUS::OPENFILEERR;
            } // open failed
        } // doing hash create, and file doesn't exist, create it
    } // file doesn't exist
    return HUFOSTATUS::OK;
}

void rena::HUFO::set_mode( rena::HASHMODE mode ){
    this -> _hmode = mode;
    return;
}

void rena::HUFO::set_purpose( rena::HASHPURPOSE purpose ){
    this -> _hpurpose = purpose;
    return;
}