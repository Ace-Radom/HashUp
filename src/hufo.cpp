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
            LOG( FATAL , hufo ,
                "Target HUFO doesn't exist: path: \"" << CPWTOACONV( CPPATHTOSTR( path ) ) << "\""
            );
            return HUFOSTATUS::FILENOTEXIST;
        } // HUF list doesn't exist
        else if ( !std::filesystem::is_regular_file( path ) )
        {
            LOG( FATAL , hufo ,
                "Target HUFO is not regular file: path: \"" << CPWTOACONV( CPPATHTOSTR( path ) ) << "\""
            );
            return HUFOSTATUS::OPENFILEERR;
        }
        this -> _pdpath = std::filesystem::path( path ).parent_path();
        this -> _rwF.open( path , std::ios::in | std::ios::out );
        if ( !( this -> _rwF.is_open() ) )
        {
            LOG( FATAL , hufo ,
                "Open target HUFO failed: path: \"" << CPWTOACONV( CPPATHTOSTR( path ) ) << "\" what: " << strerror( errno )
            )
            this -> _pdpath.clear();
            return HUFOSTATUS::OPENFILEERR;
        } // open failed
    } // doing hash check
    else if ( this -> _hpurpose == HASHPURPOSE::CREATE )
    {
        if ( std::filesystem::exists( path ) && !_ol_no_ask )
        {
            LOG( DEBUG , hufo ,
                "Asking overlay comfirm"
            );
            if ( !confirm_interrupt( CPTEXT( "This file \"" ) + CPPATHTOSTR( path ) + CPTEXT( "\" already exist. Are you sure to overwrite it?" ) , 'y' , 'N' ) )
            {
                LOG( INFO , hufo ,
                    "Comfirm interrupt"
                )
                return HUFOSTATUS::INTERRUPT;
            }
        } // given file already exists
        this -> _pdpath = std::filesystem::path( path ).parent_path();
        this -> _rwF.open( path , std::ios::in | std::ios::out | std::ios::trunc );
        if ( !( this -> _rwF.is_open() ) )
        {
            LOG( FATAL , hufo ,
                "Open target HUFO failed: path: \"" << CPWTOACONV( CPPATHTOSTR( path ) ) << "\" what: " << strerror( errno )
            );
            this -> _pdpath.clear();
            return HUFOSTATUS::OPENFILEERR;
        } // open failed
    } // doing hash create
    this -> _hufopath = path;
    LOG( INFO , hufo ,
        "Open target HUFO OK"
    );
    return HUFOSTATUS::OK;
}

