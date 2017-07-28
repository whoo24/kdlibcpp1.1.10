#pragma once

#include <list>
#include <string>

#include <dbgeng.h>
#include <dbghelp.h>
#include <windows.h>
#include <atlbase.h>

#include <boost/thread/recursive_mutex.hpp>

#include "kdlib/dbgcallbacks.h"

#include "exceptions.h"

namespace kdlib {

///////////////////////////////////////////////////////////////////////////////

class DebugManager : public DebugBaseEventCallbacksWide, public IDebugOutputCallbacksWide, public IDebugInputCallbacks
{

public:

    DebugManager();
    DebugManager( const std::wstring& remoteOptions );

    ~DebugManager();

public:

     CComPtr<IDebugClient5>         client;
     CComPtr<IDebugControl4>        control;
     CComPtr<IDebugSystemObjects4>  system;
     CComPtr<IDebugDataSpaces4>     dataspace;
     CComPtr<IDebugSymbols3>        symbols;
     CComPtr<IDebugAdvanced3>       advanced;
     CComPtr<IDebugRegisters2>      registers;

public:

    bool isRemoteInitialized() {
        return m_remote;
    }

    bool setQuietNotiification(bool quiet) {
        bool previous = m_quietNotification;
        m_quietNotification = quiet;
        return previous;
    }

    void registerEventsCallback(DebugEventsCallback *callback);

    void removeEventsCallback(DebugEventsCallback *callback);

private:

    ULONG                       m_previousExecutionStatus;
    ULONG                       m_previousCurrentThread;
    bool                        m_remote;
    bool                        m_quietNotification;

    template< class T, class TCast=T >
    bool QueryInterface_Case(
        _In_ REFIID InterfaceId,
        _Out_ PVOID* Interface
        )
    {
        if (!IsEqualIID(InterfaceId, __uuidof(T)))
            return false;

        *reinterpret_cast<TCast**>(Interface) = this;
        AddRef();
        return true;
    }

public:

    STDMETHOD_(ULONG, AddRef)() { return 1; }
    STDMETHOD_(ULONG, Release)() { return 1; }

    STDMETHOD(QueryInterface)(
        THIS_
        _In_ REFIID InterfaceId,
        _Out_ PVOID* Interface
        )
    {
        *Interface = NULL;

        if (QueryInterface_Case<IUnknown, IDebugEventCallbacksWide>(InterfaceId, Interface))
            return S_OK;
        if (QueryInterface_Case<IDebugEventCallbacksWide>(InterfaceId, Interface))
            return S_OK;
        if (QueryInterface_Case<IDebugOutputCallbacksWide>(InterfaceId, Interface))
            return S_OK;
        if (QueryInterface_Case<IDebugInputCallbacks>(InterfaceId, Interface))
            return S_OK;

        return E_NOINTERFACE;
    }

    // IDebugEventCallbacks impls
    STDMETHOD(GetInterestMask)(
        __out PULONG Mask 
        )
    {
        *Mask = 0;
        *Mask |= DEBUG_EVENT_BREAKPOINT;
        *Mask |= DEBUG_EVENT_CHANGE_ENGINE_STATE;
        *Mask |= DEBUG_EVENT_CHANGE_SYMBOL_STATE;
        *Mask |= DEBUG_EVENT_EXCEPTION;
        *Mask |= DEBUG_EVENT_LOAD_MODULE;
        *Mask |= DEBUG_EVENT_UNLOAD_MODULE;
        *Mask |= DEBUG_EVENT_CREATE_PROCESS;
        *Mask |= DEBUG_EVENT_EXIT_PROCESS;
        *Mask |= DEBUG_EVENT_CREATE_THREAD;
        *Mask |= DEBUG_EVENT_EXIT_THREAD;

        return S_OK;
    }

    STDMETHOD(Breakpoint)(
        __in IDebugBreakpoint2 *bp
    );

