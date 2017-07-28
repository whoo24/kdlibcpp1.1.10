#pragma once

#include <map>

#include <dia2.h>
#include <atlbase.h>

#include "kdlib/symengine.h"
#include "kdlib/exceptions.h"

namespace kdlib {

//////////////////////////////////////////////////////////////////////////////

typedef CComPtr< IDiaSymbol > DiaSymbolPtr;
typedef CComPtr< IDiaEnumSymbols > DiaEnumSymbolsPtr;
typedef CComPtr< IDiaDataSource > DiaDataSourcePtr;
typedef CComPtr< IDiaSession > DiaSessionPtr;
typedef CComPtr< IDiaEnumSymbolsByAddr > DiaEnumSymbolsByAddrPtr;
typedef CComPtr< IDiaEnumLineNumbers > DiaEnumLineNumbersPtr;
typedef CComPtr< IDiaLineNumber> DiaLineNumberPtr;
typedef CComPtr< IDiaSourceFile > DiaSourceFilePtr;

typedef std::map<ULONG, ULONG> DiaRegToRegRelativeBase;

//////////////////////////////////////////////////////////////////////////////

class DiaException : public SymbolException {
public:
    DiaException(const std::wstring &desc, HRESULT hres, IDiaSymbol *symbol = NULL)
        : SymbolException( makeFullDesc(desc, hres, symbol) )
        , m_hres(hres)
    {
    }

    DiaException(const std::wstring &desc)
        : SymbolException(descPrefix + desc)
        , m_hres(S_FALSE)
    {
    }

    HRESULT getRes() const {
        return m_hres;
    }
private:

    static const std::wstring descPrefix;

    static std::wstring makeFullDesc(const std::wstring &desc, HRESULT hres, IDiaSymbol *symbol = NULL);

    HRESULT m_hres;
};

////////////////////////////////////////////////////////////////////////////

class DiaSymbol : public Symbol {
public:
    DiaSymbol( DiaSymbolPtr &_symbol, MachineTypes machineType );

    static SymbolPtr fromGlobalScope( IDiaSymbol *_symbol );

    SymbolPtr getChildByName(const std::wstring &_name );

    ULONG getRva();

    SymbolPtrList findChildren(
        ULONG symTag,
        const std::wstring &name = L"",
        bool caseSensitive = FALSE
    );

    SymbolPtrList findChildrenByRVA(unsigned long symTag, unsigned long rva);

    size_t getSize();

    std::wstring getName();

    SymbolPtr getType();

    SymbolPtr getIndexType();

    SymTags getSymTag();

    ULONGLONG getVa();

    ULONG getLocType();

    LONG getOffset();

    size_t getCount();

    void getValue( NumVariant &vtValue );

    bool isBasicType();

    bool isIndirectVirtualBaseClass();

    bool isVirtualBaseClass();

    bool isVirtual();

    ULONG getBaseType();

    ULONG getBitPosition();

    ULONG getIndexId();

    ULONG getUdtKind();

    ULONG getDataKind();

    ULONG getRegisterId();

    virtual ULONG getRegRealativeId() override;

    virtual SymbolPtr getObjectPointerType() override;

    virtual ULONG getCallingConvention() override;

    virtual SymbolPtr getClassParent() override;

    MachineTypes getMachineType() {
        return m_machineType;
    }

    //SymbolPtr getChildByName(const std::string &_name);

    size_t getChildCount( ULONG symTag );

    size_t getChildCount() {
        return getChildCount(SymTagNull);
    }

    SymbolPtr getChildByIndex(ULONG symTag, ULONG _index );

    SymbolPtr getChildByIndex(ULONG _index ) {
        return getChildByIndex( SymTagNull, _index );
    }
    
    bool isConstant();

    int getVirtualBasePointerOffset();

    ULONG getVirtualBaseDispIndex();

    ULONG getVirtualBaseDispSize();

    void setLoadAddress( ULONGLONG baseAddress );

     SymbolPtr getVirtualTableShape();

    unsigned long getVirtualBaseOffset();

public:
    typedef std::pair<ULONG, const wchar_t *> ValueNameEntry;
    static const ValueNameEntry basicTypeName[];
    static const size_t cntBasicTypeName;

    static std::string getBasicTypeName( ULONG basicType );

protected:

    ULONG getRegRealativeIdImpl(const DiaRegToRegRelativeBase &DiaRegToRegRelative);

    bool isUndecorated(const std::wstring &undecName);

    template <typename TRet>
    TRet callSymbolT(
        HRESULT(STDMETHODCALLTYPE IDiaSymbol::*method)(TRet *),
        const wchar_t *methodName
    )
    {
        TRet retValue;
        HRESULT hres = (m_symbol->*method)(&retValue);
        if (S_OK != hres)
            throw DiaException(std::wstring(L"Call IDiaSymbol::") + methodName, hres, m_symbol);

        return retValue;
    }

    DiaSymbolPtr m_symbol;

    MachineTypes m_machineType;
};

////////////////////////////////////////////////////////////////////////////

class DiaSession : public SymbolSession
{
public:

    DiaSession( IDiaSession* session, IDiaSymbol *globalScope, const std::wstring symbolFile ) :
        m_globalScope( globalScope ),
        m_globalSymbol( DiaSymbol::fromGlobalScope( globalScope ) ),
        m_session( session ),
        m_symbolFileName( symbolFile )
        {}

    virtual SymbolPtr getSymbolScope() {
        return m_globalSymbol;
    }

    virtual SymbolPtr findByRva( ULONG rva, ULONG symTag = SymTagNull, LONG* displacement = NULL );

    virtual void getSourceLine( ULONG64 offset, std::wstring &fileName, ULONG &lineNo, LONG &displacement );

    virtual std::wstring getSymbolFileName() {
        return m_symbolFileName;
    }

private:

    ULONG findRvaByName( const std::string &name );

    DiaSymbolPtr    m_globalScope;
    SymbolPtr       m_globalSymbol;
    DiaSessionPtr   m_session;
    std::wstring     m_symbolFileName;

};

////////////////////////////////////////////////////////////////////////////

} // end kdlib namespace

