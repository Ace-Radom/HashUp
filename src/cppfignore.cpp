#include"cppfignore.h"

std::string& trim( std::string& _str ){
    if ( _str.empty() )
    {
        return _str;
    }
    _str.erase( 0 , _str.find_first_not_of( ' ' ) );
    _str.erase( _str.find_last_not_of( ' ' ) + 1 );
    return _str;
}

rena::cppfignore::~cppfignore(){
    if ( this -> _rwFile.is_open() )
    {
        this -> _rwFile.close();
    }
    return;
}

int rena::cppfignore::open( std::filesystem::path ifp ){
    this -> _rwFile.open( ifp , std::ios::in );
    if ( !( this -> _rwFile.is_open() ) )
    {
        return 1;
    }
    this -> _ifp = ifp;
    return 0;
}

int rena::cppfignore::parse(){
    if ( !( this -> _rwFile.is_open() ) || this -> _ifp.empty() )
    {
        return 1;
    } // fstream not open or ignore file path not set

    this -> _rlist.clear();

    std::string this_line;
    while ( std::getline( this -> _rwFile , this_line ) )
    {
        trim( this_line );
        
        if ( this_line[0] == '#' || this_line.empty() )
        {
            continue;
        } // comment line or empty line

        if ( this_line[0] == '\\' && this_line[1] == '#' )
        {
            this_line.erase( 0 , 1 );
        } // '\#' found, earse '\' and go on to parse

#pragma region line_parse

        std::string regex_temp_str;
        for ( int i = 0 ; i < this_line.size() ; i++ )
        {
            if ( i == 0 )
            {
                if ( this_line[i] == '/' )
                {
                    regex_temp_str += "^/";
                    continue;
                }
                regex_temp_str += "/";
            }
            // add '/' to regex begin to fix bugs like: 'test?' matches 'ttest0'
            
            if ( this_line[i] == '*' )
            {
                regex_temp_str += "[^/]*";
            }
            else if ( this_line[i] == '?' )
            {
                regex_temp_str += "[^/]";
            }
            else if ( this_line[i] == '.'  ||
                      this_line[i] == '\\' ||
                      this_line[i] == '$'  ||
                      this_line[i] == '('  ||
                      this_line[i] == ')'  ||
                      this_line[i] == '['  ||
                      this_line[i] == ']'  ||
                      this_line[i] == '{'  ||
                      this_line[i] == '}'  ||
                      this_line[i] == '^'  ||
                      this_line[i] == '+'  ||
                      this_line[i] == '|' )
            {
                regex_temp_str += "\\";
                regex_temp_str += this_line[i];
            }
            else
            {
                regex_temp_str += this_line[i];
            }
        }
        regex_temp_str += "$";
        DEBUG_MSG( CPATOWCONV( regex_temp_str ) );
        try {
            std::regex regex_temp( regex_temp_str );
            this -> _rlist.push_back( regex_temp );
        }
        catch ( const std::exception& e )
        {
            CPERR << e.what() << std::endl;
            return -1;
        }

#pragma endregion line_parse

    }
    return 0;
}

bool rena::cppfignore::check( std::filesystem::path path , rena::cppfignore::FILETYPE ft ){
    std::vector<std::filesystem::path> path_cut_up;
    for ( const auto& pp : path ) // path part
    {
        path_cut_up.push_back( pp );
    }

    if ( path_cut_up[0] != "/" && path_cut_up[0] != "\\" )
    {
        path_cut_up.insert( path_cut_up.begin() , "/" );
    } // add '/' add path begin

    std::filesystem::path path_search_temp;
    for ( const auto& it : path_cut_up )
    {
        path_search_temp /= it;
        if ( this -> _check_pp( path_search_temp , ft ) )
        {
            return true;
        }
    }
    return false;
}

bool rena::cppfignore::_check_pp( std::filesystem::path path , FILETYPE ft ){
    std::string path_str = CPWTOACONV( CPPATHTOSTR( path ) );

#ifdef WIN32
    std::replace( path_str.begin() , path_str.end() , '\\' , '/' );
    // cppfignore always use unix-like path format: that means '\' under windows should be changed into '/'
#endif

    if ( ft == FILETYPE::IS_DIR && path_str.back() != '/' )
    {
        path_str += "/";
    } // add '/' to dir path's end

    DEBUG_MSG( "converted str: " << CPATOWCONV( path_str ) );

    for ( const auto& r : this -> _rlist )
    {
        if ( std::regex_search( path_str , r ) )
        {
            return true;
        }
    }
    return false;
}