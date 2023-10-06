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

void rena::HUFO::_traversal_dir_write_to_hlist( const std::string& dir ){
    for ( auto it : std::filesystem::directory_iterator( dir ) )
    {
        try {
            auto fp = it.path(); // file path
            if ( std::filesystem::is_directory( fp ) )
            {
                _traversal_dir_write_to_hlist( fp.string() );
            } // dir found, traversal it
            else
            {
                HASHOBJ temp;
                temp.fp = std::filesystem::relative( fp , this -> _dpath ).string();
                this -> _hlist.push_back( temp );
            } // write relative path to _hlist
        }
        catch ( const std::exception& e )
        {
            std::cerr << "Error occured when traversaling directory \"" << dir << "\":" << std::endl;
            std::cerr << e.what() << std::endl;
            try {
                std::string path = it.path().string();
                std::cerr << "Error path: \"" << path << "\"." << std::endl;
            }
            catch ( ... )
            {
                std::cerr << "Error path output unavaliable." << std::endl;
            }
            std::cerr << "Skip the current " << ( it.is_directory() ? "directory." : "file." ) << std::endl;
        } // error occurs (mainly because the change from utf8 -> other charsets), skip
    }
    return;
}
