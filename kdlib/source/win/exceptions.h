#pragma once

#include <sstream>

#include <Windows.h>

#include "kdlib/exceptions.h"

namespace  kdlib {

class DbgEngException : public DbgWideException
{
public:

    DbgEngException( const std::wstring  &methodName, HRESULT hres ) :
        DbgWideException( buildHresultDesc( methodName, hres ).c_str() )
        {}

private:

    std::wstring buildHresultDesc( const std::wstring  &methodName, HRESULT hres ) 
    {
        std::wstringstream sstr;
        sstr << L"Call " << methodName << L" failed\n";
        sstr << L"HRESULT 0x" << std::hex << hres;
        return sstr.str();
    }

};


class Win32Exception : public DbgException
{
public: 
    Win32Exception( const std::string  &str ) :
        DbgException( buildExcDesc(str) )
    {}

private:

    static std::string buildExcDesc( const std::string  &str )
    {
        DWORD  lastErr = GetLastError();

        std::stringstream  sstr;
        sstr << str << std::endl;
        sstr << "Win32 error: 0x" << std::hex << lastErr;
        return sstr.str();

    }
};


} // kdlib namespace end 


