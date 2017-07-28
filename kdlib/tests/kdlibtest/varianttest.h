#pragma once 

#include "gtest/gtest.h"

#include "kdlib/variant.h"

using namespace kdlib;

class VariantCtorTest : public ::testing::Test {
};

TEST_F( VariantCtorTest, Ctor )
{
    EXPECT_TRUE( NumVariant(0).isInt() );
    EXPECT_TRUE( NumVariant(-1LL).isLongLong() );
    EXPECT_TRUE( NumVariant(200.0f).isFloat() );
    EXPECT_TRUE( NumVariant(200.0000001).isDouble() );
    EXPECT_TRUE( NumVariant('a').isChar() );
    EXPECT_TRUE( NumVariant(unsigned char(10) ).isUChar() );
    EXPECT_TRUE( NumVariant(unsigned short(29) ).isUShort() );
}


class VariantConversionTest : public ::testing::TestWithParam<unsigned long long> {
};

TEST_P( VariantConversionTest, Conversion)
{
    unsigned long long v = GetParam();

    EXPECT_TRUE( NumVariant( char(v) + char(v) ).isInt() && ( NumVariant( char(v) ) + NumVariant( char(v) ) ).isInt() );
    EXPECT_TRUE( NumVariant( char(v) + unsigned char(v) ).isInt() && ( NumVariant( char(v) ) + NumVariant( unsigned char(v) ) ).isInt() );
    EXPECT_TRUE( NumVariant( char(v) + short(v) ).isInt() && ( NumVariant( char(v) ) + NumVariant( short(v) ) ).isInt() );
    EXPECT_TRUE( NumVariant( char(v) + unsigned short(v) ).isInt() && ( NumVariant( char(v) ) + NumVariant( unsigned short(v) ) ).isInt() );
    EXPECT_TRUE( NumVariant( char(v) + long(v) ).isLong() && ( NumVariant( char(v) ) + NumVariant( long(v) ) ).isLong() );
    EXPECT_TRUE( NumVariant( char(v) + unsigned long(v) ).isULong() && ( NumVariant( char(v) ) + NumVariant( unsigned long(v) ) ).isULong() );
    EXPECT_TRUE( NumVariant( char(v) + long long(v) ).isLongLong() && ( NumVariant( char(v) ) + NumVariant( long long(v) ) ).isLongLong() );
    EXPECT_TRUE( NumVariant( char(v) + unsigned long long(v) ).isULongLong() && ( NumVariant( char(v) ) + NumVariant( unsigned long long(v) ) ).isULongLong() );
    EXPECT_TRUE( NumVariant( char(v) + int(v) ).isInt() && ( NumVariant( char(v) ) + NumVariant( int(v) ) ).isInt() );
    EXPECT_TRUE( NumVariant( char(v) + unsigned int(v) ).isUInt() && ( NumVariant( char(v) ) + NumVariant( unsigned int(v) ) ).isUInt() );
    EXPECT_TRUE( NumVariant( char(v) + float(v) ).isFloat() && ( NumVariant( char(v) ) + NumVariant( float(v) ) ).isFloat() );
    EXPECT_TRUE( NumVariant( char(v) + double(v) ).isDouble() && ( NumVariant( char(v) ) + NumVariant( double(v) ) ).isDouble() );

    EXPECT_TRUE( NumVariant( unsigned char(v) + unsigned char(v) ).isInt() && ( NumVariant( unsigned char(v) ) + NumVariant( unsigned char(v) ) ).isInt() );
    EXPECT_TRUE( NumVariant( unsigned char(v) + short(v) ).isInt() && ( NumVariant( unsigned char(v) ) + NumVariant( short(v) ) ).isInt() );
    EXPECT_TRUE( NumVariant( unsigned char(v) + unsigned short(v) ).isInt() && ( NumVariant( unsigned char(v) ) + NumVariant( unsigned short(v) ) ).isInt() );
    EXPECT_TRUE( NumVariant( unsigned char(v) + long(v) ).isLong() && ( NumVariant( unsigned char(v) ) + NumVariant( long(v) ) ).isLong() );
    EXPECT_TRUE( NumVariant( unsigned char(v) + unsigned long(v) ).isULong() && ( NumVariant( unsigned char(v) ) + NumVariant( unsigned long(v) ) ).isULong() );
    EXPECT_TRUE( NumVariant( unsigned char(v) + long long(v) ).isLongLong() && ( NumVariant( unsigned char(v) ) + NumVariant( long long(v) ) ).isLongLong() );
    EXPECT_TRUE( NumVariant( unsigned char(v) + unsigned long long(v) ).isULongLong() && ( NumVariant( unsigned char(v) ) + NumVariant( unsigned long long(v) ) ).isULongLong() );
    EXPECT_TRUE( NumVariant( unsigned char(v) + int(v) ).isInt() && ( NumVariant( unsigned char(v) ) + NumVariant( int(v) ) ).isInt() );
    EXPECT_TRUE( NumVariant( unsigned char(v) + unsigned int(v) ).isUInt() && ( NumVariant( unsigned char(v) ) + NumVariant( unsigned int(v) ) ).isUInt() );
    EXPECT_TRUE( NumVariant( unsigned char(v) + float(v) ).isFloat() && ( NumVariant( unsigned char(v) ) + NumVariant( float(v) ) ).isFloat() );
    EXPECT_TRUE( NumVariant( unsigned char(v) + double(v) ).isDouble() && ( NumVariant( unsigned char(v) ) + NumVariant( double(v) ) ).isDouble() );

    EXPECT_TRUE( NumVariant( short(v) + short(v) ).isInt() && ( NumVariant( short(v) ) + NumVariant( short(v) ) ).isInt() );
    EXPECT_TRUE( NumVariant( short(v) + unsigned short(v) ).isInt() && ( NumVariant( short(v) ) + NumVariant( unsigned short(v) ) ).isInt() );
    EXPECT_TRUE( NumVariant( short(v) + long(v) ).isLong() && ( NumVariant( short(v) ) + NumVariant( long(v) ) ).isLong() );
    EXPECT_TRUE( NumVariant( short(v) + unsigned long(v) ).isULong() && ( NumVariant( short(v) ) + NumVariant( unsigned long(v) ) ).isULong() );
    EXPECT_TRUE( NumVariant( short(v) + long long(v) ).isLongLong() && ( NumVariant( short(v) ) + NumVariant( long long(v) ) ).isLongLong() );
    EXPECT_TRUE( NumVariant( short(v) + unsigned long long(v) ).isULongLong() && ( NumVariant( short(v) ) + NumVariant( unsigned long long(v) ) ).isULongLong() );
    EXPECT_TRUE( NumVariant( short(v) + int(v) ).isInt() && ( NumVariant( short(v) ) + NumVariant( int(v) ) ).isInt() );
    EXPECT_TRUE( NumVariant( short(v) + unsigned int(v) ).isUInt() && ( NumVariant( short(v) ) + NumVariant( unsigned int(v) ) ).isUInt() );
    EXPECT_TRUE( NumVariant( short(v) + float(v) ).isFloat() && ( NumVariant( short(v) ) + NumVariant( float(v) ) ).isFloat() );
    EXPECT_TRUE( NumVariant( short(v) + double(v) ).isDouble() && ( NumVariant( short(v) ) + NumVariant( double(v) ) ).isDouble() );

    EXPECT_TRUE( NumVariant( unsigned short(v) + unsigned short(v) ).isInt() && ( NumVariant( unsigned short(v) ) + NumVariant( unsigned short(v) ) ).isInt() );
    EXPECT_TRUE( NumVariant( unsigned short(v) + long(v) ).isLong() && ( NumVariant( unsigned short(v) ) + NumVariant( long(v) ) ).isLong() );
    EXPECT_TRUE( NumVariant( unsigned short(v) + unsigned long(v) ).isULong() && ( NumVariant( unsigned short(v) ) + NumVariant( unsigned long(v) ) ).isULong() );
    EXPECT_TRUE( NumVariant( unsigned short(v) + long long(v) ).isLongLong() && ( NumVariant( unsigned short(v) ) + NumVariant( long long(v) ) ).isLongLong() );
    EXPECT_TRUE( NumVariant( unsigned short(v) + unsigned long long(v) ).isULongLong() && ( NumVariant( unsigned short(v) ) + NumVariant( unsigned long long(v) ) ).isULongLong() );
    EXPECT_TRUE( NumVariant( unsigned short(v) + int(v) ).isInt() && ( NumVariant( unsigned short(v) ) + NumVariant( int(v) ) ).isInt() );
    EXPECT_TRUE( NumVariant( unsigned short(v) + unsigned int(v) ).isUInt() && ( NumVariant( unsigned short(v) ) + NumVariant( unsigned int(v) ) ).isUInt() );
    EXPECT_TRUE( NumVariant( unsigned short(v) + float(v) ).isFloat() && ( NumVariant( unsigned short(v) ) + NumVariant( float(v) ) ).isFloat() );
    EXPECT_TRUE( NumVariant( unsigned short(v) + double(v) ).isDouble() && ( NumVariant( unsigned short(v) ) + NumVariant( double(v) ) ).isDouble() );

    EXPECT_TRUE( NumVariant( long(v) + long(v) ).isLong() && ( NumVariant( long(v) ) + NumVariant( long(v) ) ).isLong() );
    EXPECT_TRUE( NumVariant( long(v) + unsigned long(v) ).isULong() && ( NumVariant(long(v) ) + NumVariant( unsigned long(v) ) ).isULong() );
    EXPECT_TRUE( NumVariant( long(v) + long long(v) ).isLongLong() && ( NumVariant( long(v) ) + NumVariant( long long(v) ) ).isLongLong() );
    EXPECT_TRUE( NumVariant( long(v) + unsigned long long(v) ).isULongLong() && ( NumVariant( long(v) ) + NumVariant( unsigned long long(v) ) ).isULongLong() );
    EXPECT_TRUE( NumVariant( long(v) + int(v) ).isLong() && ( NumVariant( long(v) ) + NumVariant( int(v) ) ).isLong() );
    EXPECT_TRUE( NumVariant( long(v) + unsigned int(v) ).isULong() && ( NumVariant( long(v) ) + NumVariant( unsigned int(v) ) ).isULong() );
    EXPECT_TRUE( NumVariant( long(v) + float(v) ).isFloat() && ( NumVariant( long(v) ) + NumVariant( float(v) ) ).isFloat() );
    EXPECT_TRUE( NumVariant( long(v) + double(v) ).isDouble() && ( NumVariant( long(v) ) + NumVariant( double(v) ) ).isDouble() );

    EXPECT_TRUE( NumVariant( unsigned long(v) + unsigned long(v) ).isULong() && ( NumVariant( unsigned long(v) ) + NumVariant( unsigned long(v) ) ).isULong() );
    EXPECT_TRUE( NumVariant( unsigned long(v) + long long(v) ).isLongLong() && ( NumVariant(  unsigned long(v) ) + NumVariant( long long(v) ) ).isLongLong() );
    EXPECT_TRUE( NumVariant( unsigned long(v) + unsigned long long(v) ).isULongLong() && ( NumVariant(  unsigned long(v) ) + NumVariant( unsigned long long(v) ) ).isULongLong() );
    EXPECT_TRUE( NumVariant( unsigned long(v) + int(v) ).isULong() && ( NumVariant(  unsigned long(v) ) + NumVariant( int(v) ) ).isULong() );
    EXPECT_TRUE( NumVariant( unsigned long(v) + unsigned int(v) ).isULong() && ( NumVariant(  unsigned long(v) ) + NumVariant( unsigned int(v) ) ).isULong() );
    EXPECT_TRUE( NumVariant( unsigned long(v) + float(v) ).isFloat() && ( NumVariant(  unsigned long(v) ) + NumVariant( float(v) ) ).isFloat() );
    EXPECT_TRUE( NumVariant( unsigned long(v) + double(v) ).isDouble() && ( NumVariant(  unsigned long(v) ) + NumVariant( double(v) ) ).isDouble() );

    EXPECT_TRUE( NumVariant( long long(v) + long long(v) ).isLongLong() && ( NumVariant( long long(v) ) + NumVariant( long long(v) ) ).isLongLong() );
    EXPECT_TRUE( NumVariant( long long(v) + unsigned long long(v) ).isULongLong() && ( NumVariant(long long(v) ) + NumVariant( unsigned long long(v) ) ).isULongLong() );
    EXPECT_TRUE( NumVariant( long long(v) + int(v) ).isLongLong() && ( NumVariant( long long(v) ) + NumVariant( int(v) ) ).isLongLong() );
    EXPECT_TRUE( NumVariant( long long(v) + unsigned int(v) ).isLongLong() && ( NumVariant( long long(v) ) + NumVariant( unsigned int(v) ) ).isLongLong() );
    EXPECT_TRUE( NumVariant( long long(v) + float(v) ).isFloat() && ( NumVariant( long long(v) ) + NumVariant( float(v) ) ).isFloat() );
    EXPECT_TRUE( NumVariant( long long(v) + double(v) ).isDouble() && ( NumVariant( long long(v) ) + NumVariant( double(v) ) ).isDouble() );

    EXPECT_TRUE( NumVariant( unsigned long long(v) + unsigned long long(v) ).isULongLong() && ( NumVariant(unsigned long long(v) ) + NumVariant( unsigned long long(v) ) ).isULongLong() );
    EXPECT_TRUE( NumVariant( unsigned long long(v) + int(v) ).isULongLong() && ( NumVariant(unsigned long long(v) ) + NumVariant( int(v) ) ).isULongLong() );
    EXPECT_TRUE( NumVariant( unsigned long long(v) + unsigned int(v) ).isULongLong() && ( NumVariant( unsigned long long(v) ) + NumVariant( unsigned int(v) ) ).isULongLong() );
    EXPECT_TRUE( NumVariant( unsigned long long(v) + float(v) ).isFloat() && ( NumVariant( unsigned long long(v) ) + NumVariant( float(v) ) ).isFloat() );
    EXPECT_TRUE( NumVariant( unsigned long long(v) + double(v) ).isDouble() && ( NumVariant( unsigned long long(v) ) + NumVariant( double(v) ) ).isDouble() );

    EXPECT_TRUE( NumVariant( int(v) + int(v) ).isInt() && ( NumVariant(int(v) ) + NumVariant( int(v) ) ).isInt() );
    EXPECT_TRUE( NumVariant( int(v) + unsigned int(v) ).isUInt() && ( NumVariant( int(v) ) + NumVariant( unsigned int(v) ) ).isUInt() );
    EXPECT_TRUE( NumVariant( int(v) + float(v) ).isFloat() && ( NumVariant( int(v) ) + NumVariant( float(v) ) ).isFloat() );
    EXPECT_TRUE( NumVariant( int(v) + double(v) ).isDouble() && ( NumVariant( int(v) ) + NumVariant( double(v) ) ).isDouble() );

    EXPECT_TRUE( NumVariant( unsigned int(v) + unsigned int(v) ).isUInt() && ( NumVariant( unsigned int(v) ) + NumVariant( unsigned int(v) ) ).isUInt() );
    EXPECT_TRUE( NumVariant( unsigned int(v) + float(v) ).isFloat() && ( NumVariant( unsigned int(v) ) + NumVariant( float(v) ) ).isFloat() );
    EXPECT_TRUE( NumVariant( unsigned int(v) + double(v) ).isDouble() && ( NumVariant( unsigned int(v) ) + NumVariant( double(v) ) ).isDouble() );

    EXPECT_TRUE( NumVariant( float(v) + float(v) ).isFloat() && ( NumVariant( float(v) ) + NumVariant( float(v) ) ).isFloat() );
    EXPECT_TRUE( NumVariant( float(v) + double(v) ).isDouble() && ( NumVariant( float(v) ) + NumVariant( double(v) ) ).isDouble() );

    EXPECT_TRUE( NumVariant( double(v) + double(v) ).isDouble() && ( NumVariant( double(v) ) + NumVariant( double(v) ) ).isDouble() );
}


