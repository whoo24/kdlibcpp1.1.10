#pragma once

#include "procfixture.h"

#include "kdlib/typeinfo.h"

using namespace kdlib;

class ClangTest : public ProcessFixture
{
public:

    ClangTest() : ProcessFixture( L"typetest" ) {}

};

static const wchar_t test_code1[] = L"  \
    struct TestStruct {          \
        bool   a0;               \
        char   a1;               \
        unsigned char  a2;       \
        short  a3;               \
        unsigned short  a4;      \
        int    a5;               \
        unsigned  a6;            \
        long  a7;                \
        unsigned long  a8;       \
        long long      a9;       \
        unsigned long long  a10; \
        long long  a11;          \
        float  a12;              \
        double  a13;             \
        long double  a14;        \
        wchar_t  a15;            \
        unsigned wchar_t  a16;   \
        void*   b0;              \
        char*   b1;              \
        unsigned char   b2[10];  \
        short&  b3;              \
        const unsigned short  b4;\
        static short  b5;        \
        volatile int  b6;        \
    };";


TEST_F(ClangTest, CompileStruct)
{
    TypeInfoPtr  compiledStruct;

    ASSERT_NO_THROW( compiledStruct = compileType(test_code1, L"TestStruct") );

    EXPECT_TRUE( compiledStruct->isUserDefined() );
    EXPECT_TRUE( compiledStruct->getElement(L"b2")->isArray() );
    EXPECT_TRUE( compiledStruct->getElement(L"b0")->isPointer() );
    EXPECT_TRUE( compiledStruct->getElement(L"b0")->deref()->isVoid() );
    EXPECT_TRUE( compiledStruct->getElement(L"a3")->isBase() );

    EXPECT_EQ( L"TestStruct", compiledStruct->getName() );

    EXPECT_LT( 0, compiledStruct->getSize() );

    EXPECT_EQ( 24, compiledStruct->getElementCount() );

    EXPECT_EQ( L"Int4B", compiledStruct->getElement(L"a5")->getName() );
    EXPECT_EQ( L"Float", compiledStruct->getElement(L"a12")->getName() );
    EXPECT_EQ( L"a3", compiledStruct->getElementName(3) );
    EXPECT_EQ( L"a8", compiledStruct->getElementName(8) );

    EXPECT_EQ( 9, compiledStruct->getElementIndex( L"a9" ) );

    EXPECT_TRUE( compiledStruct->isStaticMember(L"b5") );
    EXPECT_FALSE( compiledStruct->isStaticMember(L"b6") );
    EXPECT_TRUE( compiledStruct->isStaticMember(22) );
    EXPECT_FALSE( compiledStruct->isStaticMember(23) );

   
    EXPECT_THROW( compiledStruct->getElement(L"aa")->getName(), TypeException );
    EXPECT_THROW( compiledStruct->getElement(25), IndexException );

    std::wstring  desc;
    EXPECT_NO_THROW( desc = compiledStruct->str() );
}


TEST_F(ClangTest, CompileOption)
{
    const size_t  defaultPtrSize = sizeof(void*);

    TypeInfoPtr  compiledStruct1, compiledStruct2, compiledStruct3;

    ASSERT_NO_THROW( compiledStruct1 = compileType(test_code1, L"TestStruct", L"--target=i686-pc-windows-msvc -w") );
    ASSERT_NO_THROW( compiledStruct2 = compileType(test_code1, L"TestStruct", L"--target=x86_64-pc-windows-msvc -w") );
    ASSERT_NO_THROW( compiledStruct3 = compileType(test_code1, L"TestStruct") );

    EXPECT_TRUE( compiledStruct1->getSize() < compiledStruct2->getSize() );
    EXPECT_EQ( 4, compiledStruct1->getElement(L"b1")->getSize());
    EXPECT_EQ( 8, compiledStruct2->getElement(L"b1")->getSize());
    EXPECT_EQ( defaultPtrSize, compiledStruct3->getElement(L"b1")->getSize());
}

static const wchar_t  test_code2[] = L"\
    struct TestStruct;                 \
    struct TestStruct {                \
        struct TestStruct*  next;      \
        int  value;                    \
    };                                 \
    ";

