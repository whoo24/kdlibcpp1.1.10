#pragma once

#include "kdlib/dbgtypedef.h"
#include "kdlib/breakpoint.h"
#include "kdlib/dbgcallbacks.h"
#include "kdlib/typeinfo.h"
#include "kdlib/module.h"

namespace kdlib {

///////////////////////////////////////////////////////////////////////////////

class ProcessMonitor {

public: // init and deinit

    static void init();
    static void deinit();

public: // notification

    static DebugCallbackResult processStart(PROCESS_DEBUG_ID id);
    static DebugCallbackResult processStop(PROCESS_DEBUG_ID id, ProcessExitReason reason, unsigned int ExitCode);
    static DebugCallbackResult createThread();
    static DebugCallbackResult stopThread();
    static DebugCallbackResult moduleLoad(PROCESS_DEBUG_ID id, MEMOFFSET_64 offset, const std::wstring &moduleName);
    static DebugCallbackResult moduleUnload(PROCESS_DEBUG_ID id, MEMOFFSET_64  offset, const std::wstring &moduleName);
    static DebugCallbackResult breakpointHit(PROCESS_DEBUG_ID id, BreakpointPtr& breakpoint);
    static void currentThreadChange(THREAD_DEBUG_ID id);
    static void executionStatusChange(ExecutionStatus status);
    static void breakpointsChange(PROCESS_DEBUG_ID id);
    static void localScopeChange();
    static void changeSymbolPaths();
    static DebugCallbackResult exceptionHit(const ExceptionInfo& excinfo);
    static void debugOutput(const std::wstring& text);
    static void startInput();
    static void stopInput();

public: // process manipulation

    static unsigned int getNumberProcesses();
    static void processAllTerminate();
    static void processAllDetach();


public: // module manipulation

    static ModulePtr getModule( MEMOFFSET_64  offset, PROCESS_DEBUG_ID id = -1 );
    static void insertModule( ModulePtr& module, PROCESS_DEBUG_ID id = -1 );

public: //breakpoint callbacks

    static void registerBreakpoint( BreakpointPtr& breakpoint, PROCESS_DEBUG_ID id = -1 );
    static void removeBreakpoint( BreakpointPtr& breakpoint, PROCESS_DEBUG_ID id = -1 );

public: //callbacks
    static void registerEventsCallback(DebugEventsCallback *callback);
    static void removeEventsCallback(DebugEventsCallback *callback);

public: // 

    static TypeInfoPtr getTypeInfo(const std::wstring& name, PROCESS_DEBUG_ID id = -1);
    static void insertTypeInfo( const TypeInfoPtr& typeInfo, PROCESS_DEBUG_ID id = -1);
};

///////////////////////////////////////////////////////////////////////////////

} // namespace kdlib
