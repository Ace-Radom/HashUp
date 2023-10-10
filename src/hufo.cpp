#include"hashup.h"

rena::HUFO::~HUFO(){
    if ( this -> _rwF.is_open() )
    {
        this -> _rwF.close();
    }
    return;
}

rena::HUFO::HUFOSTATUS rena::HUFO::open( const CPSTR& path , rena::HASHPURPOSE p ){
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
#ifdef _MSC_VER
            if ( !confirm_interrupt( L"This file \"" + path + L"\" already exist. Are you sure to overwrite it?" , 'y' , 'N' ) )
#else
            if ( !confirm_interrupt( "This file \"" + path + "\" already exist. Are you sure to overwrite it?" , 'y' , 'N' ) )
#endif
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
    this -> _hufopath = path;
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

rena::HUFO::HUFOSTATUS rena::HUFO::do_create( unsigned short threads ){
    if ( this -> _pdpath.empty() )
    {
        return HUFOSTATUS::NOWORKINGDIR;
    } // create root dir (_pdpath) not set
    if ( !( this -> _rwF.is_open() ) )
    {
        return HUFOSTATUS::HUFNOTOPEN;
    } // _rwF isn't open
    if ( _hf == nullptr )
    {
        this -> set_mode( HASHMODE::MD5 );
    } // hash mode not set, use default (MD5)

    this -> _traversal_dir_write_to_hlist( this -> _pdpath );
    this -> _do_hashcalc( threads );

    for ( auto it : this -> _hlist )
    {
        DEBUG_MSG( it.fp << " " << it.hash );
        this -> _rwF << CPSTRTOFCONV( CPPATHTOSTR( "." / it.fp ) ) << " " << CPSTRTOFCONV( it.hash ) << std::endl;
    }
    return HUFOSTATUS::OK;
}

rena::HUFO::HUFOSTATUS rena::HUFO::_do_hashcalc( unsigned short threads ){
    ThreadPool pool( threads );

    for ( auto it = this -> _hlist.begin() ; it != this -> _hlist.end() ; )
    {
        std::filesystem::path ap = this -> _pdpath / it -> fp; // absolute path
        it -> hash_future = pool.enqueue( this -> _hf , ap ).share();
        ++it;
    } // iterate _hlist, start tasks

    for ( auto it = this -> _hlist.begin() ; it != this -> _hlist.end() ; )
    {
        DEBUG_MSG( "Wait for " << it -> fp );
        it -> hash_future.wait();
        try {
            it -> hash = it -> hash_future.get();
        }
        catch ( const std::exception& e )
        {
            CPERR << "Operate file " << this -> _pdpath / it -> fp << " failed: " << e.what() << std::endl;
            CPERR << "Skip." << std::endl;
            it = this -> _hlist.erase( it );
            continue;
        } // open file failed when calculating hash of this file
        ++it;
    }

    return HUFOSTATUS::OK;
}

void rena::HUFO::_traversal_dir_write_to_hlist( const std::filesystem::path& dir ){
    for ( auto it : std::filesystem::directory_iterator( dir ) )
    {
        auto fp = it.path(); // file path
        if ( std::filesystem::is_directory( fp ) )
        {
            _traversal_dir_write_to_hlist( fp );
        } // dir found, traversal it
        else
        {
            if ( fp == this -> _hufopath )
            {
                continue;
            } // ignore HUFO itself
            HASHOBJ temp;
            temp.fp = std::filesystem::relative( fp , this -> _pdpath );
            this -> _hlist.push_back( temp );
        } // write relative path to _hlist
    }
    return;
}

void rena::HUFO::test(){

    DEBUG_MSG( this -> open( "/mnt/d/BaiduNetdiskDownload/1/1/LilPy102-pc/LilPy102-pc/game/gui/sound/test.huf" , HASHPURPOSE::CREATE ) );
    DEBUG_MSG( this -> do_create( 8 ) );
 

    return;
}