///////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "kdlib/dbgengine.h"
#include "kdlib/memaccess.h"
#include "win/exceptions.h"
#include "win/dbgmgr.h"

#include "autoswitch.h"

#include <vector>
#include <iomanip>

namespace  kdlib {

///////////////////////////////////////////////////////////////////////////////

namespace
{

///////////////////////////////////////////////////////////////////////////////

struct ModuleParameters : public DEBUG_MODULE_PARAMETERS
{
    ModuleParameters(MEMOFFSET_64 baseOffset) {
        HRESULT hres = g_dbgMgr->symbols->GetModuleParameters( 1, &baseOffset, 0, this );
        if ( FAILED( hres ) )
            throw DbgEngException( L"IDebugSymbol::GetModuleParameters", hres );
    }
};

///////////////////////////////////////////////////////////////////////////////

}

///////////////////////////////////////////////////////////////////////////////

MEMOFFSET_64 findModuleBase( const std::wstring &moduleName )
{
    HRESULT     hres;
    ULONG64     base;

    hres = g_dbgMgr->symbols->GetModuleByModuleNameWide( moduleName.c_str(), 0, NULL, &base );
    if ( FAILED( hres ) )
        throw DbgEngException( L"IDebugSymbol::GetModuleByModuleName", hres ); 

    return base;
}

///////////////////////////////////////////////////////////////////////////////////

MEMOFFSET_64 findModuleBase( MEMOFFSET_64 offset )
{
    HRESULT     hres;
    ULONG64     base;
    ULONG       moduleIndex;

    hres = g_dbgMgr->symbols->GetModuleByOffset( offset, 0, &moduleIndex, &base );
    if ( FAILED( hres ) )
        throw DbgEngException( L"IDebugSymbol::GetModuleByOffset", hres );

    return base;
}

///////////////////////////////////////////////////////////////////////////////

unsigned long getNumberModules()
{
    ULONG  loaded;
    ULONG  unloaded;
 
    HRESULT  hres = g_dbgMgr->symbols->GetNumberModules( &loaded, &unloaded );
    if ( FAILED( hres ) )
        throw DbgEngException( L"IDebugSymbol::GetNumberModules", hres );

    return loaded;
}

///////////////////////////////////////////////////////////////////////////////

std::vector<MEMOFFSET_64> getModuleBasesList()
{
    size_t num = getNumberModules();

    std::vector<MEMOFFSET_64>  moduleList;

    for ( ULONG i = 0; i < num; ++i )
    {
        ULONG64  base = 0;
        HRESULT  hres = g_dbgMgr->symbols->GetModuleByIndex( i, &base );
        if ( FAILED( hres ) )
            throw DbgEngException( L"IDebugSymbol::GetModuleByIndex", hres );

         moduleList.push_back( base );
    }

    return moduleList;
}

///////////////////////////////////////////////////////////////////////////////

MEMOFFSET_64 getModuleOffsetByIndex(unsigned long index)
{
    HRESULT  hres;

    if (index >= getNumberModules())
        throw IndexException(index);

    ULONG64  base = 0;
    hres = g_dbgMgr->symbols->GetModuleByIndex(index, &base);
    if (FAILED(hres))
        throw DbgEngException(L"IDebugSymbol::GetModuleByIndex", hres);

    return MEMOFFSET_64(base);
}

///////////////////////////////////////////////////////////////////////////////

//MEMOFFSET_64 findModuleBySymbol( const std::wstring &symbolName )
//{
//    HRESULT     hres;
//    ULONG64     base;
//
//    hres = g_dbgMgr->symbols->GetSymbolModuleWide( ( std::wstring(L"!") + symbolName ).c_str(), &base );
//    if ( FAILED( hres ) )
//    {
//        std::wstringstream   sstr;
//        sstr << L"failed to find module for symbol: " << symbolName;
//        throw SymbolException( sstr.str() );
//    }
//
//    return base;
//}

///////////////////////////////////////////////////////////////////////////////

std::wstring getModuleName( MEMOFFSET_64 baseOffset )
{
    baseOffset = addr64(baseOffset);

    HRESULT  hres;

    std::vector<wchar_t>  moduleName(0x100);

    hres = g_dbgMgr->symbols->GetModuleNameStringWide( 
        DEBUG_MODNAME_MODULE,
        DEBUG_ANY_ID,
        baseOffset,
        &moduleName[0],
        static_cast<ULONG>(moduleName.size()),
        NULL );

    if ( FAILED( hres ) )
        throw DbgEngException( L"IDebugSymbol::GetModuleNameString", hres );

    return std::wstring( &moduleName[0] );
}

///////////////////////////////////////////////////////////////////////////////

std::wstring getModuleImageName( MEMOFFSET_64 baseOffset )
{
    baseOffset = addr64(baseOffset);

    HRESULT  hres;
    
    std::vector<wchar_t>  moduleName(0x100);

    hres = g_dbgMgr->symbols->GetModuleNameStringWide( 
        DEBUG_MODNAME_IMAGE,
        DEBUG_ANY_ID,
        baseOffset,
        &moduleName[0],
        static_cast<ULONG>(moduleName.size()),
        NULL );

    if ( FAILED( hres ) )
        throw DbgEngException( L"IDebugSymbol::GetModuleNameString", hres );

    return std::wstring( &moduleName[0] );
}

///////////////////////////////////////////////////////////////////////////////

MEMOFFSET_32 getModuleSize( MEMOFFSET_64 baseOffset )
{
    baseOffset = addr64(baseOffset);
    return ModuleParameters(baseOffset).Size;
}

///////////////////////////////////////////////////////////////////////////////

unsigned long getModuleTimeStamp( MEMOFFSET_64 baseOffset )
{
    baseOffset = addr64(baseOffset);
    return ModuleParameters(baseOffset).TimeDateStamp;
}

///////////////////////////////////////////////////////////////////////////////

unsigned long getModuleCheckSum( MEMOFFSET_64 baseOffset )
{
    baseOffset = addr64(baseOffset);
    return ModuleParameters(baseOffset).Checksum;
}

///////////////////////////////////////////////////////////////////////////////

bool isModuleUnloaded( MEMOFFSET_64 baseOffset )
{
    baseOffset = addr64(baseOffset);
    return !!(ModuleParameters(baseOffset).Flags & DEBUG_MODULE_UNLOADED);
}

///////////////////////////////////////////////////////////////////////////////

bool isModuleUserMode( MEMOFFSET_64 baseOffset )
{
    baseOffset = addr64(baseOffset);
    return !!(ModuleParameters(baseOffset).Flags & DEBUG_MODULE_USER_MODE);
}

///////////////////////////////////////////////////////////////////////////////

std::wstring getModuleSymbolFileName( MEMOFFSET_64 baseOffset )
{
    baseOffset = addr64(baseOffset);

    HRESULT  hres;
    IMAGEHLP_MODULEW64   moduleInfo = {};

    hres = g_dbgMgr->advanced->GetSymbolInformation(
        DEBUG_SYMINFO_IMAGEHLP_MODULEW64,
        baseOffset,
        0,
        &moduleInfo,
        sizeof(moduleInfo),
        NULL,
        NULL,
        0,
        NULL );

    if ( FAILED( hres ) )
        throw DbgEngException( L"IDebugAdvanced2::GetSymbolInformation", hres );

    if (!*moduleInfo.LoadedPdbName)
    {
        ContextAutoRestore  contextRestore;

        std::wstringstream sstr;
        sstr << L"/f \"" << moduleInfo.ImageName << L"\"";

        hres = g_dbgMgr->symbols->ReloadWide( sstr.str().c_str() );
        if ( FAILED( hres ) )
            throw DbgEngException( L"IDebugSymbols::Reload", hres );

        hres = g_dbgMgr->advanced->GetSymbolInformation(
            DEBUG_SYMINFO_IMAGEHLP_MODULEW64,
            baseOffset,
            0,
            &moduleInfo,
            sizeof(moduleInfo),
            NULL,
            NULL,
            0,
            NULL );

        if ( FAILED( hres ) )
            throw DbgEngException( L"IDebugAdvanced2::GetSymbolInformation", hres );
    }

    return std::wstring( moduleInfo.LoadedPdbName );
}

///////////////////////////////////////////////////////////////////////////////

std::string getModuleVersionInfo( MEMOFFSET_64 baseOffset, const std::string &value )
{
    baseOffset = addr64(baseOffset);

    struct LANGANDCODEPAGE {
        WORD wLanguage;
        WORD wCodePage;
    };

    HRESULT  hres;

    ULONG codePagesSize = 0;

    hres = g_dbgMgr->symbols->GetModuleVersionInformation( 
        DEBUG_ANY_ID,
        baseOffset,
        "\\VarFileInfo\\Translation",
        NULL,
        0,
        &codePagesSize );

    if ( FAILED( hres ) )
         throw DbgEngException( L"IDebugSymbol2::GetModuleVersionInformation", hres ); 

    size_t codePageNum = codePagesSize / sizeof(LANGANDCODEPAGE);

    std::vector<LANGANDCODEPAGE> codePages(codePageNum);

    hres = g_dbgMgr->symbols->GetModuleVersionInformation( 
        DEBUG_ANY_ID,
        baseOffset,
        "\\VarFileInfo\\Translation",
        &codePages[0],
        codePagesSize,
        NULL );

    if ( FAILED( hres ) )
         throw DbgEngException( L"IDebugSymbol2::GetModuleVersionInformation", hres );

    ULONG productNameLength = 0;

    std::stringstream  sstr;
    sstr << "\\StringFileInfo\\" << std::hex 
            << std::setw(4) << std::setfill('0') <<  codePages[0].wLanguage 
            << std::setw(4) << std::setfill('0') << codePages[0].wCodePage 
            << "\\" << value;

    ULONG  valueLength = 0;

    g_dbgMgr->symbols->GetModuleVersionInformation( 
        DEBUG_ANY_ID,
        baseOffset,
        sstr.str().c_str(),
        NULL,
        0,
        &valueLength );

    if (!valueLength)
        return "";

    std::vector<char>  valueStr(valueLength);

    hres = g_dbgMgr->symbols->GetModuleVersionInformation( 
        DEBUG_ANY_ID,
        baseOffset,
        sstr.str().c_str(),
        &valueStr[0],
        valueLength,
        NULL );

    if ( hres == S_OK )
        return std::string( &valueStr[0] );

   return "";
}

///////////////////////////////////////////////////////////////////////////////

void getModuleFixedFileInfo( MEMOFFSET_64 baseOffset, FixedFileInfo &fixedFileInfo )
{
    baseOffset = addr64(baseOffset);

    VS_FIXEDFILEINFO Pod;
    HRESULT hres = 
        g_dbgMgr->symbols->GetModuleVersionInformation( 
            DEBUG_ANY_ID,
            baseOffset,
            "\\",
            &Pod,
            sizeof(Pod),
            NULL );
    if ( hres != S_OK )
        throw DbgEngException( L"IDebugSymbols::GetModuleVersionInformation", hres );

    fixedFileInfo.Signature = Pod.dwSignature;
    fixedFileInfo.StrucVersion = Pod.dwStrucVersion;
    fixedFileInfo.FileVersionMS = Pod.dwFileVersionMS;
    fixedFileInfo.FileVersionLS = Pod.dwFileVersionLS;
    fixedFileInfo.ProductVersionMS = Pod.dwProductVersionMS;
    fixedFileInfo.ProductVersionLS = Pod.dwProductVersionLS;
    fixedFileInfo.FileFlagsMask = Pod.dwFileFlagsMask;
    fixedFileInfo.FileFlags = Pod.dwFileFlags;
    fixedFileInfo.FileOS = Pod.dwFileOS;
    fixedFileInfo.FileType = Pod.dwFileType;
    fixedFileInfo.FileSubtype = Pod.dwFileSubtype;
    fixedFileInfo.FileDateMS = Pod.dwFileDateMS;
    fixedFileInfo.FileDateLS = Pod.dwFileDateLS;
}

///////////////////////////////////////////////////////////////////////////////

} // kdlib namespace end 

