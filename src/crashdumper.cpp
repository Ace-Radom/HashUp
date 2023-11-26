#include"renalog.h"

#define BUILD_EXP_NAME_PAIR( x ) { x , #x }

#ifdef WIN32

const std::map<rena::EXPSIGID,std::string> exp_name_map = {
    BUILD_EXP_NAME_PAIR( EXCEPTION_ACCESS_VIOLATION )         ,
    BUILD_EXP_NAME_PAIR( EXCEPTION_ARRAY_BOUNDS_EXCEEDED )    ,
    BUILD_EXP_NAME_PAIR( EXCEPTION_DATATYPE_MISALIGNMENT )    ,
    BUILD_EXP_NAME_PAIR( EXCEPTION_FLT_DENORMAL_OPERAND )     ,
    BUILD_EXP_NAME_PAIR( EXCEPTION_FLT_DIVIDE_BY_ZERO )       ,
    BUILD_EXP_NAME_PAIR( EXCEPTION_FLT_INEXACT_RESULT )       ,
    BUILD_EXP_NAME_PAIR( EXCEPTION_FLT_INEXACT_RESULT )       ,
    BUILD_EXP_NAME_PAIR( EXCEPTION_FLT_INVALID_OPERATION )    ,
    BUILD_EXP_NAME_PAIR( EXCEPTION_FLT_OVERFLOW )             ,
    BUILD_EXP_NAME_PAIR( EXCEPTION_FLT_STACK_CHECK )          ,
    BUILD_EXP_NAME_PAIR( EXCEPTION_FLT_UNDERFLOW )            ,
    BUILD_EXP_NAME_PAIR( EXCEPTION_ILLEGAL_INSTRUCTION )      ,
    BUILD_EXP_NAME_PAIR( EXCEPTION_IN_PAGE_ERROR )            ,
    BUILD_EXP_NAME_PAIR( EXCEPTION_INT_DIVIDE_BY_ZERO )       ,
    BUILD_EXP_NAME_PAIR( EXCEPTION_INT_OVERFLOW )             ,
    BUILD_EXP_NAME_PAIR( EXCEPTION_INVALID_DISPOSITION )      ,
    BUILD_EXP_NAME_PAIR( EXCEPTION_NONCONTINUABLE_EXCEPTION ) ,
    BUILD_EXP_NAME_PAIR( EXCEPTION_PRIV_INSTRUCTION )         ,
    BUILD_EXP_NAME_PAIR( EXCEPTION_STACK_OVERFLOW )           ,
    BUILD_EXP_NAME_PAIR( EXCEPTION_BREAKPOINT )               ,
    BUILD_EXP_NAME_PAIR( EXCEPTION_SINGLE_STEP )              ,
    BUILD_EXP_NAME_PAIR( SIGABRT )                            ,
    BUILD_EXP_NAME_PAIR( SIGFPE )                             ,
    BUILD_EXP_NAME_PAIR( SIGSEGV )                            ,
    BUILD_EXP_NAME_PAIR( SIGILL )                             ,
    BUILD_EXP_NAME_PAIR( SIGTERM )    
};

#else

const std::map<rena::EXPSIGID,std::string> exp_name_map = {
    BUILD_EXP_NAME_PAIR( SIGABRT )                            ,
    BUILD_EXP_NAME_PAIR( SIGBUS )                             ,
    BUILD_EXP_NAME_PAIR( SIGFPE )                             ,
    BUILD_EXP_NAME_PAIR( SIGILL )                             ,
    BUILD_EXP_NAME_PAIR( SIGQUIT )                            ,
    BUILD_EXP_NAME_PAIR( SIGSEGV )                            ,
    BUILD_EXP_NAME_PAIR( SIGSYS )                             ,
    BUILD_EXP_NAME_PAIR( SIGTRAP )                            ,
    BUILD_EXP_NAME_PAIR( SIGXCPU )                            ,
    BUILD_EXP_NAME_PAIR( SIGXFSZ )
};

