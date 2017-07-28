#pragma once

#include "procfixture.h"

#include "kdlib/typeinfo.h"
#include "kdlib/exceptions.h"
#include "kdlib/memaccess.h"

#include "test/testvars.h"

using namespace kdlib;


class TypeInfoTest : public ProcessFixture 
{
public:

    TypeInfoTest() : ProcessFixture( L"typetest" ) {}

};

TEST_F( TypeInfoTest, Ctor )
{
    EXPECT_EQ( L"structTest", loadType( L"structTest" )->getName() );
    EXPECT_EQ( L"structTest", loadType( L"targetapp!structTest" )->getName() );
    EXPECT_EQ( L"structTest", loadType( L"g_structTest" )->getName() );
    EXPECT_EQ( L"structTest", loadType( L"targetapp!g_structTest" )->getName() );

    EXPECT_THROW( loadType( L"nonExistingSymbol" ), DbgException );
}

TEST_F( TypeInfoTest, BaseTypeNames )
{
    EXPECT_EQ( L"Int1B", loadType( L"Int1B" )->getName() );
    EXPECT_EQ( L"Int2B", loadType( L"Int2B" )->getName() );
    EXPECT_EQ( L"Int4B", loadType( L"Int4B" )->getName() );
    EXPECT_EQ( L"Int8B", loadType( L"Int8B" )->getName() );
    EXPECT_EQ( L"UInt1B", loadType( L"UInt1B" )->getName() );
    EXPECT_EQ( L"UInt2B", loadType( L"UInt2B" )->getName() );
    EXPECT_EQ( L"UInt4B", loadType( L"UInt4B" )->getName() );
    EXPECT_EQ( L"UInt8B", loadType( L"UInt8B" )->getName() );

    EXPECT_EQ( L"Long", loadType( L"Long" )->getName() );
    EXPECT_EQ( L"ULong", loadType( L"ULong" )->getName() );
    EXPECT_EQ( L"Char", loadType( L"Char" )->getName() );
    EXPECT_EQ( L"WChar", loadType( L"WChar")->getName() );
    EXPECT_EQ( L"Bool", loadType( L"Bool")->getName() );
    EXPECT_EQ( L"Float", loadType( L"Float")->getName() );
    EXPECT_EQ( L"Double", loadType( L"Double")->getName() );

    EXPECT_EQ( L"Bool", loadType(L"Bool")->getName() );
}

TEST_F( TypeInfoTest, BaseTypeSizes)
{
    EXPECT_EQ( 1, loadType( L"Int1B" )->getSize() );
    EXPECT_EQ( 2, loadType( L"Int2B" )->getSize() );
    EXPECT_EQ( 4, loadType( L"Int4B" )->getSize() );
    EXPECT_EQ( 8, loadType( L"Int8B" )->getSize() );
    EXPECT_EQ( 1, loadType( L"UInt1B" )->getSize() );
    EXPECT_EQ( 2, loadType( L"UInt2B" )->getSize() );
    EXPECT_EQ( 4, loadType( L"UInt4B" )->getSize() );
    EXPECT_EQ( 8, loadType( L"UInt8B" )->getSize() );

    EXPECT_EQ( 4, loadType( L"Long" )->getSize() );
    EXPECT_EQ( 4, loadType( L"ULong" )->getSize() );
    EXPECT_EQ( 1, loadType( L"Char" )->getSize() );
    EXPECT_EQ( 2, loadType( L"WChar")->getSize() );
    EXPECT_EQ( 1, loadType( L"Bool")->getSize() );
    EXPECT_EQ( 4, loadType( L"Float")->getSize() );
    EXPECT_EQ( 8, loadType( L"Double")->getSize() );
}

TEST_F( TypeInfoTest, BaseTypeVars)
{
    EXPECT_EQ( L"Char", loadType( L"charVar" )->getName() );
    EXPECT_EQ( L"WChar", loadType( L"wcharVar" )->getName() );
    EXPECT_EQ( L"Int2B", loadType( L"shortVar" )->getName() );
    EXPECT_EQ( L"Int4B", loadType( L"longVar" )->getName() );
    EXPECT_EQ( L"Int8B", loadType( L"longlongVar" )->getName() );
    EXPECT_EQ( L"UInt1B", loadType( L"ucharVar" )->getName() );
    EXPECT_EQ( L"UInt2B", loadType( L"ushortVar" )->getName() );
    EXPECT_EQ( L"UInt4B", loadType( L"ulongVar" )->getName() );
    EXPECT_EQ( L"UInt8B", loadType( L"ulonglongVar" )->getName() );
    EXPECT_EQ( L"Bool", loadType( L"boolVar" )->getName() );
}

