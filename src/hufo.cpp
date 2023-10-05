#include"hashup.h"

rena::HUFO::~HUFO(){
    if ( this -> _rwF.is_open() )
    {
        this -> _rwF.close();
    }
    return;
}

rena::HUFO::HUFOSTATUS rena::HUFO::open( const std::string& path , rena::HASHPURPOSE p ){
    this -> _hpurpose = p;
    if ( this -> _hpurpose == HASHPURPOSE::CHECK )
    {
        if ( !std::filesystem::exists( path ) )
        {
            return HUFOSTATUS::FILENOTEXIST;
        } // .hpf list doesn't exist
        else if ( !std::filesystem::is_regular_file( path ) )
        {
            return HUFOSTATUS::OPENFILEERR;
        }
        this -> _dpath = std::filesystem::path( path ).parent_path().string();
        this -> _rwF.open( path , std::ios::in | std::ios::out );
        if ( !( this -> _rwF.is_open() ) )
        {
            this -> _dpath.clear();
            return HUFOSTATUS::OPENFILEERR;
        } // open failed
    } // doing hash check
    else if ( this -> _hpurpose == HASHPURPOSE::CREATE )
    {
        if ( std::filesystem::exists( path ) )
        {
            if ( !confirm_interrupt( "This file \"" + path + "\" already exist. Are you sure to overwrite it?" , 'y' , 'N' ) )
            {
                return HUFOSTATUS::INTERRUPT;
            }
        } // given file already exists
        this -> _dpath = std::filesystem::path( path ).parent_path().string();
        this -> _rwF.open( path , std::ios::in | std::ios::out | std::ios::trunc );
        if ( !( this -> _rwF.is_open() ) )
        {
            this -> _dpath.clear();
            return HUFOSTATUS::OPENFILEERR;
        } // open failed
    } // doing hash create
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