INSTANTIATE_TEST_CASE_P( NumberSet, VariantConversionTest, ::testing::Values(0, 1, 0x7F, 0x7FFF, 0x7FFFFFFF, 0x7FFFFFFFFFFFFFFF, 0x80, 0x8000, 0x80000000, 0x8000000000000000, 0xFF, 0xFFFF, 0xFFFFFFFF, 0xFFFFFFFFFFFFFFFF ) );


class VariantOperationTests : public ::testing::Test 
{
};

TEST_F( VariantOperationTests, Equal )
{
    EXPECT_TRUE( NumVariant(0) == 0 );
    EXPECT_TRUE( 0 == NumVariant(0) );
    EXPECT_TRUE(NumVariant(0) == NumVariant(0) );

    EXPECT_FALSE( NumVariant(0) == 1 );
    EXPECT_FALSE( 1 == NumVariant(0) );
    EXPECT_FALSE(NumVariant(0) == NumVariant(1) );
}


TEST_F( VariantOperationTests, GtLt )
{
    EXPECT_TRUE( 2 < NumVariant(3) );
    EXPECT_TRUE( 2 <= NumVariant(2) );
    EXPECT_FALSE( 2 < NumVariant(2) );
    EXPECT_TRUE( 3 > NumVariant(2) );
    EXPECT_TRUE( 3 >= NumVariant(3) );
    EXPECT_FALSE( 2 > NumVariant(2) );

    EXPECT_TRUE( NumVariant(1) == true );
    EXPECT_FALSE( NumVariant(0) == true );
    EXPECT_TRUE(false == NumVariant(0));
}


