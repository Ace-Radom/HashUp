#include"hashup.h"

rena::HUFO::~HUFO(){
    if ( this -> _rwF.is_open() )
    {
        this -> _rwF.close();
    }
    return;
}

/**
 * @param _ol_no_ask overlay no ask
*/
rena::HUFO::HUFOSTATUS rena::HUFO::open( const std::filesystem::path& path , rena::HASHPURPOSE p , bool _ol_no_ask ){
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
        if ( std::filesystem::exists( path ) && !_ol_no_ask )
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

rena::HUFO::HUFOSTATUS rena::HUFO::open( const std::filesystem::path& path , rena::HASHPURPOSE p , bool _ol_no_ask , const std::filesystem::path& ignore_file_path ){
    if ( this -> _figobj.open( ignore_file_path ) != 0 )
    {
        return HUFOSTATUS::OPENIGFERR;
    }
    if ( this -> _figobj.parse() != 0 )
    {
        return HUFOSTATUS::PARSEIGFERR;
    }
    this -> _using_fig = true;
    return this -> open( path , p , _ol_no_ask );
}

void rena::HUFO::set_mode( rena::HASHMODE mode ){
    this -> _hmode = mode;
    switch ( this -> _hmode )
    {
        case MD5:      this -> _hf = calc_file_md5;      this -> _hlen = 32;  break;
        case SHA1:     this -> _hf = calc_file_sha1;     this -> _hlen = 40;  break;
        case SHA224:   this -> _hf = calc_file_sha224;   this -> _hlen = 56;  break;
        case SHA256:   this -> _hf = calc_file_sha256;   this -> _hlen = 64;  break;
        case SHA384:   this -> _hf = calc_file_sha384;   this -> _hlen = 96;  break;
        case SHA512:   this -> _hf = calc_file_sha512;   this -> _hlen = 128; break;
#ifdef USE_OPENSSL_EVP
        case SHA3_224: this -> _hf = calc_file_sha3_224; this -> _hlen = 56;  break;
        case SHA3_256: this -> _hf = calc_file_sha3_256; this -> _hlen = 64;  break;
        case SHA3_384: this -> _hf = calc_file_sha3_384; this -> _hlen = 96;  break;
        case SHA3_512: this -> _hf = calc_file_sha3_512; this -> _hlen = 128; break;
        case SHAKE128: this -> _hf = calc_file_shake128; this -> _hlen = 32;  break;
        case SHAKE256: this -> _hf = calc_file_shake256; this -> _hlen = 64;  break;
#endif
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

    this -> _do_hashcalc( threads );

#ifdef SHOW_PROGRESS_DETAIL
    CPOUT << "Writing hash list to file." << std::endl;
#endif

    for ( auto it : this -> _hlist )
    {
        DEBUG_MSG( it.fp << " " << it.hash );
        this -> _rwF << CPWTOACONV( CPPATHTOSTR( "." / it.fp ) ) << " " << CPWTOACONV( it.hash ) << std::endl;
    }

#ifdef SHOW_PROGRESS_DETAIL
    CPOUT << rich::FColor::GREEN << "Create complete: " << rich::style_reset << this -> _ori_hlist_len << " files got in total; "
          << this -> _hlist.size() << " created; "
          << this -> _ori_hlist_len - this -> _hlist.size() << " error."
          << std::endl; 
#endif

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
            CPOUT << rich::FColor::RED << "File \"" << CPPATHTOSTR( it.fp ) << "\" Check Failed: " << rich::style_reset << "got " << it.hash_readin << ", should be " << it.hash << "." << std::endl;
            this -> _errhlist.push_back( it );
        }
    }

#ifdef SHOW_PROGRESS_DETAIL
    CPOUT << rich::FColor::GREEN << "Check complete: " << rich::style_reset << this -> _ori_hlist_len << " files got in total; " 
          << this -> _hlist.size() - this -> _errhlist.size() << " passed; " 
          << this -> _errhlist.size() << " failed; " 
          << this -> _ori_hlist_len - this -> _hlist.size() << " error." 
          << std::endl; 
#endif

    return ( _errhlist.empty() ) ? HUFOSTATUS::OK : HUFOSTATUS::HASCHECKFAILEDF;
}