TEST_F( TypeInfoTest, BaseTypeArray )
{
    EXPECT_EQ( L"Int2B[2]", loadType( L"Int2B[2]" )->getName() );
    EXPECT_EQ( 2*2, loadType( L"Int2B[2]" )->getSize() );
    EXPECT_EQ( L"Int4B[2][3]", loadType( L"Int4B[2][3]" )->getName() );
    EXPECT_EQ( 4*2*3, loadType( L"Int4B[2][3]" )->getSize() );

    EXPECT_EQ( L"Int4B[2][3]", loadType(L"intMatrix")->getName() );
    EXPECT_EQ( sizeof(intMatrix), loadType(L"intMatrix")->getSize() ); 
        
    EXPECT_THROW( loadType( L"Int2B[1" ), TypeException );
    EXPECT_THROW( loadType( L"Int2B[1[1]]" ), TypeException );
    EXPECT_THROW( loadType( L"Int2B[-1]" ), TypeException );
}

TEST_F( TypeInfoTest, ComplexTypeArray )
{
    EXPECT_EQ( L"structTest[2]", loadType( L"structTest[2]")->getName() );
    EXPECT_TRUE( loadType( L"structTest[2]")->isArray() );
    EXPECT_EQ( loadType( L"structTest")->getSize()*2*3, loadType( L"structTest[2][3]")->getSize());

    EXPECT_THROW( loadType( L"structTest[2][-1]"), TypeException );
}


TEST_F( TypeInfoTest, BaseTypePointer )
{
    EXPECT_EQ( L"UInt1B*", loadType( L"UInt1B*" )->getName() );
    EXPECT_EQ( ptrSize(), loadType( L"UInt1B*" )->getSize() );
    EXPECT_EQ( L"UInt1B***", loadType( L"UInt1B***" )->getName() );
    EXPECT_EQ( ptrSize(), loadType( L"UInt1B***" )->getSize() );

    EXPECT_EQ( L"UInt8B*", loadType( L"pbigValue")->getName() );
    EXPECT_EQ( ptrSize(), loadType( L"pbigValue")->getSize() );
    EXPECT_EQ( L"UInt8B**", loadType( L"ppbigValue")->getName() );
    EXPECT_EQ( ptrSize(), loadType( L"ppbigValue")->getSize() );

    EXPECT_EQ( L"UInt1B*", loadType( L"UInt1B(*)" )->getName() );
    EXPECT_THROW( loadType( L"Int2B(*" ), TypeException );
    EXPECT_THROW( loadType( L"Int2B*)" ), TypeException );
}

TEST_F( TypeInfoTest, Void )
{
    EXPECT_EQ( L"Void*", loadType( L"Void*" )->getName() );
    EXPECT_EQ( L"Void**", loadType( L"Void**" )->getName() );
    EXPECT_EQ( L"Void*", loadType( L"voidPtr" )->getName() );
}

TEST_F( TypeInfoTest, UdtGetField )
{
    TypeInfoPtr  structType;
    
    structType = loadType( L"structTest" );
    EXPECT_EQ( L"UInt4B", structType->getElement(L"m_field0")->getName() );

    structType = loadType( L"structWithNested" );
    EXPECT_NO_THROW( structType->getElement(L"m_field")->getName() );
    EXPECT_THROW( structType->getElement(L"m_nestedFiled")->getName(), TypeException );
    EXPECT_NO_THROW( structType->getElement(L"m_field3")->getName() );
    EXPECT_NO_THROW( structType->getElement(L"m_unnameStruct.m_field2")->getName() );
}

TEST_F( TypeInfoTest, Const )
{
    EXPECT_EQ( L"Bool", loadType( L"g_constBoolValue")->getName() );   
    EXPECT_TRUE( loadType( L"g_constNumValue")->isConstant() );
    EXPECT_EQ( 0x5555, *loadType( L"g_constNumValue") );
    EXPECT_THROW( ulongVar == (NumVariant)*loadType( L"ulongVar"), TypeException );
}

