#pragma once 

#include "gtest/gtest.h"

#include "kdlib/module.h"

#include "procfixture.h"

class WinApiTest : public ProcessFixture 
{
public:

    WinApiTest() : ProcessFixture( L"typedvartest" ) {}
};


//HANDLE WINAPI CreateFile(
//  _In_     LPCTSTR               lpFileName,
//  _In_     DWORD                 dwDesiredAccess,
//  _In_     DWORD                 dwShareMode,
//  _In_opt_ LPSECURITY_ATTRIBUTES lpSecurityAttributes,
//  _In_     DWORD                 dwCreationDisposition,
//  _In_     DWORD                 dwFlagsAndAttributes,
//  _In_opt_ HANDLE                hTemplateFile
//);

TEST_F(WinApiTest, CreateFileCall)
{
    kdlib::ModulePtr  kernel32;
        
    ASSERT_NO_THROW( kernel32 = kdlib::loadModule(L"kernel32") );
    ASSERT_NE( 0, kernel32->asULongLong() );

    kdlib::MEMOFFSET_64  createFileOffset;

    ASSERT_NO_THROW( createFileOffset = kernel32->getSymbolVa(L"CreateFileW") );
    ASSERT_NE(0, createFileOffset );

    TypeInfoPtr  HANDLE_Type;
    ASSERT_NO_THROW(HANDLE_Type = kdlib::loadType(L"Void*"));

    TypeInfoPtr  CreateFile_Type;
    ASSERT_NO_THROW( CreateFile_Type = kdlib::defineFunction( HANDLE_Type, kdlib::CallConv_NearStd) );
    ASSERT_NO_THROW( CreateFile_Type->appendField(L"lpFileName", kdlib::loadType(L"WChar*") ) );
    ASSERT_NO_THROW( CreateFile_Type->appendField(L"dwDesiredAccess", kdlib::loadType(L"UInt4B") ) );
    ASSERT_NO_THROW( CreateFile_Type->appendField(L"dwShareMode", kdlib::loadType(L"UInt4B") ) );
    ASSERT_NO_THROW( CreateFile_Type->appendField(L"lpSecurityAttributes", kdlib::loadType(L"Void*") ) );
    ASSERT_NO_THROW( CreateFile_Type->appendField(L"dwCreationDisposition", kdlib::loadType(L"UInt4B") ) );
    ASSERT_NO_THROW( CreateFile_Type->appendField(L"dwFlagsAndAttributes", kdlib::loadType(L"UInt4B") ) );
    ASSERT_NO_THROW( CreateFile_Type->appendField(L"hTemplateFile", HANDLE_Type ) );

    MEMOFFSET_64 fileNameBuf;
    ASSERT_NO_THROW( fileNameBuf= kdlib::stackAlloc(100) );

    const wchar_t*  fileNameStr = L"testfile.txt";
    std::vector<unsigned short>   fileName( fileNameStr, fileNameStr + wcslen(fileNameStr) + 1 );
    ASSERT_NO_THROW( kdlib::writeWords( fileNameBuf, fileName ) );

    TypedVarPtr  CreateFilePtr;

    ASSERT_NO_THROW( CreateFilePtr = kdlib::loadTypedVar(CreateFile_Type, createFileOffset) );

    HANDLE  fileHandle = INVALID_HANDLE_VALUE;

    if ( CreateFile_Type->getPtrSize() == 4 )
    {
        ASSERT_NO_THROW( fileHandle = (HANDLE)CreateFilePtr->call( { 
            (MEMOFFSET_32)fileNameBuf,
            (DWORD)GENERIC_READ|GENERIC_WRITE, 
            (DWORD)0,
            (MEMOFFSET_32)0,
            (DWORD)CREATE_ALWAYS,
            (DWORD)FILE_ATTRIBUTE_NORMAL,
            (DWORD)0 } ) );
    }
    else
    {
        ASSERT_NO_THROW( fileHandle = (HANDLE)CreateFilePtr->call( {
            (MEMOFFSET_64)fileNameBuf,
            (DWORD)GENERIC_READ|GENERIC_WRITE, 
            (DWORD)0,
            (MEMOFFSET_64)0,
            (DWORD)CREATE_ALWAYS,
            (DWORD)FILE_ATTRIBUTE_NORMAL,
            (DWORD)0 } ) );
    }

    EXPECT_NE( INVALID_HANDLE_VALUE, fileHandle );

    EXPECT_NO_THROW( kdlib::stackFree(100) );
}

TEST_F(WinApiTest, CreateFileCallClang)
{
    kdlib::ModulePtr  kernel32;
    ASSERT_NO_THROW( kernel32 = kdlib::loadModule(L"kernel32") );

    kdlib::MEMOFFSET_64  createFileOffset;
    ASSERT_NO_THROW( createFileOffset = kernel32->getSymbolVa(L"CreateFileW") );

    std::wstring  src = L"#include <windows.h>\r\n";
    std::wstring  opt = L"-I\"C:/Program Files (x86)/Windows Kits/8.1/Include/um\" \
        -I\"C:/Program Files (x86)/Windows Kits/8.1/Include/shared\" -w";

    MEMOFFSET_64 fileNameBuf;
    ASSERT_NO_THROW( fileNameBuf= kdlib::stackAlloc(100) );

    const wchar_t*  fileNameStr = L"testfile1.txt";
    std::vector<unsigned short>   fileName( fileNameStr, fileNameStr + wcslen(fileNameStr) + 1 );
    ASSERT_NO_THROW( kdlib::writeWords( fileNameBuf, fileName ) );

    TypeInfoPtr  CreateFile_Type;
    ASSERT_NO_THROW( CreateFile_Type = kdlib::compileType(src, L"CreateFileW", opt) );

    TypedVarPtr  CreateFilePtr;
    ASSERT_NO_THROW( CreateFilePtr = kdlib::loadTypedVar(CreateFile_Type, createFileOffset) );

    HANDLE  fileHandle = INVALID_HANDLE_VALUE;

    if ( CreateFile_Type->getPtrSize() == 4 )
    {
        ASSERT_NO_THROW( fileHandle = (HANDLE)CreateFilePtr->call( { 
            (MEMOFFSET_32)fileNameBuf,
            (DWORD)GENERIC_READ|GENERIC_WRITE, 
            (DWORD)0,
            (MEMOFFSET_32)0,
            (DWORD)CREATE_ALWAYS,
            (DWORD)FILE_ATTRIBUTE_NORMAL,
            (DWORD)0 } ) );
    }
    else
    {
        ASSERT_NO_THROW( fileHandle = (HANDLE)CreateFilePtr->call( {
            (MEMOFFSET_64)fileNameBuf,
            (DWORD)GENERIC_READ|GENERIC_WRITE, 
            (DWORD)0,
            (MEMOFFSET_64)0,
            (DWORD)CREATE_ALWAYS,
            (DWORD)FILE_ATTRIBUTE_NORMAL,
            (DWORD)0 } ) );
    }

    EXPECT_NE( INVALID_HANDLE_VALUE, fileHandle );

    EXPECT_NO_THROW( kdlib::stackFree(100) );

}