TEST_F(ClangTest, ForwardDecl)
{
    TypeInfoPtr  testStruct;
    ASSERT_NO_THROW( testStruct = compileType(test_code2, L"TestStruct") );
    EXPECT_TRUE( testStruct->getElement(L"next")->isPointer() );
    EXPECT_EQ( is64bitSystem() ? 8 : 4, testStruct->getElement(L"next")->getSize() );

    ASSERT_NO_THROW( testStruct = testStruct->getElement(L"next")->deref() );
    EXPECT_EQ( L"TestStruct", testStruct->getName() );

    std::wstring  desc;
    EXPECT_NO_THROW( desc = testStruct->getElement(L"next")->str() );
}

static const wchar_t test_code3[] = L" \
    struct Test1 {                     \
        int  a1;                       \
        int  a2;                       \
    };                                 \
                                       \
    struct Test2  {                    \
        int  b1;                       \
        struct Test1 b2;               \
        struct {                       \
            int  c1;                   \
            int  c2;                   \
        } b3;                          \
        struct {                       \
            int   c3;                  \
            int   c4;                  \
        };                             \
        union {                        \
            long  c5;                  \
            float  c6;                 \
        };                             \
        union {                        \
            char  c7;                  \
            short  c8;                 \
        } b4;                          \
        int  b5;                       \
        float;                         \
    };                                 \
    ";

TEST_F(ClangTest, Nested)
{
    TypeInfoPtr  testStruct;
    ASSERT_NO_THROW( testStruct = compileType(test_code3, L"Test2") );

    EXPECT_EQ( L"a2", testStruct->getElement(L"b2")->getElementName(1) );
    EXPECT_EQ( L"c2", testStruct->getElement(L"b3")->getElementName(1) );
    EXPECT_EQ( L"c4", testStruct->getElementName(4) );
    EXPECT_EQ( L"c6", testStruct->getElementName(6) );
    EXPECT_EQ( L"c8", testStruct->getElement(L"b4")->getElementName(1) );

    std::wstring  desc;
    EXPECT_NO_THROW( desc = testStruct->str() );
}


static const wchar_t test_code4[] = L"\
    union Test {                      \
        int  a1;                      \
        unsigned long long  a2;       \
        float  a3;                    \
    };                                \
    ";


TEST_F(ClangTest, Union)
{
    TypeInfoPtr  testUnion;
    ASSERT_NO_THROW( testUnion = compileType(test_code4, L"Test") );

    std::wstring  desc;
    EXPECT_NO_THROW( desc = testUnion->str() );
}


static const wchar_t test_code5[] = L"\
    struct Test {                     \
        int  a1 : 11;                 \
        int  a2 : 8;                  \
        int  a3 : 31;                 \
    };                                \
    ";

TEST_F(ClangTest, BitField)
{
    TypeInfoPtr  testStruct;
    ASSERT_NO_THROW( testStruct = compileType(test_code5, L"Test") );

    EXPECT_TRUE( testStruct->getElement(L"a2")->isBitField() );
    EXPECT_EQ( 11, testStruct->getElement(L"a2")->getBitOffset() );
    EXPECT_EQ( 8, testStruct->getElement(L"a2")->getBitWidth() );

    std::wstring  desc;
    EXPECT_NO_THROW( desc = testStruct->str() );
}

static const wchar_t test_code6[] = L"\
    namespace ns1 {                   \
    namespace ns2 {                   \
       struct Test {                  \
           int  f1;                   \
       };                             \
    } }                               \
    ";

TEST_F(ClangTest, Namespace)
{
    TypeInfoPtr  testStruct;
    ASSERT_NO_THROW( testStruct = compileType(test_code6, L"ns1::ns2::Test") );

    std::wstring  desc;
    EXPECT_NO_THROW( desc = testStruct->str() );
}

static const wchar_t test_code7[] = L"\
    class Test {                      \
        long long mem1;               \
     };                               \
    ";

TEST_F(ClangTest, SimpleClass)
{
    TypeInfoPtr  testClass;
    ASSERT_NO_THROW( testClass = compileType(test_code7, L"Test") );

    std::wstring  desc;
    EXPECT_NO_THROW( desc = testClass->str() );
}

