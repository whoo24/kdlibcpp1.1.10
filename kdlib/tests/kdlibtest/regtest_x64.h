#pragma once

#include "procfixture.h"

#include <kdlib/dataaccessor.h>

using namespace kdlib;
using namespace testing;

class RegTest : public ProcessFixture 
{
};


TEST_F(RegTest, GetRegIndex)
{
    EXPECT_NO_THROW(getRegisterIndex(L"rax"));
    EXPECT_NO_THROW(getRegisterIndex(L"rsi"));
    EXPECT_NO_THROW(getRegisterIndex(L"ebx"));

    EXPECT_THROW(getRegisterIndex(L"RAX"), CPUException);
    EXPECT_THROW(getRegisterIndex(L"RSI"), CPUException);
    EXPECT_THROW(getRegisterIndex(L"EBX"), CPUException);
    EXPECT_THROW(getRegisterIndex(L"abc"), CPUException);
}

TEST_F(RegTest, RegisterAccessor)
{
    DataAccessorPtr  regAccessor;
    ASSERT_NO_THROW(regAccessor=getRegisterAccessor(L"rax"));

    EXPECT_THROW(getRegisterAccessor(L"RAX"), CPUException);
}


