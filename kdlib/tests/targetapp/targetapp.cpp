// targetapp.cpp : Defines the entry point for the console application.
//

//Русские комментарии рулят!!!!

#include "stdafx.h"

#include <string>
#include <vector>

#include <process.h>
#include <windows.h>

#include "test/testfunc.h"
#include "test/testvars.h"

int breakOnRun();
int breakpointTestRun();
int memTestRun();
int stackTestRun();
int loadUnloadModuleRun();
int startChildProcess();
void __cdecl sleepThread(void*);


int _tmain(int argc, _TCHAR* argv[])
{
    FuncWithStructCdecl(shortcomplex{0,0});
    FuncWithStructStd(shortcomplex{0,0});
    FuncWithStructFast(shortcomplex{0,0});

    FuncWithStructCdecl(intcomplex{0,0});
    FuncWithStructStd(intcomplex{0,0});
    FuncWithStructFast(intcomplex{0,0});

    FuncWithStructCdecl(longcomplex{0,0});
    FuncWithStructStd(longcomplex{0,0});
    FuncWithStructFast(longcomplex{0,0});

    g_classChild.childMethod(10);
    g_classChild.overloadMethod(10);
    g_classChild.overloadMethod(10,20);

    g_stdString.find_first_of('S');

    if ( argc <= 1 )
    {
        return breakOnRun();
    }

    std::wstring testGroup = std::wstring( argv[1] );

    if ( testGroup == L"memtest" )
        return breakOnRun();

    if ( testGroup == L"typetest" )
        return breakOnRun();

    if ( testGroup == L"typedvartest" )
        return breakOnRun();

    if ( testGroup == L"breakhandlertest" )
        return breakpointTestRun();

    if ( testGroup == L"stacktest" )
        return stackTestRun();

    if ( testGroup == L"loadunloadmodule" )
        return loadUnloadModuleRun();

    if ( testGroup == L"processtest" )
    {
        Sleep( INFINITE );
        return 0;
    }

    if (testGroup == L"multithread")
    {
        LONG  startThreadCount = 0;
        for (int i = 0; i < 4; ++i)
        {
            _beginthread(sleepThread, 0, &startThreadCount);
        }
        
        while (startThreadCount < 4)
            Sleep(100);

        __debugbreak();

        Sleep(INFINITE);

        return 0;
    }

    if ( testGroup == L"childprocess" )
        return startChildProcess();

    return breakOnRun();
}

int breakOnRun()
{
    __debugbreak();
    return 0;
}

int breakpointTestRun()
{
    __debugbreak();

    CdeclFunc( 10, 10.0f );

    return 0;
}


class stackTestClass
{
public:

    stackTestClass()
    {
        m_param = 1234;
    }


    void stackMethod(double a, int b)
    {
        __debugbreak();
    }

    long  m_param;
};

//#ifdef _DEBUG
//#pragma optimize( "", on )
//#endif

std::string __stdcall stackTestRun2( int a, double b, const char* c )
{
    int  localInt = 10;

    stackTestClass   stackClass;
    stackClass.stackMethod(b, localInt);
    
    if ( localInt/5 > 0 )
        return std::string("Hello!");
    else
        return std::string("Buy!");
}

//#ifdef _DEBUG
//#pragma optimize( "", off )
//#endif

float stackTestRun1( int a, const float& b, const std::string& c )
{
    static long staticLong = 1;
    volatile double localDouble = 0.0;

    try {

        volatile float localFloat = b;

        if ( staticLong == 1 )
        {

            char  localChars[0x100];
            static float staticFloat = 3.5f;
           //c.copy(localChars, c.size());
            strcpy_s(localChars, 0x100, c.c_str() );
            stackTestRun2( a, localFloat, localChars );
            return 1.1f + staticLong + staticFloat;
        }
    }
    catch(...)
    {
    }

    return (float)localDouble;
}

int stackTestRun()
{
    stackTestRun1( 10, 0.8f, "Bye" );

    return 0;
}


int loadUnloadModuleRun()
{
    __debugbreak();

    HMODULE  hlib = LoadLibrary(L"ws2_32.dll");
    FreeLibrary(hlib);

    return 0;
}

int startChildProcess()
{
    const DWORD nBufferLength = 0x1000;
    std::vector<wchar_t> buffer(nBufferLength);
    DWORD   len = GetCurrentDirectoryW( nBufferLength, &buffer[0] );

    std::wstring  path = std::wstring( &buffer[0], len );
    path += L"\\targetapp.exe"; 

    std::wstring  cmdline = L"processtest";

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
        CREATE_NO_WINDOW | DETACHED_PROCESS, 
        NULL,
        NULL,
        &startupInfo,
        &processInfo );

    DWORD  err = GetLastError();

    WaitForSingleObject( processInfo.hProcess, INFINITE );

    CloseHandle( processInfo.hProcess );
    CloseHandle( processInfo.hThread );

    return 0;
}

void __cdecl sleepThread(void* param)
{
    InterlockedIncrement((LONG*)param);

    Sleep(INFINITE);
}
