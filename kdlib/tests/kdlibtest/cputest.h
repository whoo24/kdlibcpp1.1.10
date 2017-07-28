#pragma once

#include "procfixture.h"
#include "kdlib/cpucontext.h"

using namespace kdlib;

class CPUContextTest : public ProcessFixture 
{
public:

    CPUContextTest() : ProcessFixture( L"cputest" ) {}
};

TEST_F( CPUContextTest, DISABLED_GetRegisters )
{
    CPUContextPtr  cpu;
    ASSERT_NO_THROW( cpu = loadCPUContext() );

    MEMDISPLACEMENT  displ;
    EXPECT_EQ( L"breakOnRun", findSymbol( cpu->getIP(), displ ) );
    EXPECT_NE( 0, cpu->getSP() );
}

TEST_F( CPUContextTest, RestoreContext )
{

    NumVariant reg1, reg2;
    ASSERT_NO_THROW(reg1 = getRegisterByIndex(10));
    ASSERT_NO_THROW(reg2 = getRegisterByName(L"eax"));

    CPUContextPtr  cpu;
    ASSERT_NO_THROW( cpu = loadCPUContext() );

    ASSERT_NO_THROW( setRegisterByIndex( 10,  reg1 + 10 ) );
    ASSERT_NO_THROW( setRegisterByName( L"eax", reg2 + 121 ) );

    EXPECT_NO_THROW( cpu->restore() );

    EXPECT_EQ( reg1, getRegisterByIndex(10) );
    EXPECT_EQ( reg2, getRegisterByName(L"eax") );
}

TEST_F( CPUContextTest, GetStackRegs )
{
    EXPECT_NO_THROW( getStackOffset() );
    EXPECT_NO_THROW( getFrameOffset() );
    EXPECT_NO_THROW( getInstructionOffset() );
}

TEST_F( CPUContextTest, SetStackRegs )
{
    MEMOFFSET_64  sp = getStackOffset();
    ASSERT_NO_THROW( setStackOffset(sp+100) );
    EXPECT_EQ(sp+100, getStackOffset() );
    ASSERT_NO_THROW( setStackOffset(sp) );

    MEMOFFSET_64  fp = getFrameOffset();
    ASSERT_NO_THROW( setFrameOffset(fp+100) );
    EXPECT_EQ(fp+100, getFrameOffset() );
    ASSERT_NO_THROW( setFrameOffset(fp) );

    MEMOFFSET_64  ip = getInstructionOffset();
    ASSERT_NO_THROW( setInstructionOffset(ip+100) );
    EXPECT_EQ(ip+100, getInstructionOffset() );
    ASSERT_NO_THROW( setInstructionOffset(ip) );
}

