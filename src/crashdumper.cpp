#include"renalog.h"

#ifdef WIN32

#include<tchar.h>
#include<DbgHelp.h>

#pragma comment( lib , "dbghelp.lib" )

rena::crash_dumper::crash_dumper(){
    this -> m_OriFilter = SetUnhandledExceptionFilter( this -> ExceptionFilter );
    auto res = SymInitialize(GetCurrentProcess(), NULL, TRUE);
    if ( res != TRUE )
        CPERR << "FAILED";
    return;
}

rena::crash_dumper::~crash_dumper(){
    SetUnhandledExceptionFilter( this -> m_OriFilter );
    return;
}

std::string getSymbolInformation(const size_t index, const std::vector<uint64_t> &frame_pointers);

LONG WINAPI rena::crash_dumper::ExceptionFilter( LPEXCEPTION_POINTERS ExpInfo ){

    CONTEXT *context = ExpInfo->ContextRecord;
    std::shared_ptr<void> RaiiSysCleaner(nullptr, [&](void *) {
      SymCleanup(GetCurrentProcess());
    });

  const size_t dumpSize = 64;
  std::vector<uint64_t> frameVector(dumpSize);

  DWORD machine_type = 0;
  STACKFRAME64 frame = {};
  frame.AddrPC.Mode = AddrModeFlat;
  frame.AddrFrame.Mode = AddrModeFlat;
  frame.AddrStack.Mode = AddrModeFlat;

#ifdef _M_IX86
  frame.AddrPC.Offset = context->Eip;
  frame.AddrFrame.Offset = context->Ebp;
  frame.AddrStack.Offset = context->Esp;
  machine_type = IMAGE_FILE_MACHINE_I386;
#elif _M_X64
  frame.AddrPC.Offset = context->Rip;
  frame.AddrFrame.Offset = context->Rbp;
  frame.AddrStack.Offset = context->Rsp;
  machine_type = IMAGE_FILE_MACHINE_AMD64;
#elif _M_IA64
  frame.AddrPC.Offset = context->StIIP;
  frame.AddrFrame.Offset = context->IntSp;
  frame.AddrStack.Offset = context->IntSp;
  machine_type = IMAGE_FILE_MACHINE_IA64;
  frame.AddrBStore.Offset = context.RsBSP;
  frame.AddrBStore.Mode = AddrModeFlat;
#else
  frame.AddrPC.Offset = context->Eip;
  frame.AddrFrame.Offset = context->Ebp;
  frame.AddrStack.Offset = context->Esp;
  machine_type = IMAGE_FILE_MACHINE_I386;
#endif

  for (size_t index = 0; index < frameVector.size(); ++index)
  {
    if (StackWalk64(machine_type,
           GetCurrentProcess(),
           GetCurrentThread(),
           &frame,
           context,
           NULL,
           SymFunctionTableAccess64,
           SymGetModuleBase64,
           NULL)) {
      frameVector[index] = frame.AddrPC.Offset;
    } else {
      break;
    }
  }
  

  std::string dump;
  const size_t kSize = frameVector.size();
  CPOUT << kSize << "\n";
  for (size_t index = 0; index < kSize && frameVector[index]; ++index) {
    CPERR << frameVector[index] << std::endl;
    dump += getSymbolInformation(index, frameVector);
    dump += "\n";
  }

//   SymCleanup(GetCurrentProcess());

    CPERR << CPATOWCONV( dump ) << "\n";
    return EXCEPTION_EXECUTE_HANDLER;
}

std::string getSymbolInformation(const size_t index, const std::vector<uint64_t> &frame_pointers) {
      auto addr = frame_pointers[index];
      std::string frame_dump = "stack dump [" + std::to_string(index) + "]\t";

      DWORD64 displacement64;
      DWORD displacement;
      alignas(SYMBOL_INFO) char symbol_buffer[sizeof(SYMBOL_INFO) + MAX_SYM_NAME];
      SYMBOL_INFO *symbol = reinterpret_cast<SYMBOL_INFO *>(symbol_buffer);
      symbol->SizeOfStruct = sizeof(SYMBOL_INFO);
      symbol->MaxNameLen = MAX_SYM_NAME;

      IMAGEHLP_LINE64 line;
      line.SizeOfStruct = sizeof(IMAGEHLP_LINE64);
      std::string lineInformation;
      std::string callInformation;
      if (SymFromAddr(GetCurrentProcess(), addr, &displacement64, symbol)) {
         callInformation.append(" ").append(std::string(symbol->Name, symbol->NameLen));
         if (SymGetLineFromAddr64(GetCurrentProcess(), addr, &displacement, &line)) {
            lineInformation.append("\t").append(line.FileName).append(" L: ");
            lineInformation.append(std::to_string(line.LineNumber));
         }
         else
         {
            CPERR << GetLastError() << "\n";
         }
      }
      else
      {
        CPERR << GetLastError() << "\n";
      }
      frame_dump.append(lineInformation).append(callInformation);
      return frame_dump;
   }

bool rena::crash_dumper::_placeholder(){
    return true;
}

rena::crash_dumper __global_crashdumper__;

#endif