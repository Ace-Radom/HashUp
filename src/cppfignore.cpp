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

void rena::cppfignore::open( std::filesystem::path ifp ){
    this -> _rwFile.open( ifp , std::ios::in );
    if ( !( this -> _rwFile.is_open() ) )
    {
        throw rena_exception( "Open ignore file failed" );
    }
    this -> _ifp = ifp;
    return;
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

#pragma region line_parse

        std::string regex_temp_str;
        for ( int i = 0 ; i < this_line.size() ; i++ )
        {
            if ( i == 0 && this_line[i] == '/' )
            {
                regex_temp_str += "^";
            }
            else if ( this_line[i] == '*' )
            {
                regex_temp_str += ".*";
            }
            else if ( this_line[i] == '?' )
            {
                regex_temp_str += ".";
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
        std::cout << regex_temp_str << std::endl;
        try {
            std::regex regex_temp( regex_temp_str );
            this -> _rlist.push_back( regex_temp );
        }
        catch ( const std::exception& e )
        {
            std::cout << e.what() << std::endl;
        }

#pragma endregion line_parse

    }
}

bool rena::cppfignore::check( std::filesystem::path path ){
    std::vector<std::filesystem::path> path_cut_up;
    for ( const auto& pp : path ) // path part
    {
        path_cut_up.push_back( pp );
    }

    std::filesystem::path path_search_temp;
    for ( const auto& it : path_cut_up )
    {
        path_search_temp /= it;
        if ( this -> _check_pp( path_search_temp ) )
        {
            return true;
        }
    }
    return false;
}

bool rena::cppfignore::_check_pp( std::filesystem::path path ){
    for ( const auto& r : this -> _rlist )
    {
        if ( std::regex_match( path.string() , r ) )
        {
            return true;
        }
    }
    return false;
}