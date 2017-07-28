#include "stdafx.h"

#include <atlexcept.h>
#include <list>

#include "test/testvars.h"

#pragma pack( push,4 )

const unsigned long g_constNumValue = 0x5555;
const bool g_constBoolValue = true;

unsigned char ucharVar = 10;
unsigned short ushortVar = 1020;
unsigned long ulongVar = 10002000;
unsigned long long ulonglongVar = 1234567890;
char charVar = -5;
wchar_t wcharVar = L'G';
short shortVar = -1020;
long longVar = -1002000;
long long longlongVar = -1234567890;
float floatVar = -5.99f;
double doubleVar = 6.00000001;
bool boolVar = true;


const unsigned long ulongConst = 0x5555;
const unsigned long long ulonglongConst = 0xffffff000000;
const bool boolConst = true;

char  helloStr[] = "Hello";
wchar_t  helloWStr[] = L"Hello";
char  helloStr1[] = "Hello";

unsigned char ucharArray[] = {0, 10, 0x78, 128, 0xFF };
unsigned short ushortArray[] = {0, 10, 0xFF, 0x8000, 0xFFFF };
unsigned long ulongArray[] = {0, 0xFF, 0x8000, 0x80000000, 0xFFFFFFFF };
unsigned long long ulonglongArray[] = {0, 0xFF, 0xFFFFFFFF, 0x8000000000000000, 0xFFFFFFFFFFFFFFFF };
char charArray[] = { 0, -10, 80, -87, 127 };
short shortArray[] = { 0, -1, -128, 32000, 0x7FFF };
long longArray[] = { 0, -1, 1, 0x7FFFFFFF, 0x80000000 };
long long longlongArray[] = { 100, -200, 123456789, -123456789, -1 };
float floatArray[] = { 1.0f, 0.001f, -199.999f, 20000.01f, 0.111111f };
double doubleArray[] = { 1.0000000, 0.0000000001, -199.99999999998, 200000.00000001, 0.3333333333 };

unsigned char ucharArrayPlace[] = { 0 };
unsigned short ushortArrayPlace[] = { 0 };
unsigned long ulongArrayPlace[] = { 0 };
unsigned long long ulonglongArrayPlace[] = { 0 };
char charArrayPlace[] = { 0 };
short shortArrayPlace[] = { 0 };
long longArrayPlace[] = { 0 };
long long longlongArrayPlace[] = { 0 };
float floatArrayPlace[] = { 0 };
double doubleArrayPlace[] = { 0 };

unsigned long long bigValue = 0x8080808080808080;
unsigned long long *pbigValue = &bigValue;
unsigned long long **ppbigValue = &pbigValue;

float floatValue = -0.1f;
double doubleValue = -0.1;

unsigned long long ullValuePlace = 0;
float floatValuePlace = 0;
double doubleValuePlace = 0;

unsigned long long &ref_bigValue = bigValue;
unsigned long long *&ref_pbigValue = pbigValue;

void* voidPtr = &bigValue;
void* voidPtrArray[] = { voidPtr, voidPtr, voidPtr };

structTest      g_structTest = { 0, 500, true, 1, NULL };
structTest      g_structTest1 = { 0, 500, true, 1, &g_structTest };
structTest      g_testArray[2] = { { 0, 500, true, 1 }, { 2, 1500, false, 1 } };
structTest      *g_structTestPtr = &g_structTest;
structTest      **g_structTestPtrPtr = &g_structTestPtr;
structTestTypeDef    g_structTypeDef = {};


int intMatrix[2][3] = { { 0, 1, 2}, { 3, 4, 5 } };
int intMatrix2[2][3] = { { 0, 1, 2}, { 3, 4, 5 } };
int intMatrix3[2][3] = { { 0, 1, 2}, { 3, 4, 5 } };
int intMatrix4[2][3] = { { 0, 1, 2}, { 3, 4, 5 } };
int (*ptrIntMatrix)[2][3] = &intMatrix;
int (* arrIntMatrixPtrs[4])[2][3] = { &intMatrix, &intMatrix2, &intMatrix3, &intMatrix4 };
char* strArray[] = { "Hello", "Bye" };
char *(*ptrStrArray)[2] = &strArray;


template<typename T>
class MemSetter {
public:

    MemSetter( T* tptr, size_t length, const T& value = T() )
    {
        for ( size_t i = 0; i < length; ++i)
            tptr[i] = value;
    }
};


char bigCStr[0x2000 + 1] = {};
MemSetter<char>   m1 = MemSetter<char>( bigCStr, 0x2000, 'a' );

wchar_t bigWStr[0x2000 + 1] = {};
MemSetter<wchar_t>  m2 = MemSetter<wchar_t>( bigWStr, 0x2000, L'a' );

structWithNested g_structWithNested = { 10, { 20 }, 100500 };
structWithNested::Nested g_structNested = { 300 };

enumType g_constEnumThree = THREE;

structWithEnum g_structWithEnum = {ONE};

structWithBits g_structWithBits = { 4, 1, 5 };
structWithSignBits g_structWithSignBits = { 4, 1, 5 }; // high bit is sign extender, so, m_bit5 = -1

structWithArray  g_structWithArray = { { 0, 2 }, 3 };

int classChild::m_staticField = 200;
classChild  g_classChild;
classBase2*  g_polimorphChild = static_cast<classBase2*>(&g_classChild);

unionTest  g_unionTest = { 123456 };

structNullSize* g_nullSizeArray = 0;

pstructAbstract g_structAbstract = 0;

struct {
    struct {
        int m_fieldNestedStruct;
    };
    int m_fieldOfUnNamed;
} g_unNamedStruct = { { 4 }, 5 };

std::string   classBase2::m_stdstr = "hello";

template<int itemNum>
class ListCreator
{
public:
    ListCreator( listEntry &listHead ) {
        
        listHead.flink = &listHead;
        listHead.blink = &listHead;

        for ( int i = 0; i < itemNum; ++i )
        {
            listStruct  *s = new listStruct(i);
            listEntry  *entry = &s->next;

            listHead.blink->flink = entry;
            entry->blink = listHead.blink;
            entry->flink = &listHead;
            listHead.blink = entry;
        }
    }
};

listEntry  g_listHead;
ListCreator<5> list1( g_listHead );

virtualChild       g_virtChild;
virtualBase2*      g_polimorphVirtChild1 = static_cast<virtualBase2*>(&g_virtChild);
classBase1*        g_polimorphVirtChild2 = static_cast<classBase1*>(&g_virtChild);

listEntry deadlockEntry = { &deadlockEntry, &deadlockEntry };

#pragma pack(pop)

volatile ATL::CAtlException g_atlException(E_UNEXPECTED);

std::string  g_stdString("testString");
std::wstring  g_stdWString(L"testWcharString");

std::list<int> StdListCreator()
{
    std::list<int>  l;
    l.push_back(1);
    l.push_back(2);
    l.push_back(3); 
    return l;
}

std::list<int>  g_stdList = StdListCreator();

void VariadicFunc(int, ...);
volatile decltype(&VariadicFunc) g_variadicFuncPtr = &VariadicFunc;

namespace testspace {

testClass1  g_testClass;

}