TEST_F( VariantOperationTests, BoolAnd )
{
    EXPECT_TRUE( NumVariant(1) && 1 );
    EXPECT_TRUE( 1 && NumVariant(1) );
    EXPECT_TRUE(NumVariant(1) && NumVariant(1) );

    EXPECT_FALSE( NumVariant(0) && 1 );
    EXPECT_FALSE( 1 && NumVariant(0) );
    EXPECT_FALSE(NumVariant(0) && NumVariant(1) );
}

TEST_F( VariantOperationTests, BoolOr )
{
    EXPECT_TRUE( NumVariant(1) || 1 );
    EXPECT_TRUE( 0 || NumVariant(1) );
    EXPECT_TRUE(NumVariant(1) || NumVariant(1) );
    EXPECT_FALSE(NumVariant(0) || NumVariant(0) );
}

TEST_F( VariantOperationTests, UnarOpp )
{
  //  EXPECT_TRUE( NumVariant(1) );
  //  EXPECT_FALSE( NumVariant(0) );

    EXPECT_TRUE( !NumVariant(0) );
    EXPECT_FALSE( !NumVariant(1) );

    int a = 1;
    EXPECT_EQ( ++NumVariant(1), ++a);

    int b = 1;
    EXPECT_EQ( NumVariant(1)++, b++ );

    int c = 1;
    EXPECT_EQ( --NumVariant(1), --c );

    int d = 1;
    EXPECT_EQ( NumVariant(1)--, d-- );

    EXPECT_EQ( ~NumVariant(0), ~0 );

    EXPECT_EQ( -NumVariant(1000), -1000 );
}

