#include"renalog.h"

#ifdef WIN32

#include<tchar.h>
#include<DbgHelp.h>

#pragma comment( lib , "dbghelp.lib" )

std::string getSymbolInformation( const size_t index , const std::vector<uint64_t> &frame_pointers );

rena::crash_dumper::crash_dumper(){
    this -> m_OriFilter = SetUnhandledExceptionFilter( this -> ExceptionFilter );
    auto res = SymInitialize( GetCurrentProcess() , NULL , TRUE );
    if ( res != TRUE )
        CPERR << "FAILED";
    return;
}

rena::crash_dumper::~crash_dumper(){
    SetUnhandledExceptionFilter( this -> m_OriFilter );
    return;
}

LONG WINAPI rena::crash_dumper::ExceptionFilter( LPEXCEPTION_POINTERS ExpInfo ){
    CONTEXT *context = ExpInfo -> ContextRecord;
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
        if ( StackWalk64( machine_type ,
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

    CPOUT << CPATOWCONV( dumpmsg ) << std::endl;

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
            lineInfo.append( "\t" ).append( line.FileName ).append( " L:" );
            lineInfo.append( std::to_string( line.LineNumber ) );
        }
    }
    frame_dump.append( lineInfo ).append( ":" ).append( callInfo );
    return frame_dump;
}

bool rena::crash_dumper::_placeholder(){
    return true;
}

rena::crash_dumper __global_crashdumper__;

#endif