TEST_F(ClangTest, InvalidDef)
{
    EXPECT_THROW( compileType( L"struct Test { charintfloatdouble  mem1; };", L"Test")->str(), TypeException);
//    EXPECT_THROW( compileType( L"struct Test { char mem1;", L"Test")->str(), TypeException);
    EXPECT_THROW( compileType( L"aaaaaa func(bbbb p1);", L"func")->str(), TypeException);
}

TEST_F(ClangTest, StructNoDef)
{
    TypeInfoPtr  structNoDef;
    ASSERT_NO_THROW( structNoDef = compileType(L"struct StructNoDef;", L"StructNoDef") );

    EXPECT_EQ(L"StructNoDef", structNoDef->getName());

    std::wstring desc;
    EXPECT_NO_THROW(desc = structNoDef->str() );

    EXPECT_THROW(structNoDef->getElementCount(), TypeException);
    EXPECT_THROW(structNoDef->getElement(0), TypeException);
}

TEST_F(ClangTest, DISABLED_Function)
{
    const std::wstring  src = L"#include \"../../../kdlib/include/test/testfunc.h\"";

    EXPECT_EQ( L"Void(__cdecl)(Int4B, Float)", compileType(src, L"CdeclFunc")->getName());
    EXPECT_EQ( L"Void(__stdcall)(Int4B, Float)", compileType(src, L"StdcallFunc")->getName());
}

TEST_F(ClangTest, Include)
{

    const std::wstring  src = L"#include \"../../../kdlib/include/test/testvars.h\"";

    TypeInfoProviderPtr  typeProvider;

    ASSERT_NO_THROW( typeProvider = getTypeInfoProviderFromSource(src) );

    for ( auto typeName : {L"structTest", L"structWithNested", L"structWithArray", L"unionTest", L"structWithBits", L"structWithSignBits",
        /*L"testspace::testClass1",*/ L"structWithEnum", L"enumType" } )
    {
        TypeInfoPtr  structFromSrc, structFromPdb;
        std::wstring  str1, str2;

        ASSERT_NO_THROW( structFromSrc = typeProvider->getTypeByName(typeName) );
        ASSERT_NO_THROW( structFromPdb = loadType(typeName) );
        EXPECT_NO_THROW( str1 = structFromSrc->str() );
        EXPECT_NO_THROW( str2 = structFromPdb->str() );
        EXPECT_TRUE( str1 == str2 );
    }
}


TEST_F(ClangTest, WindowsH)
{
    std::wstring  src = L"#include <windows.h>\r\n";
    std::wstring  opt = L"-I\"C:/Program Files (x86)/Windows Kits/8.1/Include/um\" \
        -I\"C:/Program Files (x86)/Windows Kits/8.1/Include/shared\" -w";

    TypeInfoProviderPtr  typeProvider;

    ASSERT_NO_THROW( typeProvider = getTypeInfoProviderFromSource(src, opt) );

    for ( auto typeName :  {L"tagPOINT", L"tagWNDCLASSA", L"CreateFileA"} )
    {
        TypeInfoPtr  type1;
        ASSERT_NO_THROW( type1 = typeProvider->getTypeByName(typeName) );

        std::wstring  desc;
        EXPECT_NO_THROW( desc = type1->str() );
    }
}


TEST_F(ClangTest, NtddkH)
{
    std::wstring  src = L"#include <ntddk.h>\r\n";
    std::wstring  opt = L"-I\"C:/Program Files (x86)/Windows Kits/8.1/Include/km\" \
        -I\"C:/Program Files (x86)/Windows Kits/8.1/Include/shared\" \
        -D_AMD64_ --target=x86_64-pc-windows-msvc -w";

    TypeInfoProviderPtr  typeProvider;

    ASSERT_NO_THROW( typeProvider = getTypeInfoProviderFromSource(src, opt) );

    for ( auto typeName :  {L"_UNICODE_STRING", L"_IRP", L"_MDL", L"_FILE_OBJECT", L"_DEVICE_OBJECT", L"_PS_CREATE_NOTIFY_INFO",
     L"ZwCreateFile"} )
    {
        TypeInfoPtr  type1;
        ASSERT_NO_THROW( type1 = typeProvider->getTypeByName(typeName) );

        std::wstring  desc;
        EXPECT_NO_THROW( desc = type1->str() );
    }
}


