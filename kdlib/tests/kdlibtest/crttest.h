#pragma once 

#include "gtest/gtest.h"

#include "kdlib/typedvar.h"

#include "procfixture.h"

class CrtTest : public ProcessFixture 
{
public:

    CrtTest() : ProcessFixture( L"typedvartest" ) {}
};

TEST_F(CrtTest, malloc)
{
    kdlib::TypedVarPtr  malloc;
    kdlib::TypedVarPtr  free;
    MEMOFFSET_64  addr;

    ASSERT_NO_THROW(malloc = kdlib::loadTypedVar(L"malloc"));
    ASSERT_NO_THROW(free = kdlib::loadTypedVar(L"free"));

    ASSERT_NO_THROW(addr = malloc->call( { 100 } ) );
    EXPECT_NO_THROW(free->call( { addr } ) );
}