TEST_F( TypeInfoTest, Array )
{
    TypeInfoPtr  array1;
    TypeInfoPtr  array2;
    
    EXPECT_NO_THROW( array1 = loadType( L"ulonglongArray" ) );
    EXPECT_NO_THROW( array2 = loadType( L"arrIntMatrixPtrs" ) );
    
    EXPECT_EQ( sizeof(ulonglongArray), array1->getSize() );
    EXPECT_EQ( sizeof(arrIntMatrixPtrs), array2->getSize() );

    EXPECT_EQ( TEST_ARRAY_SIZE, array1->getElementCount() );
    EXPECT_EQ( 4, array2->getElementCount() );

    EXPECT_EQ(loadType(L"UInt8B")->getSize(), array1->getElement(0)->getSize());
    EXPECT_EQ(loadType(L"UInt8B")->getSize(), array1->getElement(TEST_ARRAY_SIZE)->getSize());
}

TEST_F( TypeInfoTest, CppRef )
{
    TypeInfoPtr  ref1;
    TypeInfoPtr  ref2;
    
    EXPECT_NO_THROW( ref1 = loadType( L"ref_bigValue" ) );
    EXPECT_NO_THROW( ref2 = loadType( L"ref_pbigValue" ) );

    EXPECT_TRUE( ref1->isPointer() && ref2->isPointer() );
}

TEST_F( TypeInfoTest, Typedef )
{
    EXPECT_EQ( L"structTest", loadType( L"g_structTypeDef" )->getName() );
    EXPECT_EQ( L"structTest", loadType( L"structTestTypeDef" )->getName() );
}

TEST_F( TypeInfoTest, Enum )
{
    EXPECT_NO_THROW( loadType(L"enumType") );
    EXPECT_TRUE( loadType(L"enumType")->isEnum() );
    EXPECT_EQ( TWO, *loadType(L"enumType")->getElement(L"TWO") );
    EXPECT_TRUE( (NumVariant)*loadType(L"enumType")->getElement(L"TWO") == TWO );
}

TEST_F( TypeInfoTest, BitField )
{    
    TypeInfoPtr bitFieldStruct;
    TypeInfoPtr bitField;

    EXPECT_NO_THROW( bitFieldStruct = loadType(L"g_structWithBits") );
    EXPECT_NO_THROW( bitField = bitFieldStruct->getElement(L"m_bit5") );
    EXPECT_TRUE( bitField->isBitField() );
    EXPECT_EQ( 5, bitField->getBitOffset() );
    EXPECT_EQ( 1, bitField->getBitWidth() );
    EXPECT_EQ( 3, bitFieldStruct->getElement(L"m_bit6_8")->getBitWidth() );
}

TEST_F( TypeInfoTest, StaticField )
{
    TypeInfoPtr  typeInfo;
    TypeInfoPtr  fieldType;

    EXPECT_NO_THROW( typeInfo = loadType( L"g_classChild" ) );
    EXPECT_NO_THROW( fieldType = typeInfo->getElement(L"m_staticField") );

    EXPECT_THROW( typeInfo->getElementOffset(L"m_staticField"), TypeException );
    EXPECT_EQ( classChild::m_staticField,ptrSignDWord( typeInfo->getElementVa(L"m_staticField") ) );
}

TEST_F(  TypeInfoTest, DISABLED_StaticConstField )
{
    TypeInfoPtr  typeInfo;
    EXPECT_NO_THROW( typeInfo = loadType( L"g_classChild" ) );
    EXPECT_EQ( classChild::m_staticConst, *typeInfo->getElement(L"m_staticConst") );
}


TEST_F( TypeInfoTest, GetElement )
{
    EXPECT_THROW( loadType(L"Int1B")->getElement(L"nofiled"), TypeException );
    EXPECT_EQ( L"UInt4B", loadType(L"structTest")->getElement(L"m_field0")->getName() );
    EXPECT_EQ( L"Int4B", loadType(L"structWithNested")->getElement(L"m_unnameStruct.m_field2")->getName() );
    EXPECT_THROW( loadType(L"structWithNested")->getElement(L"m_nestedFiled"), TypeException);
}

TEST_F( TypeInfoTest, GetElementOffset )
{
    EXPECT_THROW( loadType(L"Int1B")->getElementOffset(L"nofiled"), TypeException );
    EXPECT_EQ( FIELD_OFFSET( structTest, m_field3 ), loadType(L"structTest")->getElementOffset(L"m_field3") );
    EXPECT_EQ( FIELD_OFFSET( structWithNested, m_unnameStruct.m_field2 ), loadType(L"structWithNested")->getElementOffset(L"m_unnameStruct.m_field2") );
}

