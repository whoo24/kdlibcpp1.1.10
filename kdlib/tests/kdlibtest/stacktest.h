#pragma once

#include <math.h>

#include "procfixture.h"

#include "kdlib/stack.h"
#include "kdlib/memaccess.h"

using namespace kdlib;

class StackTest : public ProcessFixture 
{
public:

    StackTest() : ProcessFixture( L"stacktest" ) {}
};

TEST_F( StackTest, Ctor )
{
    EXPECT_NO_THROW( getStack() );
//    EXPECT_NO_THROW( getStack( loadCPUContext() ) );
}

TEST_F( StackTest, StackLength )
{
    StackPtr  stack;
    ASSERT_NO_THROW( stack = getStack() );
    EXPECT_LT( 3UL, stack->getFrameCount() );
}

//TEST_F( StackTest, GetFunction )
//{
//    StackFramePtr  frame;
//    TypedVarPtr  function;
//
//    ASSERT_NO_THROW( frame =  getStack()->getFrame(0) );
//    ASSERT_NO_THROW( function = frame->getFunction() );
//    EXPECT_EQ( std::wstring(L"stackTestClass::stackMethod"), findSymbol( function->getAddress() ) );
//
//    ASSERT_NO_THROW( frame =  getStack()->getFrame(1) );
//    ASSERT_NO_THROW( function = frame->getFunction() );
//    EXPECT_EQ( std::wstring(L"stackTestRun2"), findSymbol( function->getAddress() ) );
//
//    ASSERT_NO_THROW( frame =  getStack()->getFrame(2) );
//    ASSERT_NO_THROW( function = frame->getFunction() );
//    EXPECT_EQ( std::wstring(L"stackTestRun1"), findSymbol( function->getAddress() ) );
//}

TEST_F( StackTest, TypedParam )
{
    StackFramePtr  frame;
    ASSERT_NO_THROW( frame = getCurrentStackFrame() );

    EXPECT_EQ( 3, frame->getTypedParamCount() ); 

    TypedVarPtr  param;
    ASSERT_NO_THROW( param = frame->getTypedParam(0) );
    EXPECT_EQ(1234, param->deref()->getElement(L"m_param")->getValue());

    ASSERT_NO_THROW( param = frame->getTypedParam(1) );
    EXPECT_FLOAT_EQ( 0.8f, param->getValue().asFloat() );

    ASSERT_NO_THROW(param = frame->getTypedParam(2));
    EXPECT_EQ(10, param->getValue() );

    EXPECT_THROW(frame->getTypedParam(3), IndexException);
    EXPECT_THROW(frame->getTypedParam(-1), IndexException);

    EXPECT_EQ( 0, getStack()->getFrame(5)->getTypedParamCount() );
}

TEST_F(StackTest, FastcallParam)
{
    StackFramePtr  frame;
    ASSERT_NO_THROW(frame = getStack()->getFrame(1) );

    TypedVarPtr  param;

    ASSERT_NO_THROW(param = frame->getTypedParam(L"a"));
    EXPECT_EQ(10, param->getValue());

    ASSERT_NO_THROW(param = frame->getTypedParam(L"b"));
    EXPECT_FLOAT_EQ(0.8f, param->getValue().asFloat());
}

TEST_F( StackTest, TypedParamByName )
{
    StackFramePtr  frame;
    ASSERT_NO_THROW( frame = getCurrentStackFrame() );

    TypedVarPtr  param;
    ASSERT_NO_THROW( param = frame->getTypedParam(L"this") );
    EXPECT_EQ(1234, param->deref()->getElement(L"m_param")->getValue());

    ASSERT_NO_THROW( param = frame->getTypedParam(L"a") );
    EXPECT_FLOAT_EQ( 0.8f, param->getValue().asFloat() );

    ASSERT_NO_THROW( param = frame->getTypedParam(L"b") );
    EXPECT_EQ(10, param->getValue());
   
    EXPECT_THROW( frame->getTypedParam(L""), DbgException );
    EXPECT_THROW( frame->getTypedParam(L"notexist"), SymbolException );
    EXPECT_THROW(frame->getTypedParam(L"THIS"), SymbolException);
}

