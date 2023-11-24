#include"hashup.h"

#if defined( __linux__ ) || defined( __APPLE__ )

/**
 * based on the program from: https://www.flipcode.com/archives/_kbhit_for_Linux.shtml
*/
int rena::_kbhit(){
    struct termios old_attr , new_attr;
    tcgetattr( STDIN_FILENO , &old_attr );
    new_attr = old_attr;
    new_attr.c_lflag &= ~ICANON;
    tcsetattr( STDIN_FILENO , TCSANOW , &new_attr );
    setbuf( stdin , NULL );
    int bytes_waiting;
    ioctl( STDIN_FILENO , FIONREAD , &bytes_waiting );
    tcsetattr( STDIN_FILENO , TCSANOW , &old_attr );
    return bytes_waiting;
}

/**
 * based on the program from: https://stackoverflow.com/questions/7469139/what-is-the-equivalent-to-getch-getche-in-linux
*/
char rena::_getch(){
    struct termios old_attr , new_attr;
    tcgetattr( STDIN_FILENO , &old_attr );
    new_attr = old_attr;
    new_attr.c_lflag &= ~ICANON;
    tcsetattr( STDIN_FILENO , TCSANOW , &new_attr );
    char c;
    c = getchar();
    tcsetattr( STDIN_FILENO , TCSANOW , &old_attr );
    return c;
}

#endif

void rena::watch_kb_signal( const rena::HUFO* hufoobj ){
    if ( kbhit() )
    {
        char c = getch();
            
        if ( c == 's' )
        {
            LOG( INFO , kbs ,
                "KBS_S triggered"
            );
            CPOUT << rich::clear_line
                  << "==================================================" << std::endl
                  << rich::FColor::YELLOW << "Time:\t\t\t"             << rich::style_reset << CPATOWCONV( get_time_str_now() ) << std::endl
                  << rich::FColor::YELLOW << "Finished:\t\t"           << rich::style_reset << global_speed_watcher -> get_finished() << " Files" << std::endl
                  << rich::FColor::YELLOW << "Total:\t\t\t"            << rich::style_reset << hufoobj -> _hlist.size() << " Files" << std::endl
                  << rich::FColor::YELLOW << "Avg. Speed:\t\t"         << rich::style_reset << std::fixed << std::setprecision( 2 ) << global_speed_watcher -> get_speed() / 1048576.0 << "MB/s" << std::endl
                  << rich::FColor::YELLOW << "Exp. Time Left:\t\t"     << rich::style_reset << global_speed_watcher -> get_expected_time_left( hufoobj ) << std::endl
                  << rich::FColor::YELLOW << "Files in process now:" << rich::style_reset << std::endl;
            std::vector<std::filesystem::path> files_in_process = global_speed_watcher -> get_files_in_process();
            for ( int i = 0 ; i < files_in_process.size() ; i++ )
            {
                CPOUT << "\t- " << rich::FColor::YELLOW << "thread " << i << ": " << rich::style_reset << CPPATHTOSTR( files_in_process[i] ) << std::endl;
            }
            CPOUT << "==================================================" << std::endl;
        } // status
        else if ( c == 'p' )
        {
            LOG( INFO , kbs ,
                "KBS_P triggered, raise pause signal"
            );
            pause_signal.store( true );
            global_speed_watcher -> pause_watch();
            CPOUT << rich::clear_line
                  << "Process has been paused, press " << rich::FColor::YELLOW << "[R]" << rich::style_reset << " to resume." << std::endl;
wait_for_resume:
            while ( !kbhit() );
            if ( getch() != 'r' )
                goto wait_for_resume;
            LOG( INFO , kbs ,
                "KBS_P_R triggered, raise resume signal"
            );
            pause_signal.store( false );
            global_speed_watcher -> resume_watch();
        } // pause
        else if ( c == 'q' )
        {
            LOG( INFO , kbs ,
                "KBS_Q triggered, raise SIGINT"
            );
            raise( SIGINT );
        } // quit
    }
    return;
}

void rena::handle_syssigint( int signum ){
    if ( signum == SIGINT )
    {
        LOG( INFO , syssig ,
            "Catch SIGINT, raise global stop signal"
        );
        quit_signal.store( true );
    } // actually useless
    return;
}