TEST_F( TypeInfoTest, GetPtrSize )
{
    EXPECT_EQ( sizeof( &charVar ), loadType( L"charVar" )->getPtrSize() );
    EXPECT_EQ( sizeof( g_structTestPtr ), loadType( L"g_structTestPtr" )->getPtrSize() );
    EXPECT_EQ( sizeof( &g_testArray ), loadType( L"g_testArray" )->getPtrSize() );
}

TEST_F( TypeInfoTest, GetElementCount )
{
    EXPECT_THROW( loadType( L"UInt1B" )->getElementCount(), TypeException );
    EXPECT_THROW( loadType( L"UInt1B*" )->getElementCount(), TypeException );
    EXPECT_EQ( 20, loadType( L"UInt1B[20]" )->getElementCount() );
    EXPECT_EQ( 5, loadType( L"g_structTest" )->getElementCount() );
    EXPECT_EQ( sizeof(g_testArray)/sizeof(g_testArray[0]), loadType( L"g_testArray" )->getElementCount() );
    EXPECT_EQ( 3, loadType( L"enumType" )->getElementCount() );
}

TEST_F( TypeInfoTest, PtrTo )
{
    TypeInfoPtr typeInfo;
    EXPECT_NO_THROW( typeInfo = loadType( L"UInt1B" )->ptrTo() );
    EXPECT_EQ( L"UInt1B*", typeInfo->getName() );
    EXPECT_NO_THROW( typeInfo = loadType( L"g_structTestPtr" )->ptrTo() );
    EXPECT_EQ( L"structTest**", typeInfo->getName() );
    EXPECT_NO_THROW( typeInfo = loadType( L"g_constEnumThree" )->ptrTo() );
    EXPECT_EQ( L"enumType*", typeInfo->getName() );
}

TEST_F( TypeInfoTest, Deref )
{
    TypeInfoPtr typeInfo;
    EXPECT_NO_THROW( typeInfo = loadType( L"UInt1B*" )->deref() );
    EXPECT_EQ( L"UInt1B", typeInfo->getName() );
    EXPECT_NO_THROW( typeInfo = loadType( L"g_structTestPtr" )->deref() );
    EXPECT_EQ( L"structTest", typeInfo->getName() );
    EXPECT_NO_THROW( typeInfo = loadType( L"enumType*" )->deref() );
    EXPECT_EQ( L"enumType", typeInfo->getName() );
}


TEST_F( TypeInfoTest, ArrayOf )
{
    TypeInfoPtr typeInfo;
    ASSERT_NO_THROW( typeInfo = loadType( L"UInt1B" )->arrayOf(10) );
    EXPECT_EQ( L"UInt1B[10]", typeInfo->getName() );
    ASSERT_NO_THROW( typeInfo = loadType( L"g_structTestPtr" )->arrayOf(1) );
    EXPECT_EQ( L"structTest*[1]", typeInfo->getName() );
    ASSERT_NO_THROW( typeInfo = loadType( L"enumType*" )->arrayOf(0) );
    EXPECT_EQ( L"enumType*[0]", typeInfo->getName() );
}


TEST_F( TypeInfoTest, SizeOf )
{
    EXPECT_EQ( sizeof(ucharVar), getSymbolSize(L"ucharVar") );
    EXPECT_EQ( sizeof(unsigned long)*10, getSymbolSize(L"ULong[10]") );
    EXPECT_EQ( sizeof(float*), getSymbolSize(L"Float*") );
    EXPECT_EQ( sizeof(structTest), getSymbolSize(L"structTest") );
    EXPECT_EQ( sizeof(structTest), getSymbolSize(L"targetapp!structTest") );
}

TEST_F(TypeInfoTest, EnumVirtualField)
{
    TypeInfoPtr typeInfo;
    ASSERT_NO_THROW( typeInfo = loadType( L"g_virtChild" ) );
    size_t  fieldCount;
    ASSERT_NO_THROW( fieldCount = typeInfo->getElementCount() );
    for ( size_t i = 0; i < fieldCount; ++i )
        EXPECT_NO_THROW( typeInfo->getElement(i) );
}


