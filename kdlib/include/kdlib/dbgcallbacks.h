#pragma once

#include <string>

#include "kdlib/dbgtypedef.h"

namespace kdlib {

//////////////////////////////////////////////////////////////////////////////

struct DebugEventsCallback {

    virtual DebugCallbackResult onBreakpoint( BREAKPOINT_ID bpId ) = 0;
    virtual DebugCallbackResult onException( const ExceptionInfo &exceptionInfo ) = 0;
    virtual void onExecutionStatusChange( ExecutionStatus executionStatus ) = 0;
    virtual DebugCallbackResult onModuleLoad( MEMOFFSET_64 offset, const std::wstring &name ) = 0;
    virtual DebugCallbackResult onModuleUnload( MEMOFFSET_64 offset, const std::wstring &name ) = 0;
    virtual DebugCallbackResult onProcessStart(PROCESS_DEBUG_ID processid) = 0;
    virtual DebugCallbackResult onProcessExit( PROCESS_DEBUG_ID processid, ProcessExitReason  reason, unsigned long exitCode ) = 0;
    virtual DebugCallbackResult onThreadStart() = 0;
    virtual DebugCallbackResult onThreadStop() = 0;
    virtual void onCurrentThreadChange(THREAD_DEBUG_ID threadid) = 0;
    virtual void onChangeLocalScope() = 0;
    virtual void onChangeSymbolPaths() = 0;
    virtual void onChangeBreakpoints() = 0;
    virtual void onDebugOutput(const std::wstring& text) = 0;
    virtual void onStartInput() = 0;
    virtual void onStopInput() = 0;

};

void registerEventsCallback( DebugEventsCallback *callback );
void removeEventsCallback( DebugEventsCallback *callback );

//////////////////////////////////////////////////////////////////////////////

} //kdlib namespace end
