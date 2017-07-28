#include "stdafx.h"

#include <Windows.h>
#include <comutil.h>
#include <DbgHelp.h>

#include <boost/multi_index_container.hpp>
#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/tag.hpp>

#include <boost/algorithm/string/replace.hpp>
#include <boost/regex.hpp>

#include "kdlib/symengine.h"
#include "kdlib/exceptions.h"
#include "kdlib/memaccess.h"

namespace bmi = boost::multi_index;

///////////////////////////////////////////////////////////////////////////////

namespace {

    
    void escapeRegex(std::wstring &regex)
    {
        boost::replace_all(regex, L"\\", L"\\\\");
        boost::replace_all(regex, L"^", L"\\^");
        boost::replace_all(regex, L".", L"\\.");
        boost::replace_all(regex, L"$", L"\\$");
        boost::replace_all(regex, L"|", L"\\|");
        boost::replace_all(regex, L"(", L"\\(");
        boost::replace_all(regex, L")", L"\\)");
        boost::replace_all(regex, L"[", L"\\[");
        boost::replace_all(regex, L"]", L"\\]");
        boost::replace_all(regex, L"*", L"\\*");
        boost::replace_all(regex, L"+", L"\\+");
        boost::replace_all(regex, L"?", L"\\?");
        boost::replace_all(regex, L"/", L"\\/");
    }

    bool fnmatch(const std::wstring&  str, const std::wstring& match)
    {
        std::wstring  reStr = match;
        escapeRegex(reStr);

        boost::replace_all(reStr, L"\\?", L".");
        boost::replace_all(reStr, L"\\*", L".*");

        boost::wregex pattern(reStr, boost::regex::normal);

        return boost::regex_match(str, pattern);
    }
}


namespace kdlib {

///////////////////////////////////////////////////////////////////////////////

class ExportSymbolBase : public Symbol
{