TEST_F( StackTest, LocalVars )
{
    StackFramePtr  frame;

    ASSERT_NO_THROW( frame = getStack()->getFrame(2) );
    EXPECT_EQ( 3, frame->getLocalVarCount() );

    EXPECT_FLOAT_EQ(0.0f, frame->getLocalVar(0)->getValue().asFloat());
    EXPECT_FLOAT_EQ(0.8f, frame->getLocalVar(L"localFloat")->getValue().asFloat());
    EXPECT_NO_THROW(frame->getLocalVar(L"localChars"));

    EXPECT_THROW(frame->getLocalVar(-1), IndexException);
    EXPECT_THROW(frame->getLocalVar(3), IndexException);
    EXPECT_THROW(frame->getLocalVar(L"Notexist"), SymbolException);
}

TEST_F(StackTest, StaticVars)
{
    StackFramePtr  frame;
    ASSERT_NO_THROW(frame = getStack()->getFrame(2));

    EXPECT_EQ( 2, frame->getStaticVarCount() );
    EXPECT_EQ(1, *frame->getStaticVar(0));
    EXPECT_EQ(1, *frame->getStaticVar(L"staticLong"));
    EXPECT_EQ(L"staticLong", frame->getStaticVarName(0));

    EXPECT_THROW(frame->getStaticVar(2), IndexException);
    EXPECT_THROW(frame->getStaticVar(-1), IndexException);
    EXPECT_THROW(frame->getStaticVarName(2), IndexException);
    EXPECT_THROW(frame->getStaticVar(L"Notexist"), SymbolException);
}

TEST_F(StackTest, ChangeCurrentFrame)
{
    StackPtr  stack;
    StackFramePtr  frame1;

    ASSERT_NO_THROW(stack = getStack());
    ASSERT_NO_THROW( frame1 = stack->getFrame(1) );
    ASSERT_NO_THROW( setCurrentStackFrame(frame1) );

    StackFramePtr  frame2;
    ASSERT_NO_THROW( frame2 = getCurrentStackFrame() );
    EXPECT_TRUE( frame1->getIP() == frame2->getIP() && 
        frame1->getRET() == frame2->getRET() &&
        frame1->getFP() == frame2->getFP() &&
        frame1->getSP() == frame2->getSP() );

    EXPECT_EQ( 1, getCurrentStackFrameNumber() );
    
    EXPECT_NO_THROW( resetCurrentStackFrame() );

    ASSERT_NO_THROW( frame1 = getStack()->getFrame(0) );

    ASSERT_NO_THROW( frame2 = getCurrentStackFrame() );
    EXPECT_TRUE( frame1->getIP() == frame2->getIP() && 
        frame1->getRET() == frame2->getRET() &&
        frame1->getFP() == frame2->getFP() &&
        frame1->getSP() == frame2->getSP() );

    EXPECT_EQ( 0, getCurrentStackFrameNumber() );

    ASSERT_NO_THROW(setCurrentStackFrameByIndex(2));
    EXPECT_EQ(2, getCurrentStackFrameNumber());

    ASSERT_NO_THROW(debugCommand(L".frame 3"));
    EXPECT_EQ(3, getCurrentStackFrameNumber());
}

TEST_F(StackTest, ChangeScopeEvent)
{
    EventHandlerMock  eventHandler;

    EXPECT_CALL(eventHandler, onChangeLocalScope()).Times(2);

    StackFramePtr  frame1;
    ASSERT_NO_THROW(frame1 = getStack()->getFrame(1));
    ASSERT_NO_THROW(setCurrentStackFrame(frame1));

    debugCommand(L".frame 2");
}