TEST_F(TypeInfoTest, GetVTBL)
{
    TypeInfoPtr vtblInfo;
    ASSERT_NO_THROW(vtblInfo = loadType(L"classBase1")->getVTBL() );
    EXPECT_TRUE(vtblInfo->isVtbl() );
    EXPECT_EQ(3, vtblInfo->getElementCount());
}

TEST_F( TypeInfoTest, VirtualMember )
{
    TypeInfoPtr typeInfo;
    ASSERT_NO_THROW( typeInfo = loadType( L"g_virtChild" ) );
    EXPECT_TRUE( typeInfo->isVirtualMember(L"m_baseField") );
}


TEST_F( TypeInfoTest, Function )
{
    TypeInfoPtr func;
    ASSERT_NO_THROW( func = loadType(L"CdeclFunc") );
    EXPECT_EQ( L"Void(__cdecl)(Int4B, Float)", func->getName() ); 
    EXPECT_EQ( 2, func->getElementCount() );
    EXPECT_EQ( L"Float", func->getElement(1)->getName() );
    
    TypeInfoPtr method;
    ASSERT_NO_THROW( method = loadType(L"classChild::virtMethod2") );
}

TEST_F( TypeInfoTest, Str )
{
    std::wstring str;
    ASSERT_NO_THROW( str = loadType(L"enumType")->str() );
}


#pragma pack (push, 8 )

struct DefineStructTest1  {

    unsigned char  field1;
    double  field2;
};

#pragma pack(pop)



#pragma pack (push, 1)

struct DefineStructTest2  {

    unsigned char  field1;
    double  field2;
};

#pragma pack(pop)


TEST_F( TypeInfoTest, DefineStruct )
{
    TypeInfoPtr  testStruct;

    ASSERT_NO_THROW( testStruct = defineStruct(L"TestStruct", 8) );
    ASSERT_NO_THROW( testStruct->appendField(L"field1", loadType(L"Int1B") ) );
    ASSERT_NO_THROW( testStruct->appendField(L"field2", loadType(L"Double") ) );
    EXPECT_EQ( sizeof(DefineStructTest1), testStruct->getSize() );
    EXPECT_EQ( FIELD_OFFSET(DefineStructTest1, field2), testStruct->getElementOffset(L"field2") );

    ASSERT_NO_THROW( testStruct = defineStruct(L"TestStruct", 1) );
    ASSERT_NO_THROW( testStruct->appendField(L"field1", loadType(L"Int1B") ) );
    ASSERT_NO_THROW( testStruct->appendField(L"field2", loadType(L"Double") ) );
    EXPECT_EQ( sizeof(DefineStructTest2), testStruct->getSize() );
    EXPECT_EQ( FIELD_OFFSET(DefineStructTest2, field2), testStruct->getElementOffset(L"field2") );

    ASSERT_NO_THROW( testStruct = defineStruct(L"TestStruct") );
    EXPECT_THROW( testStruct->appendField(L"field", testStruct ), TypeException );
    ASSERT_NO_THROW( testStruct->appendField(L"field", loadType(L"Bool") ) );
    EXPECT_THROW( testStruct->appendField(L"field", loadType(L"Long") ), TypeException );
}


TEST_F( TypeInfoTest, DefineUnion )
{
    TypeInfoPtr  testUnion;

    ASSERT_NO_THROW( testUnion = defineUnion(L"TestUnion") );
    ASSERT_NO_THROW( testUnion->appendField(L"field1", loadType(L"Int1B") ) );
    ASSERT_NO_THROW( testUnion->appendField(L"field2", loadType(L"Double") ) );
    EXPECT_EQ( sizeof(double), testUnion->getSize() );
    EXPECT_EQ( 0, testUnion->getElementOffset(L"field2") );

    ASSERT_NO_THROW( testUnion = defineUnion(L"TestUnion") );
    EXPECT_THROW( testUnion->appendField(L"field", testUnion ), TypeException );
    ASSERT_NO_THROW( testUnion->appendField(L"field", loadType(L"Bool") ) );
    EXPECT_THROW( testUnion->appendField(L"field", loadType(L"Long") ), TypeException );
}