std::atomic<bool> block_for_sig_restore( true );
std::map<rena::EXPSIGID,struct sigaction> saved_sig_action;

#endif // defined( __linux__ ) || defined( __APPLE__ )

rena::crash_dumper::crash_dumper(){
#ifdef WIN32
    this -> m_OriFilter = SetUnhandledExceptionFilter( this -> ExceptionFilter );
    auto res = SymInitialize( GetCurrentProcess() , NULL , TRUE );
    if ( res != TRUE )
        CPERR << "Initialize symbols failed" << std::endl;
#else
    struct sigaction actions;
    sigemptyset( &actions.sa_mask );
    actions.sa_sigaction = &( this -> sigHandler );
    actions.sa_flags = SA_SIGINFO;
    for ( const auto& sigpair : exp_name_map )
    {
        struct sigaction old_action;
        memset( &old_action , 0 , sizeof( old_action ) );
        if ( sigaction( sigpair.first , &actions , &old_action ) < 0 )
        {
            CPERR << "Sigaction failed by: " << sigpair.second << std::endl;
        }
        else
        {
            saved_sig_action[sigpair.first] = old_action;
        } // save original signal action in order to restore them later
    }
#endif
    return;
}

rena::crash_dumper::~crash_dumper(){
#ifdef WIN32
    SetUnhandledExceptionFilter( this -> m_OriFilter );
#endif
    return;
}

#ifdef WIN32

std::string getSymbolInformation( const size_t index , const std::vector<uint64_t> &frame_pointers );

LONG WINAPI rena::crash_dumper::ExceptionFilter( LPEXCEPTION_POINTERS ExpInfo ){
    CONTEXT *context = ExpInfo -> ContextRecord;
    DWORD ExpID = ExpInfo -> ExceptionRecord -> ExceptionCode;
    std::shared_ptr<void> RaiiSysCleaner( nullptr , [&]( void * ){
        SymCleanup( GetCurrentProcess() );
    });

    const size_t dumpSize = 64;
    std::vector<uint64_t> frameVector( dumpSize );

    DWORD machine_type   = 0;
    STACKFRAME64 frame   = {};
    frame.AddrPC.Mode    = AddrModeFlat;
    frame.AddrFrame.Mode = AddrModeFlat;
    frame.AddrStack.Mode = AddrModeFlat;

#if defined( _M_IX86 )      // x86
    frame.AddrPC.Offset     = context -> Eip;
    frame.AddrFrame.Offset  = context -> Ebp;
    frame.AddrStack.Offset  = context -> Esp;
    machine_type            = IMAGE_FILE_MACHINE_I386;
#elif defined( _M_X64 )     // x64
    frame.AddrPC.Offset     = context -> Rip;
    frame.AddrFrame.Offset  = context -> Rbp;
    frame.AddrStack.Offset  = context -> Rsp;
    machine_type            = IMAGE_FILE_MACHINE_AMD64;
#elif defined( _M_IA64 )    // IA64
    frame.AddrPC.Offset     = context -> StIIP;
    frame.AddrFrame.Offset  = context -> IntSp;
    frame.AddrStack.Offset  = context -> IntSp;
    machine_type            = IMAGE_FILE_MACHINE_IA64;
    frame.AddrBStore.Offset = context.RsBSP;
    frame.AddrBStore.Mode   = AddrModeFlat;
#else                       // i386
    frame.AddrPC.Offset     = context -> Eip;
    frame.AddrFrame.Offset  = context -> Ebp;
    frame.AddrStack.Offset  = context -> Esp;
    machine_type            = IMAGE_FILE_MACHINE_I386;
#endif

    for ( size_t index = 0 ; index < frameVector.size() ; index++ )
    {
        if ( StackWalk( machine_type ,
                        GetCurrentProcess() ,
                        GetCurrentThread() ,
                        &frame ,
                        context ,
                        NULL ,
                        SymFunctionTableAccess ,
                        SymGetModuleBase ,
                        NULL
                      ) )
        {
            frameVector[index] = frame.AddrPC.Offset;
        }
        else
        {
            break;
        }
    }

    std::string dumpmsg;
    const size_t kSize = frameVector.size();
    for ( size_t index = 0 ; index < kSize && frameVector[index] ; index++ )
    {
        dumpmsg += getSymbolInformation( index , frameVector );
        dumpmsg += "\n";
    }

    CPOUT << rich::FColor::RED << "Unhandled exception occured, HashUp has crashed." << rich::style_reset << std::endl;

    if ( RENALOG_ISREADY() )
    {
        __global_logger__ -> flush();
        __global_logger__ -> dump_logline_begin( renalog::RENALOGSEVERITY::FATAL , "rlcdmp" );  // renalog crash dump
        *__global_logger__ << "===== Unhandled exception occured, PROGRAM CRASHED =====\n\n"
                           << "Received FATAL exception: " << get_exception_name( ExpID ) << " [PID: " << getpid() << "]\n"
                           << "=========STACKDUMP=========\n"
                           << dumpmsg
                           << "===========================\n\n";
        __global_logger__ -> copy_current_log_for_fatal();
        RENALOG_FREE();
    }
    else
    {
        CPERR << "Received FATAL exception: " << CPATOWCONV( get_exception_name( ExpID ) ) << " [PID: " << getpid() << "]\n"
              << "=========STACKDUMP=========\n"
              << CPATOWCONV( dumpmsg )
              << "===========================\n\n";
    } // renalog isn't ready

    return EXCEPTION_EXECUTE_HANDLER;
}

