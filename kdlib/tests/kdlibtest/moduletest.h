#pragma once

#include <boost/bind.hpp>

#include "kdlib/module.h"
#include "kdlib/memaccess.h"
#include "kdlib/exceptions.h"

#include "test/testvars.h"

#include "procfixture.h"
#include "eventhandlermock.h"

using namespace kdlib;
using namespace testing;

class ModuleTest : public ProcessFixture 
{
};

TEST_F( ModuleTest, Ctor )
{
    std::wstring  targetname = m_targetModule->getName();
    MEMOFFSET_64  targetstart = m_targetModule->getBase();

    EXPECT_EQ( targetname, loadModule( targetname )->getName() );
    EXPECT_EQ( targetname, loadModule( targetstart )->getName() );
}

TEST_F( ModuleTest, getSize )
{
    EXPECT_NE( 0, m_targetModule->getSize() );
}

TEST_F( ModuleTest, getBase )
{
    EXPECT_NE( 0, m_targetModule->getBase() );
    EXPECT_EQ( m_targetModule->getBase(), *m_targetModule );
    EXPECT_EQ( m_targetModule->getBase() + 0x100, 0x100 + *m_targetModule );
}

TEST_F( ModuleTest, getEnd )
{
    EXPECT_EQ( m_targetModule->getSize(), m_targetModule->getEnd() -  m_targetModule->getBase() );
    EXPECT_TRUE( isVaValid( m_targetModule->getEnd() - 1 ) );
    EXPECT_FALSE( isVaValid( m_targetModule->getEnd() + 1 ) );
}

TEST_F( ModuleTest, getSymFile )
{
    EXPECT_FALSE( m_targetModule->getSymFile().empty() );
}

TEST_F( ModuleTest, getImageName )
{
    EXPECT_EQ( L"targetapp.exe",  m_targetModule->getImageName() );
}

TEST_F( ModuleTest, findByAddr )
{
    EXPECT_THROW( loadModule( m_targetModule->getBase() - 1 ), DbgException );
    EXPECT_NO_THROW( loadModule( m_targetModule->getBase() ) );
    EXPECT_NO_THROW( loadModule( m_targetModule->getBase() + 1 ) );
    EXPECT_THROW( loadModule( m_targetModule->getEnd() + 1 ), DbgException );
}

TEST_F( ModuleTest, getSymbolVa )
{
    EXPECT_NE( 0, m_targetModule->getSymbolVa(L"helloStr") );
    EXPECT_EQ( m_targetModule->getSymbolVa(L"g_structTest"), ptrPtr(  m_targetModule->getSymbolVa(L"g_structTestPtr") ) );
}

TEST_F( ModuleTest, getTypeByName )
{
    EXPECT_EQ( L"structTest", m_targetModule->getTypeByName(L"structTest")->getName() );
    EXPECT_EQ( L"structTest", m_targetModule->getTypeByName(L"g_structTest")->getName() );
}

TEST_F( ModuleTest, enumSymbol )
{
    SymbolOffsetList  symLst = m_targetModule->enumSymbols();
    EXPECT_NE( 0, symLst.size() );
    EXPECT_FALSE( symLst.end() == std::find_if( symLst.begin(), symLst.end(), boost::bind( &SymbolOffset::first, _1 ) == L"helloStr" ) );

    symLst = m_targetModule->enumSymbols(L"g_*");
    EXPECT_NE( 0, symLst.size() );
    EXPECT_FALSE( symLst.end() == std::find_if( symLst.begin(), symLst.end(), boost::bind( &SymbolOffset::first, _1 ) == L"g_structTest" ) );
    EXPECT_TRUE( symLst.end() == std::find_if( symLst.begin(), symLst.end(), boost::bind( &SymbolOffset::first, _1 ) == L"helloStr" ) );

    symLst = m_targetModule->enumSymbols(L"g_structTest");
    EXPECT_EQ( 1, symLst.size() );

    symLst = m_targetModule->enumSymbols(L"structTest");
    EXPECT_EQ( 0, symLst.size() );
}

TEST_F( ModuleTest, findSymbol )
{
    std::wstring  name;
    MEMDISPLACEMENT  displacement;
    EXPECT_NO_THROW( name = m_targetModule->findSymbol( m_targetModule->getSymbolVa(L"g_structTest"), displacement ) );
    EXPECT_EQ( L"g_structTest", name );
    EXPECT_EQ( 0, displacement );

    EXPECT_NO_THROW( name = m_targetModule->findSymbol( m_targetModule->getSymbolVa(L"g_structTest") + 1, displacement ) );
    EXPECT_EQ( L"g_structTest", name );
    EXPECT_EQ( 1, displacement );

    EXPECT_THROW( m_targetModule->findSymbol( m_targetModule->getSymbolVa(L"g_structTestAAAA"), displacement ), SymbolException );
}