TEST_F( TypeInfoTest, FunctionName )
{
    TypeInfoPtr ti;

    ASSERT_NO_THROW( ti = loadTypedVar(L"CdeclFuncPtr")->getType()->deref() );
    EXPECT_EQ(L"Void(__cdecl)(Int4B, Float)", ti->getName());

    ASSERT_NO_THROW( ti = loadTypedVar(L"MethodPtr")->getType()->deref() );
    EXPECT_EQ(L"Void(__thiscall FuncTestClass::)()", ti->getName() );

    ASSERT_NO_THROW(ti = loadTypedVar(L"FuncReturnClass")->getType());
    EXPECT_EQ(L"FuncTestClass(__cdecl)()", ti->getName());

    ASSERT_NO_THROW(ti = loadTypedVar(L"StdcallFunc")->getType());

#ifdef _WIN64
    EXPECT_EQ(L"Void(__cdecl)(Int4B, Float)", ti->getName());
#else
    EXPECT_EQ(L"Void(__stdcall)(Int4B, Float)", ti->getName());
#endif
    
    ASSERT_NO_THROW( ti = loadTypedVar(L"ArrayOfCdeclFuncPtr")->getType()->getElement(0)->deref() );
    EXPECT_EQ(L"Void(__cdecl)(Int4B, Float)", ti->getName());

    ASSERT_NO_THROW( ti = loadTypedVar(L"ArrayOfMethodPtr")->getType()->getElement(0)->deref() );
    EXPECT_EQ(L"Void(__thiscall FuncTestClass::)()", ti->getName());
}

TEST_F( TypeInfoTest, FunctionPtrName )
{
    TypeInfoPtr ti;

    ASSERT_NO_THROW( ti = loadTypedVar(L"CdeclFuncPtr")->getType() );
    EXPECT_EQ( 0, ti->getName().compare(L"Void(__cdecl*)(Int4B, Float)") );

    ASSERT_NO_THROW( ti = loadTypedVar(L"MethodPtr")->getType() );
    EXPECT_EQ( 0, ti->getName().compare(L"Void(__thiscall FuncTestClass::*)()") );
}

TEST_F( TypeInfoTest, FunctionArrName )
{
    TypeInfoPtr ti;

    ASSERT_NO_THROW( ti = loadTypedVar(L"ArrayOfCdeclFuncPtr")->getType() );
    EXPECT_EQ( 0, ti->getName().compare(L"Void(__cdecl*[3])(Int4B, Float)") );

    ASSERT_NO_THROW( ti = loadTypedVar(L"ArrayOfMethodPtr")->getType() );
    EXPECT_EQ( 0, ti->getName().compare(L"Void(__thiscall FuncTestClass::*[2])()") );
}

TEST_F(TypeInfoTest, ArrayOverflow)
{
    TypeInfoPtr  intType = loadType(L"Int2B");

    EXPECT_NO_THROW(intType->arrayOf(SIZE_MAX / intType->getSize()));
    EXPECT_EQ(SIZE_MAX / intType->getSize(), intType->arrayOf(SIZE_MAX / intType->getSize())->getElementCount());
    EXPECT_EQ(SIZE_MAX / intType->getSize() * intType->getSize(), intType->arrayOf(SIZE_MAX / intType->getSize())->getSize());
    EXPECT_NO_THROW(intType->arrayOf(0));
    EXPECT_EQ(0, intType->arrayOf(0)->getSize());

    EXPECT_THROW(intType->arrayOf(SIZE_MAX / intType->getSize() + 1), TypeException);
    EXPECT_THROW(intType->arrayOf(SIZE_MAX), TypeException);
    EXPECT_THROW(intType->arrayOf(-1), TypeException);

}

TEST_F(TypeInfoTest, DefineFunc)
{
    TypeInfoPtr  testFunction1;

    ASSERT_NO_THROW( testFunction1 = defineFunction(loadType(L"Int1B") ) );
    EXPECT_EQ(L"Int1B(__cdecl)()", testFunction1->getName());

    TypeInfoPtr  testFunction2;
    ASSERT_NO_THROW( testFunction2 = defineFunction(loadType(L"Char*") ) );
    ASSERT_NO_THROW( testFunction2->appendField(L"arg1", loadType(L"Int1B") ) );
    ASSERT_NO_THROW( testFunction2->appendField(L"arg2", loadType(L"Double") ) );

    EXPECT_EQ( 2, testFunction2->getElementCount() );
    EXPECT_EQ( L"Double", testFunction2->getElement(1)->getName() );
    EXPECT_EQ( L"Char*", testFunction2->getReturnType()->getName() );

    EXPECT_THROW( testFunction2->getElement(2), IndexException);
}