    STDMETHOD(ChangeEngineState)(
        __in ULONG Flags,
        __in ULONG64 Argument );


    STDMETHOD(Exception)(
        __in PEXCEPTION_RECORD64 Exception,
        __in  ULONG FirstChance );

    STDMETHOD(LoadModule)(
        __in ULONG64 ImageFileHandle,
        __in ULONG64 BaseOffset,
        __in ULONG ModuleSize,
        __in PCWSTR ModuleName,
        __in PCWSTR ImageName,
        __in ULONG CheckSum,
        __in ULONG TimeDateStamp
        );

    STDMETHOD(UnloadModule)(
        __in PCWSTR ImageBaseName,
        __in ULONG64 BaseOffset
        );

    STDMETHOD(CreateProcess)(
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
        );

    STDMETHOD(ExitProcess)(
        __in ULONG ExitCode
        );

    STDMETHOD(CreateThread)(
        ULONG64  handle,
        ULONG64  dataOffset,
        ULONG64  startOffset
        );

    STDMETHOD(ExitThread)(
        ULONG ExitCode
        );

    STDMETHOD(ChangeSymbolState)(
        __in ULONG Flags,
        __in ULONG64 Argument
        );

    STDMETHOD(Output)(
        __in ULONG Mask,
        __in PCWSTR Text
        );

    STDMETHOD(StartInput)(
        __in ULONG BufferSize
        );

    STDMETHOD(EndInput)(
        );

};


class DebugManagerWrapper {

public:

    void set( DebugManager*  dbgMgr ) {
        m_dbgMgr = dbgMgr;
    }

    void reset() {
        delete m_dbgMgr;
        m_dbgMgr = 0;
    }

    DebugManager* operator->() {
        if (!m_dbgMgr)
            throw DbgException("pykd is not initialized");

        return m_dbgMgr;
    }

    bool operator==(DebugManager* ptr) {
        return m_dbgMgr == ptr;
    }

    bool operator!=(DebugManager* ptr) {
        return !(m_dbgMgr == ptr);
    }

private:

    DebugManager*  m_dbgMgr;
};


extern DebugManagerWrapper   g_dbgMgr;

///////////////////////////////////////////////////////////////////////////////

class OutputReader : public IDebugOutputCallbacks, private boost::noncopyable {

public:

    explicit OutputReader(IDebugClient5*  client)
    {
        HRESULT  hres;

        m_callbacks = NULL;
        m_client = client;

        hres = m_client->GetOutputCallbacks(&m_callbacks);
        if ( FAILED( hres ) )
            throw DbgEngException( L"IDebugClient::GetOutputCallbacks", hres);

        hres = m_client->SetOutputCallbacks(this );
        if ( FAILED( hres ) )
            throw DbgEngException( L"IDebugClient::SetOutputCallbacks", hres);
    }

    ~OutputReader() 
    {
        m_client->SetOutputCallbacks(m_callbacks);
    }

    const std::wstring&
    Line() const {
        return  m_readLine;
    }

    CComPtr<IDebugClient5>& getClient() {
        return m_client;
    }

private:

     // IUnknown.
    STDMETHOD(QueryInterface)(
        __in REFIID InterfaceId,
        __out PVOID* Interface ) {
        return E_NOINTERFACE;
    }

    STDMETHOD_(ULONG, AddRef)() {
        return 1L;
    }


    STDMETHOD_(ULONG, Release)() {
        return 0L;
    }

   STDMETHOD(Output)(
        __in ULONG Mask,
        __in PCSTR Text );

private:

    std::wstring                        m_readLine;

    CComPtr<IDebugClient5>              m_client;

    PDEBUG_OUTPUT_CALLBACKS             m_callbacks;
};

///////////////////////////////////////////////////////////////////////////////

ExecutionStatus ConvertDbgEngineExecutionStatus( ULONG status );

///////////////////////////////////////////////////////////////////////////////

} // kdlib namespace end