rena::HUFO::HUFOSTATUS rena::HUFO::open( const std::filesystem::path& path , rena::HASHPURPOSE p , bool _ol_no_ask , const std::filesystem::path& ignore_file_path ){
    LOG( INFO , hufo ,
        "Opening IGF and parsing"
    );
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

    LOG( INFO , hufo ,
        "Starting hash list create"
    );

    if ( this -> _pdpath.empty() )
    {
        LOG( ERROR , hufo ,
            "Calling hash list create with empty _pdpath"
        );
        return HUFOSTATUS::NOWORKINGDIR;
    } // create root dir (_pdpath) not set
    if ( !( this -> _rwF.is_open() ) )
    {
        LOG( ERROR , hufo ,
            "Calling hash list create with closed _rwF"
        );
        return HUFOSTATUS::HUFNOTOPEN;
    } // _rwF isn't open
    if ( _hf == nullptr )
    {
        LOG( WARNING , hufo ,
            "Hook _hf points to nullptr, set hash mode to default md5"
        );
        this -> set_mode( HASHMODE::MD5 );
    } // hash mode not set, use default (MD5)

    // these three if clauses should actually not be called: these errors should have already been handled

    this -> _hlist.clear();
    this -> _traversal_dir_write_to_hlist( this -> _pdpath );

    HUFOSTATUS ret = this -> _do_hashcalc( threads );
    if ( ret == HUFOSTATUS::CALLQUIT )
    {
        return HUFOSTATUS::CALLQUIT;
    }

#ifdef SHOW_PROGRESS_DETAIL
    CPOUT << "Writing hash list to file." << std::endl;
#endif

    for ( auto it : this -> _hlist )
    {
        LOG( DEBUG , hufo ,
            "File \"" << CPWTOACONV( CPPATHTOSTR( it.fp ) ) << "\" hash: " << CPWTOACONV( it.hash ) << ", write"
        );
        DEBUG_MSG( it.fp << " " << it.hash );
        this -> _rwF << CPWTOACONV( CPPATHTOSTR( "." / it.fp ) ) << " " << CPWTOACONV( it.hash ) << std::endl;
    }

#ifdef SHOW_PROGRESS_DETAIL
    LOG( INFO , hufo ,
        "Hash list create complete, " << this -> _ori_hlist_len << " files got in total, " 
                                      << this -> _hlist.size() << " created, "
                                      << this -> _ori_hlist_len - this -> _hlist.size() << " error"
    );
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

    LOG( INFO , hufo ,
        "Starting hash list check"
    );

    if ( this -> _pdpath.empty() )
    {
        LOG( ERROR , hufo ,
            "Calling hash list check with empty _pdpath"
        );
        return HUFOSTATUS::NOWORKINGDIR;
    } // check root dir (_pdpath) not set
    if ( !( this -> _rwF.is_open() ) )
    {
        LOG( ERROR , hufo ,
            "Calling hash list check with closed _rwF"
        );
        return HUFOSTATUS::HUFNOTOPEN;
    } // _rwF isn't open
    if ( _hf == nullptr )
    {
        LOG( ERROR , hufo ,
            "Calling hash list check with hook _hf points to nullptr"
        );
        return HUFOSTATUS::HMODENOTSET;
    } // hash mode not set

    this -> _hlist.clear();
    this -> _errhlist.clear();

    this -> _read_huf_write_to_hlist();

    HUFOSTATUS ret = this -> _do_hashcalc( threads );
    if ( ret == HUFOSTATUS::CALLQUIT )
    {
        return HUFOSTATUS::CALLQUIT;
    }

#ifdef SHOW_PROGRESS_DETAIL
    CPOUT << "Checking." << std::endl;
#endif

    for ( auto it : this -> _hlist )
    {
        if ( it.hash == it.hash_readin )
        {
            LOG( DEBUG , hufo ,
                "File \"" << CPWTOACONV( CPPATHTOSTR( it.fp ) ) << "\" check passed: hash got and calced: " << CPWTOACONV( it.hash )
            );
            DEBUG_MSG( it.fp << " Checked" );
        }
        else
        {
            LOG( WARNING , hufo ,
                "File \"" << CPWTOACONV( CPPATHTOSTR( it.fp ) ) << "\" check failed: hash got: " << CPWTOACONV( it.hash_readin ) << " hash calced: " << CPWTOACONV( it.hash )
            );
            CPOUT << rich::FColor::RED << "File \"" << CPPATHTOSTR( it.fp ) << "\" Check Failed: " << rich::style_reset << "got " << it.hash_readin << ", should be " << it.hash << "." << std::endl;
            this -> _errhlist.push_back( it );
        }
    }

#ifdef SHOW_PROGRESS_DETAIL
    LOG( INFO , hufo ,
        "Hash list check completed, " << this -> _ori_hlist_len << "files got in total, "
                                      << this -> _hlist.size() - this -> _errhlist.size() << "passed, "
                                      << this -> _errhlist.size() << " failed, "
                                      << this -> _ori_hlist_len - this -> _hlist.size() << " error"
    );
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
        LOG( INFO , hufo ,
            "Found " << this -> _ori_hlist_len << " files in total, no error before hashcalc"
        );
        CPOUT << "." << std::endl;
    } // no files have error and have already been erased before hashcalc
    else
    {
        size_t err_before_hashcalc_num = this -> _ori_hlist_len - this -> _hlist.size();
        LOG( INFO , hufo ,
            "Found " << this -> _ori_hlist_len << " files in total, " << err_before_hashcalc_num << " error(s) before hashcalc"
        );
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

    CPOUT << "Press: "
          << rich::FColor::YELLOW << "[S]" << rich::style_reset << "tatus: Detailed Progress\t" 
          << rich::FColor::YELLOW << "[P]" << rich::style_reset << "ause: Suspend the Process\t"
          << rich::FColor::YELLOW << "[Q]" << rich::style_reset << "uit: Quit current process" << std::endl;
    // keyboard signals are only available under windows and linux

#pragma region show_dynamic_progress

    noecho();
    nocursor();

    do {
        CPOUT << rich::clear_line << "Progress: " << global_speed_watcher -> get_finished() << "/" << this -> _hlist.size() << " "
              << std::fixed << std::setprecision( 2 ) << global_speed_watcher -> get_speed() / 1048576.0 << "MB/s\r" << std::flush;
        watch_kb_signal( this );
        std::this_thread::sleep_for( std::chrono::microseconds( 50 ) );
    } while ( !pool.is_terminated() );

    echo();
    showcursor();

    if ( quit_signal.load() )
    {
        LOG( INFO , hufo ,
            "global stop called (SIGINT / KBS_Q), stop hashcalc"
        );
        CPOUT << rich::clear_line << "Cleaningup and exiting..." << std::endl;
        delete global_speed_watcher;
        global_speed_watcher = nullptr;
        return HUFOSTATUS::CALLQUIT;
    } // call quit

#pragma endregion show_dynamic_progress

    CPOUT << rich::clear_line << "Progress: " << global_speed_watcher -> get_finished() << "/" << this -> _hlist.size() << " "
          << std::fixed << std::setprecision( 2 ) << global_speed_watcher -> get_speed() / 1048576.0 <<  "MB/s" << std::endl;
    // last flush

    LOG( INFO , hufo ,
        "All calc jobs finished, speed: " << global_speed_watcher -> get_speed() / 1048576.0 << "MB/s duration: " << global_speed_watcher -> get_duration_s() << "s"
    );

    CPOUT << "Total time spent on hash calculations: " << std::setprecision( 2 ) << global_speed_watcher -> get_duration_s() << "s." << std::endl;
    delete global_speed_watcher;
    global_speed_watcher = nullptr;
    // free global_speed_watcher
    CPOUT << "Getting results." << std::endl;
#else // SHOW_PROGRESS_DETAIL
    while ( !pool.is_terminated() );
#endif

    LOG( INFO , hufo ,
        "Start getting results"
    );

    for ( auto it = this -> _hlist.begin() ; it != this -> _hlist.end() ; )
    {
        try {
            it -> hash = it -> hash_future.get();
            LOG( DEBUG , hufo ,
                "File \"" << CPWTOACONV( CPPATHTOSTR( it -> fp ) ) << "\" future got successfully"
            );
        }
        catch ( const std::exception& e )
        {
            LOG( WARNING , hufo ,
                "File \"" << CPWTOACONV( CPPATHTOSTR( it -> fp ) ) << "\" future got failed: what: " << e.what()
            );
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
    LOG( INFO , hufo ,
        "Traversing directory \"" << CPWTOACONV( CPPATHTOSTR( dir ) ) << "\""
    )

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
            try {
                temp.fsize = std::filesystem::file_size( fp );
            }
            catch ( const std::exception& e )
            {
                LOG( WARNING , hufo ,
                    "Failed to get file size: file: \"" << CPWTOACONV( CPPATHTOSTR( temp.fp ) ) << "\" what: " << e.what()
                );
                CPERR << rich::FColor::RED << "Get file size failed by \"" << CPPATHTOSTR( temp.fp ) << "\": " << rich::style_reset << e.what() << std::endl;
            }
            this -> _tfsize += temp.fsize;
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
    LOG( INFO , hufo ,
        "Reading HUF \"" << CPWTOACONV( CPPATHTOSTR( this -> _hufopath ) ) << "\""
    )

    std::string buf;
    while ( std::getline( this -> _rwF , buf ) )
    {
        HASHOBJ temp;
        temp.fp = CPATOWCONV( buf.substr( 0 , buf.rfind( ' ' ) ) );
        temp.hash_readin = CPATOWCONV( buf.substr( buf.rfind( ' ' ) + 1 ) );
        std::filesystem::path ptemp = temp.fp; // path temp (for file exist check and size get)
        if ( ptemp.is_relative() )
        {
            ptemp = this -> _pdpath / temp.fp;
        } // relative path, make absolute temp path for file exist check and size get
        if ( !std::filesystem::exists( ptemp ) )
        {
            LOG( WARNING , hufo ,
                "File in HUF does not exist: file: \"" << CPWTOACONV( CPPATHTOSTR( temp.fp ) ) << "\""
            );
            CPERR << rich::FColor::RED << "File \"" << CPPATHTOSTR( temp.fp ) << "\" doesn't not exist." << rich::style_reset << std::endl
                  << "Skip." << std::endl;
            this -> _ori_hlist_len++;
            continue;
        } // file in huf not exists
        try {
            temp.fsize = std::filesystem::file_size( ptemp );
        }
        catch ( const std::exception& e )
        {
            LOG( WARNING , hufo ,
                "Failed to get file size: file: \"" << CPWTOACONV( CPPATHTOSTR( temp.fp ) ) << "\" what: " << e.what()
            );
            CPERR << rich::FColor::RED << "Get file size failed by \"" << CPPATHTOSTR( temp.fp ) << "\": " << rich::style_reset << e.what() << std::endl;
        }
        this -> _tfsize += temp.fsize;
        if ( temp.hash_readin.size() != this -> _hlen )
        {
            LOG( WARNING , hufo ,
                "Wrong hash len in HUF: file: \"" << CPWTOACONV( CPPATHTOSTR( temp.fp ) ) << "\" got " << temp.hash_readin.size() << " should be " << this -> _hlen
            );
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
