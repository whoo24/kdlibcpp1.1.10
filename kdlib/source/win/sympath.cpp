// 
// Win-[DbgEng]: Debug symbols path
// 

////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include <boost/smart_ptr/scoped_array.hpp>

#include "kdlib/dbgengine.h"

#include "win/exceptions.h"
#include "win/dbgmgr.h"

////////////////////////////////////////////////////////////////////////////////

namespace kdlib {

////////////////////////////////////////////////////////////////////////////////

std::wstring getSymbolPath()
{
    ULONG retSymPathChars = 0;
    g_dbgMgr->symbols->GetSymbolPath(NULL, 0, &retSymPathChars);
    if (!retSymPathChars)
        return std::wstring();

    const ULONG symPathChars = retSymPathChars + 1;
    boost::scoped_array<wchar_t> symPath( new wchar_t [symPathChars] );
    RtlZeroMemory(symPath.get(), sizeof(wchar_t) * symPathChars);
    HRESULT hres = 
        g_dbgMgr->symbols->GetSymbolPathWide(symPath.get(), symPathChars, &retSymPathChars);
    if (S_OK != hres)
        throw DbgEngException(L"IDebugSymbols::GetSymbolPath", hres);

    return std::wstring( symPath.get() );
}

////////////////////////////////////////////////////////////////////////////////

void setSymbolPath(const std::wstring &symPath)
{
    g_dbgMgr->symbols->SetSymbolPathWide(symPath.c_str());
}

////////////////////////////////////////////////////////////////////////////////

void appendSymbolPath(const std::wstring &symPath)
{
    g_dbgMgr->symbols->AppendSymbolPathWide(symPath.c_str());
}

////////////////////////////////////////////////////////////////////////////////

std::wstring getSrcPath()
{
    ULONG retSrcPathChars = 0;
    g_dbgMgr->symbols->GetSourcePath(NULL, 0, &retSrcPathChars);
    if (!retSrcPathChars)
        return std::wstring();

    const ULONG srcPathChars = retSrcPathChars + 1;
    boost::scoped_array<wchar_t> srcPath(new wchar_t[srcPathChars]);
    RtlZeroMemory(srcPath.get(), sizeof(wchar_t) * srcPathChars);
   
    HRESULT hres =
        g_dbgMgr->symbols->GetSourcePathWide(srcPath.get(), srcPathChars, &retSrcPathChars);
    
    if (S_OK != hres)
        throw DbgEngException(L"IDebugSymbols::GetSourcePathWide", hres);

    return std::wstring(srcPath.get());
}

////////////////////////////////////////////////////////////////////////////////

void setSrcPath(const std::wstring &srcPath)
{
    HRESULT  hres;

    hres = g_dbgMgr->symbols->SetSourcePathWide(srcPath.c_str());
    if (FAILED(hres))
        throw DbgEngException(L"IDebugSymbols::SetSourcePathWide", hres);
}

////////////////////////////////////////////////////////////////////////////////

void appendSrcPath(const std::wstring &srcPath)
{
    HRESULT  hres;

    hres = g_dbgMgr->symbols->AppendSourcePathWide(srcPath.c_str());
    if (FAILED(hres))
        throw DbgEngException(L"IDebugSymbols::AppendSourcePathWide", hres);
}

////////////////////////////////////////////////////////////////////////////////

}   // namespace kdlib

////////////////////////////////////////////////////////////////////////////////
