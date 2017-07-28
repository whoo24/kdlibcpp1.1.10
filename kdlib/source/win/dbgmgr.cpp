#include "stdafx.h"

#include <comutil.h>

#include <kdlib/dbgio.h>

#include "dbgmgr.h"
#include "moduleimp.h"
#include "processmon.h"

#include "win/exceptions.h"

namespace kdlib {

DebugManagerWrapper   g_dbgMgr;

///////////////////////////////////////////////////////////////////////////////

DebugManager::DebugManager()
{
    m_previousExecutionStatus = DebugStatusNoDebuggee;
    m_previousCurrentThread = 0;
    m_remote = false;
    m_quietNotification = false;

    CoInitialize(NULL);

    HRESULT  hres = DebugCreate( __uuidof(IDebugClient4), (void **)&client );
    if ( FAILED( hres ) )
        throw DbgEngException(L"DebugCreate", hres);

    control = CComQIPtr<IDebugControl4>(client);
    system = CComQIPtr<IDebugSystemObjects4>(client);
    dataspace = CComQIPtr<IDebugDataSpaces4>(client);
    symbols = CComQIPtr<IDebugSymbols3>(client);
    advanced = CComQIPtr<IDebugAdvanced2>(client);
    registers = CComQIPtr<IDebugRegisters2>(client);

    client->SetEventCallbacksWide( this );
    client->SetOutputCallbacksWide(this);
    client->SetInputCallbacks(this);

    client->SetOutputMask(DEBUG_OUTPUT_NORMAL);
}

///////////////////////////////////////////////////////////////////////////////

DebugManager::DebugManager( const std::wstring& remoteOptions )
{
    m_previousExecutionStatus = DebugStatusNoChange;
    m_previousCurrentThread = 0;
    m_remote = true;

    CoInitialize(NULL);

    HRESULT  hres = DebugConnectWide( remoteOptions.c_str(), __uuidof(IDebugClient4), (void **)&client );
    if ( FAILED( hres ) )
        throw DbgEngException(L"DebugConnectWide", hres);

    hres = client->ConnectSession( 0, 0x1000 );
    if ( FAILED( hres ) )
        throw DbgEngException(L"IDebugClient::ConnectSession", hres );

    control = CComQIPtr<IDebugControl4>(client);
    system = CComQIPtr<IDebugSystemObjects4>(client);
    dataspace = CComQIPtr<IDebugDataSpaces4>(client);
    symbols = CComQIPtr<IDebugSymbols3>(client);
    advanced = CComQIPtr<IDebugAdvanced2>(client);
    registers = CComQIPtr<IDebugRegisters2>(client);

    client->SetEventCallbacksWide(this);

    client->SetOutputCallbacksWide(this);

    client->SetOutputMask(DEBUG_OUTPUT_NORMAL);
}

///////////////////////////////////////////////////////////////////////////////

DebugManager::~DebugManager()
{
    client->SetEventCallbacks(NULL);

    client->SetOutputCallbacks(NULL);

    CoUninitialize();
}

///////////////////////////////////////////////////////////////////////////////

void DebugManager::registerEventsCallback(DebugEventsCallback *callback)
{
    ProcessMonitor::registerEventsCallback(callback);
}

///////////////////////////////////////////////////////////////////////////////

void DebugManager::removeEventsCallback(DebugEventsCallback *callback)
{
    ProcessMonitor::removeEventsCallback(callback);
}

///////////////////////////////////////////////////////////////////////////////

inline
ULONG ConvertCallbackResult( DebugCallbackResult result )
{
    switch( result )
    {
    case DebugCallbackBreak:
        return DEBUG_STATUS_BREAK;

    case DebugCallbackProceed:
        return DEBUG_STATUS_GO_HANDLED;

    default:
        assert( 0 );

    case DebugCallbackNoChange:
        return DEBUG_STATUS_NO_CHANGE;
    }
}

///////////////////////////////////////////////////////////////////////////////

DebugCallbackResult BreakpointCallbackHandler(IDebugBreakpoint2 *bp);

HRESULT STDMETHODCALLTYPE DebugManager::Breakpoint( IDebugBreakpoint2 *bp ) 
{
    DebugCallbackResult  result = BreakpointCallbackHandler(bp);
   
    return ConvertCallbackResult( result );
}

///////////////////////////////////////////////////////////////////////////////

ExecutionStatus ConvertDbgEngineExecutionStatus( ULONG status )
{
    switch( status )
    {
    case DEBUG_STATUS_NO_CHANGE:
        return DebugStatusNoChange;

    case DEBUG_STATUS_GO:
    case DEBUG_STATUS_GO_HANDLED:
    case DEBUG_STATUS_GO_NOT_HANDLED:
    case DEBUG_STATUS_STEP_OVER:
    case DEBUG_STATUS_STEP_INTO:
    case DEBUG_STATUS_STEP_BRANCH:
    case DEBUG_STATUS_IGNORE_EVENT:
        return DebugStatusGo;

    case DEBUG_STATUS_BREAK:
        return DebugStatusBreak;

    case DEBUG_STATUS_NO_DEBUGGEE:
        return DebugStatusNoDebuggee;
    }

    NOT_IMPLEMENTED();
}

HRESULT STDMETHODCALLTYPE DebugManager::ChangeEngineState(
    __in ULONG Flags,
    __in ULONG64 Argument )
{
    try {

        if (((Flags & DEBUG_CES_EXECUTION_STATUS) != 0) &&
            ((Argument & DEBUG_STATUS_INSIDE_WAIT) == 0) &&
            (ULONG)Argument != m_previousExecutionStatus)
        {
            if (m_previousExecutionStatus == DEBUG_STATUS_NO_DEBUGGEE &&
                (ULONG)Argument != DEBUG_STATUS_GO)
                return S_OK;

            ExecutionStatus  executionStatus = ConvertDbgEngineExecutionStatus((ULONG)Argument);

            if (!m_quietNotification)
                ProcessMonitor::executionStatusChange(executionStatus);

            m_previousExecutionStatus = (ULONG)Argument;
        }

        if (m_previousExecutionStatus == DEBUG_STATUS_BREAK &&
            ((Flags & DEBUG_CES_CURRENT_THREAD) != 0) &&
            (m_previousCurrentThread != (ULONG)Argument) &&
            (DEBUG_ANY_ID != (ULONG)Argument))
        {
            ULONG  threadId = (ULONG)Argument;

            if (!m_quietNotification)
                ProcessMonitor::currentThreadChange(threadId);

            m_previousCurrentThread = threadId;
        }

        if ((Flags & DEBUG_CES_BREAKPOINTS) != 0)
        {
            ProcessMonitor::breakpointsChange(getCurrentProcessId());
        }

        if ((Flags & DEBUG_CES_EFFECTIVE_PROCESSOR) != 0)
        {
            if (!m_quietNotification)
                ProcessMonitor::localScopeChange();
        }
    }
    catch (kdlib::DbgException&)
    {
    }

    return S_OK;
}


///////////////////////////////////////////////////////////////////////////////

HRESULT STDMETHODCALLTYPE DebugManager::Exception(
    __in PEXCEPTION_RECORD64 Exception,
    __in ULONG FirstChance )
{

    DebugCallbackResult  result = DebugCallbackNoChange;

    try {

        ExceptionInfo   excinfo = {};

        excinfo.firstChance = FirstChance != 0;

        excinfo.exceptionCode = Exception->ExceptionCode;
        excinfo.exceptionFlags = Exception->ExceptionFlags;
        excinfo.exceptionRecord = Exception->ExceptionRecord;
        excinfo.exceptionAddress = Exception->ExceptionAddress;
        excinfo.parameterCount = Exception->NumberParameters;

        for (ULONG i = 0; i < Exception->NumberParameters; ++i)
            excinfo.parameters[i] = Exception->ExceptionInformation[i];

        result = ProcessMonitor::exceptionHit(excinfo);
    }
    catch (kdlib::DbgException&)
    {
    }

    return ConvertCallbackResult(result);
}

///////////////////////////////////////////////////////////////////

HRESULT STDMETHODCALLTYPE DebugManager::LoadModule(
    __in ULONG64 ImageFileHandle,
    __in ULONG64 BaseOffset,
    __in ULONG ModuleSize,
    __in PCWSTR ModuleName,
    __in PCWSTR ImageName,
    __in ULONG CheckSum,
    __in ULONG TimeDateStamp
    )
{
    DebugCallbackResult  result = DebugCallbackNoChange;

    try {

        std::wstring  moduleName = getModuleName(BaseOffset);

        result = ProcessMonitor::moduleLoad( getCurrentProcessId(), BaseOffset, moduleName);
    }
    catch (kdlib::DbgException&)
    {}

    return ConvertCallbackResult(result);
}

///////////////////////////////////////////////////////////////////

HRESULT STDMETHODCALLTYPE DebugManager::UnloadModule(
    __in PCWSTR ImageBaseName,
    __in ULONG64 BaseOffset
    )
{
    DebugCallbackResult  result = DebugCallbackNoChange;

    try {

        std::wstring  moduleName = getModuleName(BaseOffset);

        result = ProcessMonitor::moduleUnload(getCurrentProcessId(), BaseOffset, moduleName);

    }
    catch (kdlib::DbgException&)
    {}

    return ConvertCallbackResult(result);
}

///////////////////////////////////////////////////////////////////

HRESULT STDMETHODCALLTYPE DebugManager::CreateProcess(
    __in ULONG64 ImageFileHandle,
    __in ULONG64 Handle,
    __in ULONG64 BaseOffset,
    __in ULONG ModuleSize,
    __in PCWSTR ModuleName,
    __in PCWSTR ImageName,
    __in ULONG CheckSum,
    __in ULONG TimeDateStamp,
    __in ULONG64 InitialThreadHandle,
    __in ULONG64 ThreadDataOffset,
    __in ULONG64 StartOffset
    )
{
    DebugCallbackResult  result = DebugCallbackNoChange;

    try {

        PROCESS_DEBUG_ID  processId = getCurrentProcessId();

        result = ProcessMonitor::processStart(processId);

        ProcessMonitor::moduleLoad(processId, BaseOffset, std::wstring(ModuleName));

    }
    catch (kdlib::DbgException&)
    {
    }

    return ConvertCallbackResult( result );
}

///////////////////////////////////////////////////////////////////

HRESULT STDMETHODCALLTYPE DebugManager::ExitProcess(
    __in ULONG ExitCode
    )
{
    DebugCallbackResult  result = DebugCallbackNoChange;

    try {

        PROCESS_DEBUG_ID  procId = getCurrentProcessId();

        result = ProcessMonitor::processStop(procId, ProcessExit, ExitCode);

    }
    catch (kdlib::DbgException&)
    {
    }

    return ConvertCallbackResult( result );
}

///////////////////////////////////////////////////////////////////

HRESULT STDMETHODCALLTYPE DebugManager::CreateThread(
    ULONG64  handle,
    ULONG64  dataOffset,
    ULONG64  startOffset
    )
{
    DebugCallbackResult  result = DebugCallbackNoChange;

    try {

        result = ProcessMonitor::createThread();

    }
    catch (kdlib::DbgException&)
    {
    }

    return ConvertCallbackResult(result);
}

///////////////////////////////////////////////////////////////////

HRESULT STDMETHODCALLTYPE DebugManager::ExitThread(
    ULONG ExitCode
    )
{
    DebugCallbackResult  result = DebugCallbackNoChange;

    try {

        result = ProcessMonitor::stopThread();
    }
    catch (kdlib::DbgException&)
    {
    }

    return ConvertCallbackResult(result);
}

///////////////////////////////////////////////////////////////////

HRESULT STDMETHODCALLTYPE DebugManager::ChangeSymbolState(
    __in ULONG Flags,
    __in ULONG64 Argument
    )
{
    try {

        if ( ( Flags & DEBUG_CSS_SCOPE ) != 0 )
        {
            if (!m_quietNotification)
                ProcessMonitor::localScopeChange();
        }

        if ((Flags & DEBUG_CSS_PATHS) != 0)
            ProcessMonitor::changeSymbolPaths();
    }
    catch (kdlib::DbgException&)
    {
    }

    return S_OK;
}

///////////////////////////////////////////////////////////////////

HRESULT STDMETHODCALLTYPE DebugManager::Output(
    __in ULONG Mask,
    __in PCWSTR Text
    )
{
    try {

        if ( ( Mask & ( DEBUG_OUTPUT_NORMAL | DEBUG_OUTPUT_ERROR | DEBUG_OUTPUT_WARNING ) ) != 0 )
            ProcessMonitor::debugOutput(std::wstring(Text));

    }
    catch (kdlib::DbgException&)
    {
    }

    return S_OK;
}

///////////////////////////////////////////////////////////////////

HRESULT STDMETHODCALLTYPE DebugManager::StartInput(
    __in ULONG BufferSize
    )
{
    ProcessMonitor::startInput();
    return S_OK;
}

///////////////////////////////////////////////////////////////////

HRESULT STDMETHODCALLTYPE DebugManager::EndInput(
    )
{
    ProcessMonitor::stopInput();
    return S_OK;
}

///////////////////////////////////////////////////////////////////

HRESULT STDMETHODCALLTYPE OutputReader::Output(
        __in ULONG Mask,
        __in PCSTR Text )
{
    if (Mask == DEBUG_OUTPUT_NORMAL)
    {
        m_readLine += _bstr_t(Text);
    }

    return S_OK;
}

///////////////////////////////////////////////////////////////////



} //kdlib namespace end