std::string getSymbolInformation( const size_t index , const std::vector<uint64_t> &frame_pointers ){
    auto addr = frame_pointers[index];
    std::string frame_dump = "stack dump [" + std::to_string( index ) + "] ";

    DWORD64 displacement64;
    DWORD displacement;
    alignas( SYMBOL_INFO ) char symbol_buffer[sizeof( SYMBOL_INFO ) + MAX_SYM_NAME];
    SYMBOL_INFO *symbol = reinterpret_cast<SYMBOL_INFO*>( symbol_buffer );
    symbol -> SizeOfStruct = sizeof( SYMBOL_INFO );
    symbol -> MaxNameLen = MAX_SYM_NAME;

    IMAGEHLP_LINE line;
    line.SizeOfStruct = sizeof( IMAGEHLP_LINE );
    std::string lineInfo;
    std::string callInfo;
    if ( SymFromAddr( GetCurrentProcess() , addr , &displacement64 , symbol ) )
    {
        callInfo.append( " " ).append( std::string( symbol -> Name , symbol -> NameLen ) );
        if ( SymGetLineFromAddr( GetCurrentProcess() , addr , &displacement , &line ) )
        {
            lineInfo.append( line.FileName ).append( " L:" ).append( std::to_string( line.LineNumber ) );
        }
    }
    frame_dump.append( lineInfo ).append( ":" ).append( callInfo );
    return frame_dump;
}

#else

bool should_do_exit(){
    static std::atomic<uint64_t> firstExit( 0 );
    auto const count = firstExit.fetch_add( 1 , std::memory_order_relaxed );
    return count == 0;
}

std::string stack_trace_dump();

