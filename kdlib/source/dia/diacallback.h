// 
// DIA load callback
// 

#pragma once

#include <comutil.h>

namespace kdlib {

class DiaLoadCallback2 : public IDiaLoadCallback2 {
    int m_nRefCount;
    std::wstring *m_openedSymbolFile;
public:
    DiaLoadCallback2(std::wstring *openedSymbolFile = NULL) 
        : m_nRefCount(0), m_openedSymbolFile(openedSymbolFile)
    {
    }

    ULONG STDMETHODCALLTYPE AddRef() {
        m_nRefCount++;
        return m_nRefCount;
    }
    ULONG STDMETHODCALLTYPE Release() {
        const int nRefCount = (--m_nRefCount);
        if (!nRefCount)
            delete this;
        return nRefCount;
    }
    HRESULT STDMETHODCALLTYPE QueryInterface( REFIID rid, void **ppUnk ) {
        if ( ppUnk == NULL )
            return E_INVALIDARG;

        if (rid == __uuidof( IDiaLoadCallback2 ) )
            *ppUnk = (IDiaLoadCallback2 *)this;
        else if (rid == __uuidof( IDiaLoadCallback ) )
            *ppUnk = (IDiaLoadCallback *)this;
        else if (rid == __uuidof( IUnknown ) )
            *ppUnk = (IUnknown *)this;
        else
            *ppUnk = NULL;

        if ( *ppUnk != NULL ) {
            AddRef();
            return S_OK;
        }
        return E_NOINTERFACE;
    }

    HRESULT STDMETHODCALLTYPE NotifyDebugDir(
        BOOL fExecutable, 
        DWORD cbData,
        BYTE data[]
    ) 
    {
        DBG_UNREFERENCED_PARAMETER(fExecutable);
        DBG_UNREFERENCED_PARAMETER(cbData);
        DBG_UNREFERENCED_PARAMETER(data);
        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE NotifyOpenDBG(
        LPCOLESTR dbgPath, 
        HRESULT resultCode
    )
    {
        if (S_OK == resultCode && m_openedSymbolFile)
            *m_openedSymbolFile = std::wstring( _bstr_t(dbgPath) );

        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE NotifyOpenPDB(
        LPCOLESTR pdbPath, 
        HRESULT resultCode
    )
    {
        if (S_OK == resultCode && m_openedSymbolFile)
            *m_openedSymbolFile = std::wstring( _bstr_t(pdbPath) );

        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE RestrictRegistryAccess() {
        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE RestrictSymbolServerAccess() {
      return S_OK;
    }
    HRESULT STDMETHODCALLTYPE RestrictOriginalPathAccess() {
        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE RestrictReferencePathAccess() {
        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE RestrictDBGAccess() {
        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE RestrictSystemRootAccess() {
        return S_OK;
    }
};

}
