// 
// Load debug symbols using DIA
// 

#include "stdafx.h"


#include <dia2.h>
#include <diacreate.h>

#include <boost/utility.hpp>

#include "kdlib/symengine.h"
#include "kdlib/memaccess.h"

#include "dia/diawrapper.h"
#include "win/utils.h"

#include "diacallback.h"

#include <initguid.h>
DEFINE_GUID(MSDIA11_CLASSGUID, 0x761d3bcd, 0x1304, 0x41d5, 0x94, 0xe8, 0xea, 0xc5, 0x4e, 0x4a, 0xc1, 0x72);

DEFINE_GUID(MSDIA12_CLASSGUID, 0x3bfcea48, 0x620f, 0x4b6b, 0x81, 0xf7, 0xb9, 0xaf, 0x75, 0x45, 0x4c, 0x7d);


//////////////////////////////////////////////////////////////////////////////////

namespace kdlib {

//////////////////////////////////////////////////////////////////////////////////

// DIA data source loader
interface IDataProvider
{
    virtual ~IDataProvider() {}
    virtual HRESULT load(__inout IDiaDataSource &DiaDataSource) = 0;
};

//////////////////////////////////////////////////////////////////////////////////

// Load debug symbols using DIA
static SymbolSessionPtr createSession(
    IDataProvider &DataProvider,
    ULONGLONG loadBase,
    const std::wstring &symbolFileName
)
{
    HRESULT hres;
    DiaDataSourcePtr dataSource;

    do {

        hres = dataSource.CoCreateInstance(__uuidof(DiaSource), NULL, CLSCTX_INPROC_SERVER);
        if ( S_OK == hres )
            break;

        hres = dataSource.CoCreateInstance(MSDIA12_CLASSGUID, NULL, CLSCTX_INPROC_SERVER);
        if (S_OK == hres)
            break;

        hres = dataSource.CoCreateInstance(MSDIA11_CLASSGUID, NULL, CLSCTX_INPROC_SERVER);
        if (S_OK == hres)
            break;

        HMODULE  hModule = NULL;

        if ( !GetModuleHandleEx(
                GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT, 
                (LPCTSTR)createSession,
                &hModule) )
                    throw DiaException(L"failed to load msdia library");

        DWORD fileNameSize = 0x1000;
        
        std::vector<wchar_t>  fileNameBuffer(fileNameSize);

        GetModuleFileNameW(hModule, &fileNameBuffer[0], fileNameSize);

        std::wstring  fileName(&fileNameBuffer[0], fileNameSize);

        size_t pos = fileName.find_last_of(L'\\');

        fileName.replace(pos, fileName.length() - pos, L"\\msdia120.dll");

        hres = NoRegCoCreate(fileName.c_str(), MSDIA12_CLASSGUID, __uuidof(IDiaDataSource), (void**)&dataSource);
        if (S_OK == hres)
            break;

        pos = fileName.find_last_of(L'\\');

        fileName.replace(pos, fileName.length() - pos, L"\\msdia110.dll");

        hres = NoRegCoCreate(fileName.c_str(), MSDIA11_CLASSGUID, __uuidof(IDiaDataSource), (void**)&dataSource);
        if (S_OK == hres)
            break;

        throw DiaException(L"Call ::CoCreateInstance", hres);

    } while( FALSE);

    hres = DataProvider.load(*dataSource);
    if ( S_OK != hres )
        throw DiaException(L"Call IDiaDataSource::loadDataXxx", hres);

    DiaSessionPtr _session;
    hres = dataSource->openSession(&_session);
    if ( S_OK != hres )
        throw DiaException(L"Call IDiaDataSource::openSession", hres);

    hres = _session->put_loadAddress(loadBase);
    if (S_OK != hres)
        throw DiaException(L"Call IDiaSession::put_loadAddress", hres);

    DiaSymbolPtr _globalScope;
    hres = _session->get_globalScope(&_globalScope);
    if ( S_OK != hres )
        throw DiaException(L"Call IDiaSymbol::get_globalScope", hres);

    return SymbolSessionPtr( new DiaSession( _session, _globalScope, symbolFileName ) );
}

//////////////////////////////////////////////////////////////////////////////////

// Load debug symbols using symbol file
class DataFromPdb : public IDataProvider {
    std::wstring m_filePath;

public:
    DataFromPdb(__in const std::wstring &filePath) {
        m_filePath = filePath;
    }