TEST_F( VariantOperationTests, LeftShift )
{
    EXPECT_EQ( 1 << 2, NumVariant(1) << 2 );
    EXPECT_EQ( 1 << 2, 1 << NumVariant(2) );
    EXPECT_EQ( NumVariant( -1 << 2UL ).isSigned(), ( NumVariant(-1) << 2UL ).isSigned() );
}


TEST_F( VariantOperationTests, RightShift )
{
    EXPECT_EQ( 0x1000 >> 2, NumVariant(0x1000) >> 2 );
    EXPECT_EQ( 0x1000 >> 2, 0x1000 >> NumVariant(2) );
    EXPECT_EQ( NumVariant( -1 >> 2UL ).isSigned(), ( NumVariant(-1) >> 2UL ).isSigned() );
}


TEST_F( VariantOperationTests, isSigned )
{
    EXPECT_TRUE( NumVariant( char(0) ).isSigned() );
    EXPECT_FALSE( NumVariant( unsigned char(0) ).isSigned() );
    EXPECT_TRUE( NumVariant( short(0) ).isSigned() );
    EXPECT_FALSE( NumVariant( unsigned short(0) ).isSigned() );
    EXPECT_TRUE( NumVariant( long(0) ).isSigned() );
    EXPECT_FALSE( NumVariant( unsigned long(0) ).isSigned() );
    EXPECT_TRUE( NumVariant( long long(0) ).isSigned() );
    EXPECT_FALSE( NumVariant( unsigned long long(0) ).isSigned() );
    EXPECT_TRUE( NumVariant( int(0) ).isSigned() );
    EXPECT_FALSE( NumVariant( unsigned int(0) ).isSigned() );
    EXPECT_TRUE( NumVariant( float(0.0f) ).isSigned() );
    EXPECT_TRUE( NumVariant( double(0) ).isSigned() );
}

TEST_F( VariantOperationTests, isInteger )
{
    EXPECT_TRUE( NumVariant( char(0) ).isInteger() );
    EXPECT_TRUE( NumVariant( unsigned char(0) ).isInteger() );
    EXPECT_TRUE( NumVariant( short(0) ).isInteger() );
    EXPECT_TRUE( NumVariant( unsigned short(0) ).isInteger() );
    EXPECT_TRUE( NumVariant( long(0) ).isInteger() );
    EXPECT_TRUE( NumVariant( unsigned long(0) ).isInteger() );
    EXPECT_TRUE( NumVariant( long long(0) ).isInteger() );
    EXPECT_TRUE( NumVariant( unsigned long long(0) ).isInteger() );
    EXPECT_TRUE( NumVariant( int(0) ).isInteger() );
    EXPECT_TRUE( NumVariant( unsigned int(0) ).isInteger() );
    EXPECT_FALSE( NumVariant( float(0.0f) ).isInteger() );
    EXPECT_FALSE( NumVariant( double(0) ).isInteger() );
}