void rena::crash_dumper::sigHandler( int signum , siginfo_t* info , void* ctx ){
    if ( should_do_exit() == false )
    {
        while( block_for_sig_restore.load() )
        {
            std::this_thread::sleep_for( std::chrono::seconds( 1 ) );
        }
    }

    CPOUT << rich::FColor::RED << "Unhandled exception occured, HashUp has crashed." << rich::style_reset << std::endl;
    
    if ( RENALOG_ISREADY() )
    {
        __global_logger__ -> flush();
        __global_logger__ -> dump_logline_begin( renalog::RENALOGSEVERITY::FATAL , "rlcdmp" );  // renalog crash dump
        *__global_logger__ << "===== Unhandled exception occured, PROGRAM CRASHED =====\n\n"
                           << "Received FATAL exception: " << get_exception_name( signum ) << " [PID: " << getpid() << "]\n"
                           << "=========STACKDUMP=========\n"
                           << stack_trace_dump()
                           << "===========================\n\n";
        __global_logger__ -> copy_current_log_for_fatal();
        RENALOG_FREE();
    }
    else
    {
        CPOUT << "===== Unhandled exception occured, PROGRAM CRASHED =====\n\n"
              << "Received FATAL exception: " << get_exception_name( signum ) << " [PID: " << getpid() << "]\n"
              << "=========STACKDUMP=========\n"
              << stack_trace_dump()
              << "===========================\n\n";
    }

    for ( const auto& sigpair : saved_sig_action )
    {
        if ( sigaction( sigpair.first , &sigpair.second , NULL ) < 0 )
        {
            CPERR << "Sigaction failed by: " << sigpair.first << std::endl;
        }
    }
    block_for_sig_restore.store( false );
    return;
}

std::string stack_trace_dump(){
    const size_t dump_depth = 50;
    void* dumpmsg[dump_depth];
    const size_t size = backtrace( dumpmsg , dump_depth );
    char** message = backtrace_symbols( dumpmsg , size );
    std::ostringstream oss;

    for ( size_t i = 1 ; i < size && message != nullptr ; i++ )
    {
        std::string strmsg( message[i] );
        std::string mangled_name;
        std::string offset;

        const auto firstBracket = strmsg.find_last_of( '(' );
        const auto secondBracket = strmsg.find_last_of( ')' );
        if ( firstBracket != std::string::npos && secondBracket != std::string::npos )
        {
            const auto betweenBrackets = strmsg.substr( firstBracket + 1 , secondBracket - firstBracket - 1 );
            const auto plusSign = betweenBrackets.find_first_of( '+' );
            if ( plusSign != std::string::npos )
            {
                mangled_name = betweenBrackets.substr( 0 , plusSign );
                offset = betweenBrackets.substr( plusSign + 1 );
            }
        }
        else
        {
            const auto plusSign = strmsg.find_first_of( '+' );
            const auto lastUnderscore = strmsg.rfind( " _" );
            if ( plusSign != std::string::npos && lastUnderscore != std::string::npos )
            {
                mangled_name = strmsg.substr( lastUnderscore + 1 , plusSign - lastUnderscore - 2 );
                offset = strmsg.substr( plusSign + 2 );
            }
        }

        if ( !mangled_name.empty() && !offset.empty() )
        {
            int status;
            char* real_name = abi::__cxa_demangle( mangled_name.c_str() , 0 , 0 , &status );
            if ( status == 0 )
            {
                oss << "stack dump [" << i << "] " << real_name << " + " << offset << std::endl;
            }
            else
            {
                oss << "stack dump [" << i << "] " << mangled_name << " + " << offset << std::endl;
            }
            free( real_name );
        }
        else
        {
            oss << "stack dump [" << i << "] " << strmsg << std::endl;
        }
    }
    free( message );
    return oss.str();
}

#endif

std::string rena::crash_dumper::get_exception_name( rena::EXPSIGID ExpID ){
    const auto it = exp_name_map.find( ExpID );
    if ( it == exp_name_map.end() )
    {
        std::string unknown_exp;
        unknown_exp.assign( "Unknown Exception: " ).append( std::to_string( ExpID ) );
        return unknown_exp;
    }
    return it -> second;
}

bool rena::crash_dumper::_placeholder(){
    return true;
}

rena::crash_dumper __global_crashdumper__;

#undef BUILD_EXP_NAME_PAIR
