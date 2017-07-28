
#include "stdafx.h"

#include <Windows.h>
#include <Psapi.h>
#include <TlHelp32.h>

#include "kdlib/dbgengine.h"
#include "kdlib/exceptions.h"

namespace {

//////////////////////////////////////////////////////////////////////////////

std::wstring getLiveProcessUser(DWORD processID)
{
    std::wstring  fullUserName = L"<unknown>";
    HANDLE  hProcess = NULL;
    HANDLE  hToken = NULL;
    PCHAR  tokenUserBuffer = NULL;
        
    do {

       hProcess = OpenProcess( PROCESS_QUERY_INFORMATION, FALSE, processID );
       if ( !hProcess )
           break;

       OpenProcessToken(hProcess,  TOKEN_QUERY | TOKEN_READ, &hToken);
       if ( !hToken )
           break;

       DWORD  tokenUserSize =  0;
       GetTokenInformation(hToken, TokenOwner, NULL, 0, &tokenUserSize);

       tokenUserBuffer = new char [tokenUserSize];
       PTOKEN_USER  tokenUser = reinterpret_cast<PTOKEN_USER>(tokenUserBuffer);
       if ( !GetTokenInformation(hToken, TokenOwner, tokenUserBuffer, tokenUserSize, &tokenUserSize) )
           break;

       WCHAR  userName[100];
       DWORD  userNameLength = sizeof(userName)/sizeof(WCHAR);
       WCHAR  domainName[100];
       DWORD  domainNameLength = sizeof(domainName)/sizeof(WCHAR);
       SID_NAME_USE  sidNameUse;
       if ( !LookupAccountSidW(
           NULL, 
           tokenUser->User.Sid, 
           userName,
           &userNameLength, 
           domainName,
           &domainNameLength,
           &sidNameUse ) )
             break;

       fullUserName = std::wstring(domainName) + L"@" + std::wstring(userName);

    } while(FALSE);

    if ( tokenUserBuffer )
        delete[] tokenUserBuffer;

    if ( hProcess != NULL )
        CloseHandle(hProcess);

    if ( hToken != NULL )
        CloseHandle(hToken);

    return fullUserName;
}

//////////////////////////////////////////////////////////////////////////////

}

namespace  kdlib {

//////////////////////////////////////////////////////////////////////////////

unsigned long getNumberLiveProcesses()
{
    DWORD  bytesNeeded;
    DWORD  processIds[0x1000];

    EnumProcesses(processIds, sizeof(processIds), &bytesNeeded);

    return bytesNeeded/sizeof(DWORD);
}

//////////////////////////////////////////////////////////////////////////////

void getLiveProcessesList(std::vector<LiveProcessInfo> &processInfo)
{

    HANDLE snapshotHandle = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snapshotHandle ==  INVALID_HANDLE_VALUE)
        throw DbgException("Failed to get processes snapshot");

    PROCESSENTRY32W  processEntry;
    processEntry.dwSize = sizeof( PROCESSENTRY32 );

    bool  getProcessRes = Process32FirstW(snapshotHandle, &processEntry);

    while( getProcessRes )
    {
        LiveProcessInfo  pinfo;

        pinfo.pid =processEntry.th32ProcessID;
        pinfo.parentPid = processEntry.th32ParentProcessID;
        pinfo.name = std::wstring( processEntry.szExeFile);
        pinfo.user = getLiveProcessUser(pinfo.pid);

        processInfo.push_back(pinfo);

        getProcessRes = Process32NextW(snapshotHandle, &processEntry);
    }
}

}