TEST_F( ModuleTest, getTypedVar )
{
    MEMOFFSET_64 offset;
    ASSERT_TRUE( 0 != (offset = m_targetModule->getSymbolVa(L"helloStr") ) );
    EXPECT_NO_THROW( m_targetModule->getTypedVarByAddr( offset ) );
    EXPECT_NO_THROW( m_targetModule->getTypedVarByName(L"helloStr") ) ;
    EXPECT_NO_THROW( m_targetModule->getTypedVarByTypeName(L"structTest", offset) ) ;

    EXPECT_THROW( m_targetModule->getTypedVarByAddr( 0 ), SymbolException );
    EXPECT_THROW( m_targetModule->getTypedVarByName(L"helloStr1111"), SymbolException ); 
    EXPECT_THROW( m_targetModule->getTypedVarByTypeName( L"", 1),  SymbolException);

    EXPECT_NO_THROW( m_targetModule->getTypedVarByTypeName( L"structTest", m_targetModule->getEnd() ) );
    EXPECT_THROW( m_targetModule->getTypedVarByTypeName( L"structTest", m_targetModule->getEnd() )->getElement(L"m_field0")->getValue(), MemoryException );
}

TEST_F( ModuleTest, getSymbolSize )
{
    EXPECT_EQ( sizeof(structTest), m_targetModule->getSymbolSize(L"structTest") );
    EXPECT_EQ( sizeof(g_structTest), m_targetModule->getSymbolSize(L"g_structTest") );
    EXPECT_EQ( sizeof(structNullSize), m_targetModule->getSymbolSize(L"structNullSize") );

    EXPECT_THROW(m_targetModule->getSymbolSize(L"1"), SymbolException );

    EXPECT_EQ( sizeof(structTest), getSymbolSize(L"structTest") );
    EXPECT_EQ( sizeof(structTest), getSymbolSize(L"targetapp!structTest") );
}

TEST_F( ModuleTest, getSourceLine )
{
    std::wstring fileName;
    unsigned long  lineNo;
    long displacement;

    EXPECT_NO_THROW( m_targetModule->getSourceLine( m_targetModule->getSymbolVa(L"CdeclFunc") + 2, fileName, lineNo, displacement ) );
    EXPECT_TRUE( fileName.find(L"testfunc.cpp") != std::wstring::npos );
    EXPECT_EQ( 30, lineNo );
    EXPECT_EQ( 2, displacement );

    //EXPECT_NO_THROW( m_targetModule->getSourceLine( m_targetModule->getSymbolVa(L"ucharVar"), fileName, lineNo, displacement ) );

    //EXPECT_TRUE( fileName.find(L"testvars.cpp") != std::wstring::npos );
    //EXPECT_EQ( 10, lineNo );
    //EXPECT_EQ( 0, displacement );
}


TEST_F( ModuleTest, getFunction )
{
    MEMOFFSET_64  addr;
    TypedVarPtr  funcPtr;

    ASSERT_NO_THROW( addr = m_targetModule->getTypedVarByName(L"stackTestRun1")->getAddress() );

    EXPECT_EQ( addr, m_targetModule->getFunctionByAddr(addr)->getAddress() );
    EXPECT_EQ( addr, m_targetModule->getFunctionByAddr(addr + 10)->getAddress() );
    EXPECT_EQ( addr, m_targetModule->getFunctionByAddr(addr + 50)->getAddress() );
    EXPECT_EQ( addr, m_targetModule->getFunctionByAddr(addr + 100)->getAddress() );

    size_t  funcSize;
    ASSERT_NO_THROW(funcSize =  m_targetModule->getFunctionByAddr(addr)->getSize() );

    EXPECT_THROW( m_targetModule->getFunctionByAddr( addr - 1 ), SymbolException );
    EXPECT_THROW( m_targetModule->getFunctionByAddr( addr + funcSize ), SymbolException );
}

class ModuleCallbackTest : public ProcessFixture 
{
public:

    ModuleCallbackTest() : ProcessFixture( L"loadunloadmodule" ) {}

    virtual void TearDown() {
    }
};


TEST_F( ModuleCallbackTest, loadModule)
{
    EventHandlerMock    eventHandler;

    DefaultValue<kdlib::DebugCallbackResult>::Set( DebugCallbackNoChange );

    EXPECT_CALL(eventHandler, onExecutionStatusChange(_) ).Times(AnyNumber());
    EXPECT_CALL(eventHandler, onCurrentThreadChange(_)).Times(AnyNumber());

    EXPECT_CALL(eventHandler, onModuleLoad( _, std::wstring(L"ws2_32")) ).Times(1);
    EXPECT_CALL(eventHandler, onModuleLoad( _,  Ne(std::wstring(L"ws2_32")))).Times(AnyNumber());

    EXPECT_CALL(eventHandler, onModuleUnload( _, _ )).Times(AnyNumber());

    targetGo();
}

