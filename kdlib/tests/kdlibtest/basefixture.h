#pragma once

#include <Windows.h>

#include "gtest/gtest.h"

#include "kdlib/dbgengine.h"
#include "kdlib/exceptions.h"


class BaseFixture : public ::testing::Test
{
public:

    kdlib::PROCESS_ID  StartTargetappWithParam( const std::wstring& cmdline ) 
    {
        std::vector<wchar_t>   buffer(0x1000);
        DWORD   len = GetCurrentDirectory( static_cast<DWORD>(buffer.size()), &buffer[0] );

        std::wstring  path = std::wstring( &buffer[0], len );
        path += L"\\targetapp.exe"; 

        buffer.clear();
        buffer.insert( buffer.begin(), path.begin(), path.end() );
        buffer.insert( buffer.end(), L' ' );
        buffer.insert( buffer.end(), cmdline.begin(), cmdline.end() );
        buffer.insert( buffer.end(), 0);

        STARTUPINFO   startupInfo = {0};
        startupInfo.cb = sizeof(startupInfo);

        PROCESS_INFORMATION  processInfo = {0};

        BOOL result = CreateProcessW(
            path.c_str(),
            &buffer[0],
            NULL,
            NULL,
            FALSE,
            CREATE_NO_WINDOW,
            NULL,
            NULL,
            &startupInfo,
            &processInfo );

        CloseHandle( processInfo.hProcess );
        CloseHandle( processInfo.hThread );

        return processInfo.dwProcessId;
    }




    virtual void TearDown() {
        try {
            kdlib::terminateAllProcesses();
        } catch(kdlib::DbgException&)
        {}
    }
};