    virtual SymbolPtrList findChildren( ULONG symTag, const std::wstring &name = L"", bool caseSensitive = FALSE )
    {
        NOT_IMPLEMENTED();
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

    virtual SymbolPtr getChildByIndex(ULONG _index )
    {
        NOT_IMPLEMENTED();
    }

    virtual SymbolPtr getChildByName(const std::wstring &_name )
    {
        NOT_IMPLEMENTED();
    }

    virtual size_t getChildCount()
    {
        NOT_IMPLEMENTED();
    }

    virtual size_t getChildCount(ULONG symTag )
    {
        NOT_IMPLEMENTED();
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
        return LocIsStatic;
    }
    
    virtual MachineTypes getMachineType()
    {
        NOT_IMPLEMENTED();
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
        return SymTagPublicSymbol;
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

    virtual void getValue( NumVariant &vtValue)
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

///////////////////////////////////////////////////////////////////////////////

class ExportSymbol : public ExportSymbolBase
{
public:

    ExportSymbol(const std::wstring &name, ULONG rva, ULONGLONG moduleBase, MachineTypes machineType) :
      m_name(name),
      m_rva(rva),
      m_moduleBase(moduleBase),
      m_machineType(machineType)
      {}


private:

    virtual std::wstring getName()
    {
        return m_name;
    }

    virtual ULONG getRva()
    {
        return m_rva;
    }

    virtual ULONGLONG getVa()
    {
        return m_moduleBase + m_rva;
    }

    virtual MachineTypes getMachineType()
    {
        return m_machineType;
    }


    std::wstring  m_name;
    ULONG  m_rva;
    ULONGLONG  m_moduleBase;
    MachineTypes m_machineType;

};

///////////////////////////////////////////////////////////////////////////////

class FunctionMap
{
public:
    typedef ULONG           FunctionAddress;
    typedef LONG            FunctionOffset;
    typedef std::wstring    FunctionName;

    bool add(const FunctionName& name, const FunctionAddress address)
    {
        typedef std::pair<NameIndex::iterator, bool> InsertionResult;

        const Entry entry(name, address);
        InsertionResult result = GetNameIndex().insert(entry);

        //NB: true if item inserted, false - if already exists
       return result.second;
    }

    void clear()
    {
        GetNameIndex().clear();
    }

    bool findByName(const FunctionName& name, FunctionAddress& address) const
    {
        NameIndex::const_iterator it = GetNameIndex().find(name);
        if (GetNameIndex().end() == it)
            return false;
        
        address = (*it).Address;
        return true;
    }

    bool findByAddress(const FunctionAddress address, FunctionName& name, FunctionAddress& funcAddress, FunctionOffset& offset ) const
    {
        if (GetAddressIndex().empty())
            return false;

        AddressIndex::const_iterator it = GetAddressIndex().lower_bound(address);
        if (GetAddressIndex().end() == it)
        {
            name = (*GetAddressIndex().rbegin()).Name;
            offset = static_cast<FunctionOffset>(address - (*GetAddressIndex().rbegin()).Address);
            funcAddress = (*GetAddressIndex().rbegin()).Address;
        }
        else
        {
           if ((address < (*it).Address) && GetAddressIndex().begin() != it)
                --it;

            name = (*it).Name;
            offset = static_cast<FunctionOffset>(address - (*it).Address);
            funcAddress = (*it).Address;
        }
        return true;
    }

    size_t size() const
    {
        return m_storage.size();
    }

    bool findByIndex(const size_t index, FunctionName& name, FunctionAddress& funcAddress) const
    {
        if (index >= size())
            return false;

        NameIndex::const_iterator it = GetNameIndex().begin();
        std::advance(it, index);

        name = (*it).Name;
        funcAddress = (*it).Address;

        return true;
    }

private:
    struct Entry
    {
        Entry(const FunctionName& name, const FunctionAddress address)
            : Name(name)
            , Address(address)
        {}

        FunctionName    Name;
        FunctionAddress Address;
    };

    struct ByAddress {};
    struct ByName {};

    typedef bmi::multi_index_container<
        Entry,
        bmi::indexed_by<
            bmi::ordered_unique<
                bmi::tag<ByAddress>,
                bmi::member<Entry, FunctionAddress, &Entry::Address>
            >,
            bmi::hashed_unique<
                bmi::tag<ByName>,
                bmi::member<Entry, FunctionName, &Entry::Name>
            >
        >
    > Container;

    typedef Container::index<ByAddress>::type AddressIndex;
    typedef Container::index<ByName>::type NameIndex;

private:
    const AddressIndex& GetAddressIndex() const
    {
        return m_storage.get<ByAddress>();
    }

    AddressIndex& GetAddressIndex()
    {
        return m_storage.get<ByAddress>();
    }

    const NameIndex& GetNameIndex() const
    {
        return m_storage.get<ByName>();
    }

    NameIndex& GetNameIndex()
    {
        return m_storage.get<ByName>();
    }

private:
    Container m_storage;
};

///////////////////////////////////////////////////////////////////////////////

class ExportSymbolDir;
typedef boost::shared_ptr<ExportSymbolDir> ExportSymbolDirPtr;

class ExportSymbolDir : public ExportSymbolBase
{
public:

    static ExportSymbolDirPtr getExportSymbolDir( ULONGLONG moduleBase )
    {
        return ExportSymbolDirPtr( new ExportSymbolDir(moduleBase) );
    }

    SymbolPtr findByRva( ULONG rva, ULONG symTag, LONG* displacement )
    {
        std::wstring name;
        ULONG address;
        LONG offset;

        if ( !m_exportMap.findByAddress( rva, name, address, offset ) )
            throw SymbolException( L"symbol can not be found by rva");

        if ( displacement )
            *displacement = offset;

        return SymbolPtr(new ExportSymbol(name, address, m_moduleBase, getMachineType()));
    }

    virtual MachineTypes getMachineType()
    {
        switch (m_machineType)
        {
        case IMAGE_FILE_MACHINE_I386: return machine_I386;
        case IMAGE_FILE_MACHINE_AMD64: return machine_AMD64;
        }

        throw SymbolException(L"Unkonw machine type");
    }

    virtual SymbolPtr getChildByName(const std::wstring &name)
    {
        FunctionMap::FunctionAddress addr;

        if (!m_exportMap.findByName(name, addr))
            throw SymbolException(name + L" symbol is not found");

        return SymbolPtr(new ExportSymbol(name, addr, m_moduleBase, getMachineType()));
    }

    virtual SymbolPtrList findChildren(ULONG symTag, const std::wstring &mask = L"", bool caseSensitive = FALSE)
    {
        SymbolPtrList  symLst;

        if (symTag == SymTagPublicSymbol)
        {
            for (size_t i = 0; i < m_exportMap.size(); ++i)
            {
                std::wstring funcName;
                ULONG address;
                m_exportMap.findByIndex(i, funcName, address);

                if (mask.empty() || mask == L"*" || fnmatch(funcName, mask) )
                    symLst.push_back(SymbolPtr(new ExportSymbol(funcName, address, m_moduleBase, getMachineType())));
            }
        }

        return symLst;
    }

private:

    ExportSymbolDir( ULONGLONG moduleBase )
    {
        ULONG64  ntHeaderOffset = moduleBase + ptrDWord( moduleBase + 0x3c );

        IMAGE_NT_HEADERS  ntHeaders;
        readMemory( ntHeaderOffset, &ntHeaders, sizeof(ntHeaders) );

        m_machineType = ntHeaders.FileHeader.Machine;

        m_moduleBase = moduleBase;

        if ( m_machineType == IMAGE_FILE_MACHINE_I386 )
        {
            GetExport<IMAGE_NT_HEADERS32>( moduleBase );
            return;
        }
        
        if ( m_machineType == IMAGE_FILE_MACHINE_AMD64 )
        {
            GetExport<IMAGE_NT_HEADERS64>( moduleBase );
            return;
        }

        throw SymbolException( L"Unkonw machine type");
    }

    template<typename NTHEADERT>
    void  GetExport( ULONG64 moduleBase )
    {
        ULONG64  ntHeaderOffset = moduleBase + ptrDWord( moduleBase + 0x3c );

        NTHEADERT  ntHeader;
        readMemory( ntHeaderOffset, &ntHeader, sizeof(ntHeader) );
    
        if ( ntHeader.OptionalHeader.DataDirectory[0].Size == 0 )
            return;

        ULONG64  exportDirOffset = moduleBase + ntHeader.OptionalHeader.DataDirectory[0].VirtualAddress;

        ULONG  namesCount = (ULONG) ptrDWord( exportDirOffset + 0x18 );
        ULONG64  funcRvaOffset = moduleBase + ptrDWord( exportDirOffset + 0x1C );
        ULONG64  namesOffset = moduleBase + ptrDWord( exportDirOffset + 0x20 ); 
        ULONG64  ordinalsOffset = moduleBase + ptrDWord( exportDirOffset + 0x24 );

        for ( ULONG i = 0; i < namesCount; ++i ) 
        {
            std::string  exportName = loadCStr( moduleBase + ptrDWord( namesOffset + 4 * i ) );
            USHORT  exportOrdinal = (USHORT)ptrWord( ordinalsOffset + 2 * i );
            ULONG rva = (ULONG)ptrDWord( funcRvaOffset + 4 * exportOrdinal );

            std::vector<char> undecorBuffer(1000);
            DWORD  undecorLength = UnDecorateSymbolName(exportName.c_str(), &undecorBuffer[0], (DWORD)undecorBuffer.size(), UNDNAME_NAME_ONLY);
            if (undecorLength > 0)
                exportName = std::string(&undecorBuffer[0], undecorLength);

            std::wstring wideExportNamr = _bstr_t( exportName.c_str() );

            m_exportMap.add( wideExportNamr, rva );
        }
    }

    FunctionMap m_exportMap;

    ULONG  m_machineType;

    ULONGLONG  m_moduleBase;
};

///////////////////////////////////////////////////////////////////////////////

class ExportSession : public SymbolSession
{
public:

    ExportSession( ULONGLONG moduleBase ) :
      m_moduleBase( moduleBase )
      {
          m_exportDir = ExportSymbolDir::getExportSymbolDir(moduleBase);
      }

    virtual SymbolPtr getSymbolScope() {
        return SymbolPtr( m_exportDir );
    }

    virtual SymbolPtr findByRva( ULONG rva, ULONG symTag = SymTagNull, LONG* displacement = NULL ) {
        return m_exportDir->findByRva( rva, symTag, displacement );
    }

    virtual void getSourceLine( ULONG64 offset, std::wstring &fileName, ULONG &lineNo, LONG &displacement ) 
    {
        throw SymbolException( L"there is no source file" );
    }

    virtual std::wstring getSymbolFileName() {
        return std::wstring(L"export symbols");
    }

private:

    ULONGLONG           m_moduleBase;

    ExportSymbolDirPtr   m_exportDir;
};

///////////////////////////////////////////////////////////////////////////////

SymbolSessionPtr loadSymbolFromExports(ULONGLONG loadBase)
{
    return SymbolSessionPtr( new ExportSession( loadBase ) );
}

///////////////////////////////////////////////////////////////////////////////

}; // end kdlib namespace