TEST_F(TypeInfoTest, GetBaseClass)
{
    TypeInfoPtr  g_classChild;
    ASSERT_NO_THROW( g_classChild = loadType(L"g_classChild") );

    EXPECT_EQ( 2, g_classChild->getBaseClassesCount() );
    EXPECT_EQ( L"classBase1", g_classChild->getBaseClass(L"classBase1")->getName() );
    EXPECT_EQ( L"classBase2", g_classChild->getBaseClass(1)->getName() );

    EXPECT_THROW( g_classChild->getBaseClass(2), IndexException);
    EXPECT_THROW( g_classChild->getBaseClass(L"invalidBaseName"), TypeException);
}

TEST_F(TypeInfoTest, GetBaseClassOffset)
{
    TypeInfoPtr  g_classChild;
    ASSERT_NO_THROW( g_classChild = loadType(L"g_classChild") );

    EXPECT_EQ( 0, g_classChild->getBaseClassOffset(0) );
    EXPECT_EQ( g_classChild->getBaseClass(0)->getSize(), g_classChild->getBaseClassOffset(1) );
    EXPECT_THROW( g_classChild->getBaseClassOffset(2), IndexException);

    EXPECT_EQ( g_classChild->getBaseClassOffset(0), g_classChild->getBaseClassOffset(L"classBase1") );
    EXPECT_EQ( g_classChild->getBaseClassOffset(1), g_classChild->getBaseClassOffset(L"classBase2") );
}

TEST_F(TypeInfoTest, IsBaseClassVirtual)
{
    EXPECT_TRUE( loadType(L"virtualBase1")->isBaseClassVirtual(L"classBase1") );
    EXPECT_FALSE( loadType(L"virtualChild")->isBaseClassVirtual(1) );
}

TEST_F(TypeInfoTest, GetMethod)
{
    TypeInfoPtr  g_classChild;

    ASSERT_NO_THROW( g_classChild = loadType(L"g_classChild") );

    EXPECT_TRUE( loadType(L"classChild::childMethod")->isFunction() );
    EXPECT_TRUE( g_classChild->getMethod(L"childMethod")->isFunction() );
    EXPECT_EQ( L"Int4B(__thiscall classChild::)(Int4B)", g_classChild->getMethod(L"childMethod")->getName() );

    EXPECT_THROW( loadType(L"classChild::baseMethod"), SymbolException );
    EXPECT_THROW( g_classChild->getElement(L"baseMethod"), TypeException );
    EXPECT_THROW( g_classChild->getElement(L"NotExistMethod"), TypeException );
}

TEST_F(TypeInfoTest, GetOverloadedMethod)
{
    TypeInfoPtr  g_classChild;
    ASSERT_NO_THROW( g_classChild = loadType(L"g_classChild") );

    EXPECT_EQ( L"Int4B(__thiscall classChild::)(Int4B)", g_classChild->getMethod(L"overloadMethod", L"Int4B(__thiscall classChild::)(Int4B)")->getName() );
    EXPECT_EQ( L"Int4B(__thiscall classChild::)(Int4B, Int4B)", g_classChild->getMethod(L"overloadMethod", L"Int4B(__thiscall classChild::)(Int4B, Int4B)")->getName() );

    EXPECT_THROW( g_classChild->getMethod(L"overloadMethod", L"Int4B(__thiscall classChild::)(Int4B, Float)"), TypeException );
}

TEST_F(TypeInfoTest, GetVirtualMethod)
{
    TypeInfoPtr g_virtChild;
    ASSERT_NO_THROW( g_virtChild = loadType( L"g_virtChild" ) );
    EXPECT_TRUE( g_virtChild->getMethod(L"virtMethod1")->isFunction() );
    EXPECT_TRUE( g_virtChild->getMethod(L"virtMethod1")->isVirtual() );
    EXPECT_FALSE( g_virtChild->getMethod(L"virtualChild")->isVirtual() );
    EXPECT_THROW( loadType(L"Int2B")->isVirtual(), TypeException );
}

TEST_F(TypeInfoTest, GetTypeProviderFromPdb)
{
    TypeInfoProviderPtr  typeInfoProvider;
    ASSERT_NO_THROW( typeInfoProvider = getTypeInfoProviderFromPdb( m_targetModule->getSymFile() ) );

    TypeInfoPtr  structType;
    ASSERT_NO_THROW( structType = typeInfoProvider->getTypeByName(L"structTest") );

    EXPECT_EQ( L"structTest", structType->getName() );
}


