#pragma once

#include "kdlib/breakpoint.h"

#include "procfixture.h"
#include "eventhandlermock.h"

using namespace kdlib;
using namespace testing;

class BreakPointTest : public ProcessFixture
{
public:

    BreakPointTest() : ProcessFixture( L"breakhandlertest" ) {}

};


TEST_F( BreakPointTest, StopOnBreak )
{
    EventHandlerMock  eventHandler;
    
    DefaultValue<kdlib::DebugCallbackResult>::Set( DebugCallbackNoChange );

    EXPECT_CALL( eventHandler, onBreakpoint( _ ) ).Times(1);
    EXPECT_CALL(eventHandler, onCurrentThreadChange(_)).Times(AnyNumber());

    EXPECT_CALL( eventHandler, onExecutionStatusChange( kdlib::DebugStatusGo) ).Times(1);
    EXPECT_CALL( eventHandler, onExecutionStatusChange( kdlib::DebugStatusBreak) ).Times(1);

    ASSERT_NO_THROW( softwareBreakPointSet( m_targetModule->getSymbolVa( L"CdeclFunc" ) ) );

    targetGo();
}

TEST_F( BreakPointTest, RemoveBreak )
{
    EventHandlerMock    eventHandler;

    DefaultValue<kdlib::DebugCallbackResult>::Set( DebugCallbackNoChange );

    EXPECT_CALL(eventHandler, onCurrentThreadChange(_)).Times(AnyNumber());
    EXPECT_CALL(eventHandler, onProcessExit(_, ProcessExit, _)).Times(1);

    EXPECT_CALL( eventHandler, onBreakpoint( _ ) ).Times( 0 );
    EXPECT_CALL( eventHandler, onExecutionStatusChange( kdlib::DebugStatusGo) ).Times(1);
    EXPECT_CALL( eventHandler, onExecutionStatusChange( kdlib::DebugStatusNoDebuggee) ).Times(1);

    BreakpointPtr  bp;
    ASSERT_NO_THROW( bp = softwareBreakPointSet( m_targetModule->getSymbolVa( L"CdeclFunc" ) ) );
    ASSERT_NO_THROW( bp->remove() );
    EXPECT_EQ( 0, getNumberBreakpoints() );

    targetGo();
}


TEST_F( BreakPointTest, EnumBreak )
{
    BreakpointPtr  bp1, bp2, bp3;
 
    EXPECT_EQ( 0, getNumberBreakpoints() );
    ASSERT_NO_THROW( bp1 = softwareBreakPointSet( m_targetModule->getSymbolVa( L"CdeclFunc" ) ) );
    EXPECT_EQ( 1, getNumberBreakpoints() );
    ASSERT_NO_THROW( bp2 = softwareBreakPointSet( m_targetModule->getSymbolVa( L"StdcallFunc" ) ) );
    EXPECT_EQ( 2, getNumberBreakpoints() );
    ASSERT_NO_THROW( bp2->remove() );
    EXPECT_EQ( 1, getNumberBreakpoints() );
    ASSERT_NO_THROW( bp3 = softwareBreakPointSet( m_targetModule->getSymbolVa( L"FastcallFunc" ) ) );
    EXPECT_EQ( 2, getNumberBreakpoints() );

    BreakpointPtr  bpIndex;
    ASSERT_NO_THROW( bpIndex = getBreakpointByIndex(0) );
    EXPECT_TRUE( bpIndex->getId() == bp1->getId() || bpIndex->getId() == bp3->getId() );

    ASSERT_NO_THROW( bpIndex = getBreakpointByIndex(1) );
    EXPECT_TRUE( bpIndex->getId() == bp1->getId() || bpIndex->getId() == bp3->getId() );

    EXPECT_THROW( getBreakpointByIndex(2), IndexException );
    EXPECT_THROW( getBreakpointByIndex(-1), IndexException );
}

class BreakpointMock 
{
public:

    MOCK_METHOD0( onHit, kdlib::DebugCallbackResult () );
    MOCK_METHOD0( onRemove, void() );
};


TEST_F( BreakPointTest, AutoBreakpoint )
{
    AutoBreakpoint<BreakpointMock>  bp(m_targetModule->getSymbolVa( L"CdeclFunc"));

    DefaultValue<kdlib::DebugCallbackResult>::Set( DebugCallbackBreak );

    EXPECT_CALL( bp, onHit() ).Times(1);
    EXPECT_CALL( bp, onRemove() ).Times(1);

    ASSERT_EQ( DebugStatusBreak, targetGo() );

    EXPECT_EQ( DebugStatusBreak, targetExecutionStatus() );
    EXPECT_EQ( getInstructionOffset(), bp.getOffset() );
}


TEST_F( BreakPointTest, AutoBreakpointSet )
{
    AutoBreakpoint<BreakpointMock>  bp0;

    AutoBreakpoint<BreakpointMock>  bp1(m_targetModule->getSymbolVa( L"CdeclFunc"));

    AutoBreakpoint<BreakpointMock>  bp2(AutoBreakpoint<BreakpointMock>(m_targetModule->getSymbolVa( L"CdeclFunc") + 1));

    EXPECT_CALL( bp1, onRemove() ).Times(1);
    EXPECT_CALL( bp2, onRemove() ).Times(1);

    EXPECT_EQ( 2, getNumberBreakpoints() );

    bp1 = bp2;

    EXPECT_EQ( 1, getNumberBreakpoints() );

    bp1.release();

    EXPECT_EQ( 0, getNumberBreakpoints() );
}



TEST_F( BreakPointTest, ScopedBreakpoint )
{
    {
        ScopedBreakpoint  bp1;

        ASSERT_NO_THROW( bp1 = softwareBreakPointSet( m_targetModule->getSymbolVa( L"CdeclFunc" ) ) );
        EXPECT_EQ( 1, getNumberBreakpoints() );

        ScopedBreakpoint  bp2( softwareBreakPointSet( m_targetModule->getSymbolVa( L"CdeclFunc") + 1 ) );
        EXPECT_EQ( 2, getNumberBreakpoints() );
    }

    EXPECT_EQ( 0, getNumberBreakpoints() );
}


TEST_F( BreakPointTest, ProcessTerminate )
{
    {
        ScopedBreakpoint  bp;

        ASSERT_NO_THROW( bp = softwareBreakPointSet( m_targetModule->getSymbolVa( L"CdeclFunc" ) ) );
        EXPECT_EQ( 1, getNumberBreakpoints() );

        EXPECT_EQ( DebugStatusBreak, targetGo() );

        ASSERT_NO_THROW( terminateProcess(m_processId) );

        EXPECT_THROW( getNumberBreakpoints(), DbgException ); // there is no targets 
    }
}
