#include"hashup.h"

rena::HUFO::~HUFO(){
    if ( this -> _rwF.is_open() )
    {
        this -> _rwF.close();
    }
    return;
}

rena::HUFO::HUFOSTATUS rena::HUFO::open( const std::filesystem::path& path , rena::HASHPURPOSE p ){
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
            if ( !confirm_interrupt( L"This file \"" + CPPATHTOSTR( path ) + L"\" already exist. Are you sure to overwrite it?" , 'y' , 'N' ) )
#else
            if ( !confirm_interrupt( "This file \"" + CPPATHTOSTR( path ) + "\" already exist. Are you sure to overwrite it?" , 'y' , 'N' ) )
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

rena::HUFO::HUFOSTATUS rena::HUFO::start( unsigned short threads ){
    if ( this -> _hpurpose == HASHPURPOSE::CHECK )
    {
        return this -> do_check( threads );
    }
    else if ( this -> _hpurpose == HASHPURPOSE::CREATE )
    {
        return this -> do_create( threads );
    }
}

rena::HUFO::HUFOSTATUS rena::HUFO::do_create( unsigned short threads ){

#ifdef SHOW_PROGRESS_DETAIL
    CPOUT << "Doing hash list create." << std::endl;
#endif

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

    this -> _hlist.clear();
    this -> _traversal_dir_write_to_hlist( this -> _pdpath );

#ifdef SHOW_PROGRESS_DETAIL
    CPOUT << "Found " << this -> _hlist.size() << " files in total." << std::endl;
#endif

    this -> _do_hashcalc( threads );

#ifdef SHOW_PROGRESS_DETAIL
    CPOUT << "Write hash list to file." << std::endl;
#endif

    for ( auto it : this -> _hlist )
    {
        DEBUG_MSG( it.fp << " " << it.hash );
        this -> _rwF << CPWTOACONV( CPPATHTOSTR( "." / it.fp ) ) << " " << CPWTOACONV( it.hash ) << std::endl;
    }
    return HUFOSTATUS::OK;
}

rena::HUFO::HUFOSTATUS rena::HUFO::do_check( unsigned short threads ){

#ifdef SHOW_PROGRESS_DETAIL
    CPOUT << "Doing hash check." << std::endl;
#endif

    if ( this -> _pdpath.empty() )
    {
        return HUFOSTATUS::NOWORKINGDIR;
    } // check root dir (_pdpath) not set
    if ( !( this -> _rwF.is_open() ) )
    {
        return HUFOSTATUS::HUFNOTOPEN;
    } // _rwF isn't open
    if ( _hf == nullptr )
    {
        return HUFOSTATUS::HMODENOTSET;
    } // hash mode not set

    this -> _hlist.clear();
    this -> _errhlist.clear();

    this -> _read_huf_write_to_hlist();

#ifdef SHOW_PROGRESS_DETAIL
    CPOUT << "Found " << this -> _hlist.size() << " files in total." << std::endl;
#endif

    this -> _do_hashcalc( threads );

#ifdef SHOW_PROGRESS_DETAIL
    CPOUT << "Checking." << std::endl;
#endif

    for ( auto it : this -> _hlist )
    {
        if ( it.hash == it.hash_readin )
        {
            DEBUG_MSG( it.fp << " Checked" );
        }
        else
        {
            CPOUT << "File \"" << CPPATHTOSTR( it.fp ) << "\" Check Failed: " << it.hash_readin << " -> " << it.hash << std::endl;
            this -> _errhlist.push_back( it );
        }
    }
    return ( _errhlist.empty() ) ? HUFOSTATUS::OK : HUFOSTATUS::HASCHECKFAILEDF;
}

unsigned int rena::HUFO::get_errhlist_len(){
    return this -> _errhlist.size();
}

rena::HUFO::HUFOSTATUS rena::HUFO::_do_hashcalc( unsigned short threads ){
    ThreadPool pool( threads );

    for ( auto it = this -> _hlist.begin() ; it != this -> _hlist.end() ; )
    {
        std::filesystem::path ap = this -> _pdpath / it -> fp; // absolute path
        it -> hash_future = pool.enqueue( this -> _hf , ap ).share();
        ++it;
    } // iterate _hlist, start tasks

    unsigned long long file_waiting_now_index = 1;
    for ( auto it = this -> _hlist.begin() ; it != this -> _hlist.end() ; )
    {
        DEBUG_MSG( "Waiting for " << it -> fp );

#ifdef SHOW_PROGRESS_DETAIL
        CPOUT << "Progress: " << file_waiting_now_index << "/" << this -> _hlist.size() << "\r" << std::flush;
#endif

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
        file_waiting_now_index++;
    }

#ifdef SHOW_PROGRESS_DETAIL
    CPOUT << "\n";
#endif

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

void rena::HUFO::_read_huf_write_to_hlist(){
    std::string buf;
    while ( std::getline( this -> _rwF , buf ) )
    {
        HASHOBJ temp;
        temp.fp = CPATOWCONV( buf.substr( 0 , buf.rfind( ' ' ) ) );
        temp.hash_readin = CPATOWCONV( buf.substr( buf.rfind( ' ' ) + 1 ) );
        this -> _hlist.push_back( temp );
    }
    return;
}