    virtual HRESULT load(__inout IDiaDataSource &dataSource) override {
        return dataSource.loadDataFromPdb( m_filePath.c_str() );
    }
};

// Access to executable file over RVA-callback
class ReadExeAtRVACallback : public IDiaReadExeAtRVACallback {
    ULONGLONG m_loadBase;
    int m_nRefCount;
    CComPtr< IDiaLoadCallback2 > m_diaLoadCallback2;

public:
    ReadExeAtRVACallback(
        __in ULONGLONG loadBase,
        __out std::wstring &openedSymbolFile
    )   : m_loadBase(loadBase), m_nRefCount(1)
        , m_diaLoadCallback2( new DiaLoadCallback2(&openedSymbolFile) ) 
    {
    }

    // IUnknown impl
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

        if (rid == __uuidof(IDiaReadExeAtRVACallback))
        {
            *ppUnk = this;
            return S_OK;
        }

        return m_diaLoadCallback2->QueryInterface(rid, ppUnk);
    }

    // IDiaReadExeAtRVACallback impl
    virtual HRESULT STDMETHODCALLTYPE ReadExecutableAtRVA( 
        /* [in] */ DWORD relativeVirtualAddress,
        /* [in] */ DWORD cbData,
        /* [out] */ DWORD *pcbData,
        /* [size_is][out] */ BYTE *pbData
    ) override 
    {
            if ( readMemoryUnsafe(
                m_loadBase + relativeVirtualAddress,
                pbData,
                cbData,
                FALSE,
                pcbData) )
                    return S_OK;

        return E_FAIL; //S_FALSE;
    }
};

//////////////////////////////////////////////////////////////////////////////////

struct SymSrvLoadHelper : boost::noncopyable
{
    static std::wstring g_symSrvDir;

    HMODULE m_loadedSymSrv;

    SymSrvLoadHelper() : m_loadedSymSrv(NULL)
    {
        if (g_symSrvDir.empty())
            return;

        if (::GetModuleHandle(_T("symsrv.dll")))
        {
            // already loaded
            return;
        }

        std::wstring symSrvFullPath = g_symSrvDir;
        if (L'\\' != *symSrvFullPath.rbegin())
            symSrvFullPath += L"\\";
        symSrvFullPath += L"symsrv.dll";

        m_loadedSymSrv = ::LoadLibraryW( symSrvFullPath.c_str() );
    }

    ~SymSrvLoadHelper()
    {
        if (m_loadedSymSrv)
            ::FreeLibrary(m_loadedSymSrv);
    }
};
std::wstring SymSrvLoadHelper::g_symSrvDir;

// Load debug symbols using ReadExeAtRVACallback
struct DataForExeByRva : IDataProvider {

    std::wstring m_openedSymbolFile;

    DataForExeByRva(
        __in ULONGLONG loadBase,
        __in const std::wstring &executable,
        __in const std::wstring &symbolSearchPath
    )   : m_loadBase(loadBase)
    {
        m_executable = executable;

        if (symbolSearchPath.empty())
            m_symbolSearchPath = getSymbolPath();
        else
            m_symbolSearchPath = symbolSearchPath;
    }

    virtual HRESULT load(__inout IDiaDataSource &dataSource) override {
        CComPtr< IUnknown > readExeAtRVACallback(new ReadExeAtRVACallback(m_loadBase, m_openedSymbolFile) );
        SymSrvLoadHelper symSrvLoadHelper;
        return 
            dataSource.loadDataForExe(
                m_executable.c_str(),
                m_symbolSearchPath.c_str(),
                readExeAtRVACallback);
    }

protected:
    ULONGLONG m_loadBase;

    std::wstring m_executable;
    std::wstring m_symbolSearchPath;
};

//////////////////////////////////////////////////////////////////////////////////

SymbolSessionPtr  loadSymbolFile(const std::wstring &filePath, ULONGLONG loadBase )
{
    DataFromPdb dataFromPdb(filePath);
    return createSession(dataFromPdb, loadBase, filePath);
}

//////////////////////////////////////////////////////////////////////////////////

SymbolSessionPtr loadSymbolFile(
    __in ULONGLONG loadBase,
    __in const std::wstring &executable,
    __in_opt std::wstring symbolSearchPath /*= std::string()*/
)
{
    DataForExeByRva dataForExeByRva(loadBase, executable, symbolSearchPath);

    SymbolSessionPtr symSession = createSession(dataForExeByRva, loadBase, dataForExeByRva.m_openedSymbolFile);

    return symSession;
}

//////////////////////////////////////////////////////////////////////////////////

void setSymSrvDir(const std::wstring &symSrvDir)
{
    SymSrvLoadHelper::g_symSrvDir = symSrvDir;
}

//////////////////////////////////////////////////////////////////////////////////

class NoSymbolScope : public Symbol
{
public:

    virtual SymbolPtrList findChildren(ULONG symTag, const std::wstring &name = L"", bool caseSensitive = FALSE)
    {
        throw SymbolException(L"symbol not found");
    }

    virtual SymbolPtrList findChildrenByRVA(unsigned long symTag, unsigned long rva)
    {
        NOT_IMPLEMENTED();
    }

    virtual ULONG getBaseType()
    {
        NOT_IMPLEMENTED();
    }

    virtual ULONG getBitPosition()
    {
        NOT_IMPLEMENTED();
    }

    virtual SymbolPtr getChildByIndex(ULONG _index)
    {
        throw SymbolException(L"symbol not found");
    }

    virtual SymbolPtr getChildByName(const std::wstring &_name)
    {
        throw SymbolException(L"symbol not found");
    }

    virtual size_t getChildCount()
    {
        return 0;
    }

    virtual size_t getChildCount(ULONG symTag)
    {
        return 0;
    }

    virtual size_t getCount()
    {
        NOT_IMPLEMENTED();
    }

    virtual ULONG getDataKind()
    {
        NOT_IMPLEMENTED();
    }

    virtual SymbolPtr getIndexType()
    {
        NOT_IMPLEMENTED();
    }

    virtual ULONG getLocType()
    {
        NOT_IMPLEMENTED();
    }

    virtual MachineTypes getMachineType()
    {
        return MachineTypes::machine_I386;
    }

    virtual std::wstring getName()
    {
        NOT_IMPLEMENTED();
    }

    virtual LONG getOffset()
    {
        NOT_IMPLEMENTED();
    }

    virtual ULONG getRva()
    {
        NOT_IMPLEMENTED();
    }

    virtual size_t getSize()
    {
        NOT_IMPLEMENTED();
    }

    virtual SymTags getSymTag()
    {
        NOT_IMPLEMENTED();
    }

    virtual SymbolPtr getType()
    {
        NOT_IMPLEMENTED();
    }

    virtual ULONG getUdtKind()
    {
        NOT_IMPLEMENTED();
    }

    virtual ULONGLONG getVa()
    {
        NOT_IMPLEMENTED();
    }

    virtual void getValue(NumVariant &vtValue)
    {
        NOT_IMPLEMENTED();
    }

    virtual ULONG getVirtualBaseDispIndex()
    {
        NOT_IMPLEMENTED();
    }

    virtual int getVirtualBasePointerOffset()
    {
        NOT_IMPLEMENTED();
    }

    virtual ULONG getVirtualBaseDispSize()
    {
        NOT_IMPLEMENTED();
    }

    virtual bool isBasicType()
    {
        NOT_IMPLEMENTED();
    }

    virtual bool isConstant()
    {
        NOT_IMPLEMENTED();
    }

    virtual bool isIndirectVirtualBaseClass()
    {
        NOT_IMPLEMENTED();
    }

    virtual bool isVirtualBaseClass()
    {
        NOT_IMPLEMENTED();
    }

    virtual bool isVirtual()
    {
        NOT_IMPLEMENTED();
    }

    virtual unsigned long getRegisterId()
    {
        NOT_IMPLEMENTED();
    }

    virtual ULONG getRegRealativeId()
    {
        NOT_IMPLEMENTED();
    }

    virtual SymbolPtr getObjectPointerType()
    {
        NOT_IMPLEMENTED();
    }

    virtual ULONG getCallingConvention()
    {
        NOT_IMPLEMENTED();
    }

    virtual SymbolPtr getClassParent()
    {
        NOT_IMPLEMENTED();
    }

    virtual SymbolPtr getVirtualTableShape()
    {
        NOT_IMPLEMENTED();
    }

    virtual unsigned long getVirtualBaseOffset()
    {
        NOT_IMPLEMENTED();
    }
};


class NoSymbolSession : public SymbolSession {

public:


    virtual SymbolPtr getSymbolScope() {
        return SymbolPtr(new NoSymbolScope());
    }

    virtual SymbolPtr findByRva(MEMOFFSET_32 rva, unsigned long symTag = SymTagNull, long* displacement = NULL) {
        throw SymbolException(L"Symbol not found");
    }

    virtual void getSourceLine(MEMOFFSET_64 offset, std::wstring &fileName, unsigned long &lineNo, long &displacement) {
        throw SymbolException(L"Source file not found");
    }

    virtual std::wstring getSymbolFileName() {
        return L"no symbols";
    }

};

SymbolSessionPtr loadNoSymbolSession()
{
    return SymbolSessionPtr(new NoSymbolSession());
}

//////////////////////////////////////////////////////////////////////////////////

}   // namespace kdlib