rena::HUFO::HUFOSTATUS rena::HUFO::_do_hashcalc( unsigned short threads ){
    ThreadPool pool( threads );

    this -> _ori_hlist_len += this -> _hlist.size();

#ifdef SHOW_PROGRESS_DETAIL
    CPOUT << "Found " << this -> _ori_hlist_len << " files in total";
    if ( this -> _ori_hlist_len == this -> _hlist.size() )
    {
        CPOUT << "." << std::endl;
    } // no files have error and have already been erased before hashcalc
    else
    {
        size_t err_before_hashcalc_num = this -> _ori_hlist_len - this -> _hlist.size();
        CPOUT << ", " << err_before_hashcalc_num << " file" << ( ( err_before_hashcalc_num == 1 ) ? " " : "s " ) << "had error and skipped." << std::endl;
    }
#endif

    if ( this -> _hlist.empty() )
        return HUFOSTATUS::OK;
    // empty hlist

#ifdef SHOW_PROGRESS_DETAIL
    auto calc_hash_start_time = std::chrono::steady_clock::now();
    rena::global_speed_watcher = new rena::speedwatcher( calc_hash_start_time );
#endif

    for ( auto it = this -> _hlist.begin() ; it != this -> _hlist.end() ; )
    {
        std::filesystem::path ap; // absolute path
        if ( it -> fp.is_relative() )
        {
            ap = this -> _pdpath / it -> fp;
        }
        else
        {
            ap = it -> fp;
        }
        it -> hash_future = pool.enqueue( this -> _hf , ap ).share();
        ++it;
    } // iterate _hlist, start tasks

#ifdef SHOW_PROGRESS_DETAIL
    do {
        CPOUT << rich::clear_line << "Progress: " << global_speed_watcher -> get_finished() << "/" << this -> _hlist.size() << " "
              << std::fixed << std::setprecision( 2 ) << global_speed_watcher -> get_speed() / 1048576.0 <<  "MB/s\r" << std::flush;
        std::this_thread::sleep_for( std::chrono::microseconds( 50 ) );
    } while ( !pool.is_terminated() );
    CPOUT << rich::clear_line << "Progress: " << global_speed_watcher -> get_finished() << "/" << this -> _hlist.size() << " "
          << std::fixed << std::setprecision( 2 ) << global_speed_watcher -> get_speed() / 1048576.0 <<  "MB/s" << std::endl;
    // last flush

    auto calc_hash_end_time = std::chrono::steady_clock::now();
    auto calc_hash_duration = std::chrono::duration_cast<std::chrono::milliseconds>( calc_hash_end_time - calc_hash_start_time );
    CPOUT << "Total time spent on hash calculations: " << calc_hash_duration.count() / 1000.0 << "s." << std::endl;
    delete global_speed_watcher;
    global_speed_watcher = nullptr;
    // free global_speed_watcher
    CPOUT << "Getting results." << std::endl;
#else
    while ( !pool.is_terminated() );
#endif

    for ( auto it = this -> _hlist.begin() ; it != this -> _hlist.end() ; )
    {
        try {
            it -> hash = it -> hash_future.get();
        }
        catch ( const std::exception& e )
        {
            CPERR << rich::FColor::RED << "Operate file \"" << CPPATHTOSTR( it -> fp ) << "\" failed: " << rich::style_reset << e.what() << std::endl
                  << "Skip." << std::endl;
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
            if ( this -> _using_fig )
            {
                if ( this -> _figobj.check( std::filesystem::relative( fp , this -> _pdpath ) , cppfignore::FILETYPE::IS_DIR ) )
                {
                    continue;
                } // should be ignored
            } // using file ignore, do ignore check
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
            if ( this -> _using_fig )
            {
                if ( this -> _figobj.check( temp.fp , cppfignore::FILETYPE::IS_FILE ) )
                {
                    continue;
                } // should be ignored
            } // using file ignore, do ignore check
            this -> _hlist.push_back( temp );
        } // write relative path to _hlist
    }
    std::sort( this -> _hlist.begin() , this -> _hlist.end() , []( const HASHOBJ& a , const HASHOBJ& b ){
        return a.fp < b.fp;
    });
    // sort hash list to lexicographic order
    return;
}

void rena::HUFO::_read_huf_write_to_hlist(){
    std::string buf;
    while ( std::getline( this -> _rwF , buf ) )
    {
        HASHOBJ temp;
        temp.fp = CPATOWCONV( buf.substr( 0 , buf.rfind( ' ' ) ) );
        temp.hash_readin = CPATOWCONV( buf.substr( buf.rfind( ' ' ) + 1 ) );
        if ( temp.hash_readin.size() != this -> _hlen )
        {
            CPERR << rich::FColor::RED << "File \"" << CPPATHTOSTR( temp.fp ) << "\" wrong hash length: " << rich::style_reset << "got " << temp.hash_readin.size() << ", should be " << this -> _hlen << "." << std::endl
                  << "Skip." << std::endl;
            this -> _ori_hlist_len++;
            // this file will not be written into _hlist, but it's still an error file
            continue;
        }
        for ( auto& c : temp.hash_readin )
        {
            if ( std::isupper( c ) )
            {
                c = std::tolower( c );
            }
        } // ignore cases
        this -> _hlist.push_back( temp );
    }
    return;
}
