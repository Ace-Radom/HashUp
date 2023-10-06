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
        this -> _pdpath = std::filesystem::path( path ).parent_path();
        this -> _rwF.open( path , std::ios::in | std::ios::out );
        if ( !( this -> _rwF.is_open() ) )
        {
            this -> _pdpath.clear();
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
        this -> _pdpath = std::filesystem::path( path ).parent_path();
        this -> _rwF.open( path , std::ios::in | std::ios::out | std::ios::trunc );
        if ( !( this -> _rwF.is_open() ) )
        {
            this -> _pdpath.clear();
            return HUFOSTATUS::OPENFILEERR;
        } // open failed
    } // doing hash create
    return HUFOSTATUS::OK;
}

void rena::HUFO::set_mode( rena::HASHMODE mode ){
    this -> _hmode = mode;
    switch ( this -> _hmode )
    {
        case MD5:    this -> _hf = calc_file_md5;    break;
        case SHA1:   this -> _hf = calc_file_sha1;   break;
        case SHA256: this -> _hf = calc_file_sha256; break;
        case SHA512: this -> _hf = calc_file_sha512; break;
    }
    return;
}

void rena::HUFO::set_purpose( rena::HASHPURPOSE purpose ){
    this -> _hpurpose = purpose;
    return;
}

rena::HUFO::HUFOSTATUS rena::HUFO::do_hashcalc( unsigned short threads ){
    ThreadPool pool( threads );

    for ( auto it = this -> _hlist.begin() ; it != this -> _hlist.end() ; )
    {
        std::filesystem::path ap = this -> _pdpath / it -> fp; // absolute path
        FILE* f = nullptr;
        f = fopen( ap.string().c_str() , "rb" );
        if ( f == nullptr )
        {
            std::cerr << "Opening file " << ap << " failed, skip.";
            this -> _hlist.erase( it );
            continue;
        } // open file failed
        it -> hash_future = pool.enqueue( this -> _hf , f ).share();
        ++it;
    } // iterate _hlist, start tasks

    for ( auto it = this -> _hlist.begin() ; it != this -> _hlist.end() ; ++it )
    {
        DEBUG_MSG( "Wait for " << it -> fp );
        it -> hash_future.wait();
        it -> hash = it -> hash_future.get();
    }

    return HUFOSTATUS::OK;
}

void rena::HUFO::_traversal_dir_write_to_hlist( const std::filesystem::path& dir ){
    for ( auto it : std::filesystem::directory_iterator( dir ) )
    {
        try {
            auto fp = it.path(); // file path
            fp.string();
            // the change from filesystem::path -> string sometimes leads to an error because of character mapping.
            // try to change it once (and don't store the out-come string) and let the program to catch if this error occurs.
            // catch it here because later it's really hard and complex to get the first-error-point.
            if ( std::filesystem::is_directory( fp ) )
            {
                _traversal_dir_write_to_hlist( fp );
            } // dir found, traversal it
            else
            {
                HASHOBJ temp;
                temp.fp = std::filesystem::relative( fp , this -> _pdpath );
                this -> _hlist.push_back( temp );
            } // write relative path to _hlist
        }
        catch ( const std::exception& e )
        {
            std::cerr << "Error occured when traversaling directory \"" << dir.string() << "\":" << std::endl;
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
