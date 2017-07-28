#pragma once

#include <string>

#pragma pack( push, 4 )

extern char  helloStr[];
extern wchar_t  helloWStr[];
extern char  helloStr1[];

extern unsigned char ucharVar;
extern unsigned short ushortVar;
extern unsigned long ulongVar;
extern unsigned long long ulonglongVar;
extern char charVar;
extern wchar_t wcharVar;
extern short shortVar;
extern long longVar;
extern long long longlongVar;
extern float floatVar;
extern double doubleVar;
extern bool boolVar;

extern const bool boolConst;
extern const unsigned long ulongConst;
extern const unsigned long long ulonglongConst;

#define TEST_ARRAY_SIZE  5
extern unsigned char ucharArray[TEST_ARRAY_SIZE];
extern unsigned short ushortArray[TEST_ARRAY_SIZE];
extern unsigned long ulongArray[TEST_ARRAY_SIZE];
extern unsigned long long ulonglongArray[TEST_ARRAY_SIZE];
extern char charArray[TEST_ARRAY_SIZE];
extern short shortArray[TEST_ARRAY_SIZE];
extern long longArray[TEST_ARRAY_SIZE];
extern long long longlongArray[TEST_ARRAY_SIZE];
extern float floatArray[TEST_ARRAY_SIZE];
extern double doubleArray[TEST_ARRAY_SIZE];

extern unsigned char ucharArrayPlace[TEST_ARRAY_SIZE];
extern unsigned short ushortArrayPlace[TEST_ARRAY_SIZE];
extern unsigned long ulongArrayPlace[TEST_ARRAY_SIZE];
extern unsigned long long ulonglongArrayPlace[TEST_ARRAY_SIZE];
extern char charArrayPlace[TEST_ARRAY_SIZE];
extern short shortArrayPlace[TEST_ARRAY_SIZE];
extern long longArrayPlace[TEST_ARRAY_SIZE];
extern long long longlongArrayPlace[TEST_ARRAY_SIZE];
extern float floatArrayPlace[TEST_ARRAY_SIZE];
extern double doubleArrayPlace[TEST_ARRAY_SIZE];

extern unsigned long long bigValue;
extern unsigned long long *pbigValue;
extern unsigned long long **ppbigValue;

extern float floatValue;
extern double doubleValue;

extern unsigned long long ullValuePlace;
extern float floatValuePlace;
extern double doubleValuePlace;


extern unsigned long long &ref_bigValue;
extern unsigned long long *&ref_pbigValue;

extern const unsigned long long bigConstValue;

extern void* voidPtr;
extern void* voidPtrArray[];

struct structTest {
    unsigned long m_field0;
    unsigned long long m_field1;
    bool m_field2;
    unsigned short m_field3;
    structTest* m_field4;
};

extern structTest      g_structTest;
extern structTest      g_structTest1;
extern structTest      g_testArray[2];
extern structTest      *g_structTestPtr;
extern structTest      **g_structTestPtrPtr;

typedef struct structTest   structTestTypeDef;
extern structTestTypeDef    g_structTypeDef;


extern int intMatrix[2][3];
extern int intMatrix2[2][3];
extern int intMatrix3[2][3];
extern int intMatrix4[2][3];
extern int (*ptrIntMatrix)[2][3];
extern int (*arrIntMatrixPtrs[4])[2][3];

extern char* strArray[];
extern char *(*ptrStrArray)[2];

extern char  bigCStr[];
extern wchar_t  bigWStr[];

struct structWithNested {

    struct Nested 
    {
        int m_nestedFiled;
    };

    int m_field;

    struct {

         int m_field2;

    } m_unnameStruct;

    struct {

         int m_field3;

         struct {
             char m_filed4;
         };

         union {
             unsigned char m_field5;
         };
    };

    union {

        float  m_field6;

    } m_unnameUnion;

    union {

         long  m_field7;

         union {
             short m_field8;
         };

         struct {
             short m_field9;
         };
    };
};

extern structWithNested g_structWithNested;
extern structWithNested::Nested g_structNested;

enum enumType {
    ONE = 1,
    TWO = 2,
    THREE = 3
};

extern enumType g_constEnumThree;

struct structWithEnum {
    enumType  digit;
};

extern structWithEnum  g_structWithEnum;

struct structWithBits {
    unsigned long m_bit0_4  : 5;
    unsigned long m_bit5    : 1;
    unsigned long m_bit6_8  : 3;
    unsigned long long m_bit0_60 : 60;
};

extern structWithBits g_structWithBits;


struct structWithSignBits {
    long m_bit0_4  : 5;
    long m_bit5    : 1;
    long m_bit6_8  : 3;
    unsigned long long m_bit0_60 : 60;
};

extern structWithSignBits g_structWithSignBits;


struct structWithArray {
    int   m_arrayField[2];
    int   m_noArrayField;
};

extern structWithArray  g_structWithArray;


union unionTest {
    long m_value;
    double m_doubleValue;
    structTest m_structValue;
};

extern unionTest  g_unionTest;

struct structNullSize {};
extern structNullSize* g_nullSizeArray;

struct structAbstract;
typedef struct structAbstract  *pstructAbstract;
extern pstructAbstract g_structAbstract;

////////////////////////////////////////////////////////////////////////////////

class classProBase1  {

public:

    int proBaseMethod(long a) const { return a * 10; }

    virtual int virtMethod1(float a)
    {
        return -10;
    }

};

class classBase1 : public classProBase1 {
public:
    int m_baseField;
    int baseMethod( long a) const { return proBaseMethod(a) + m_baseField; }
    virtual int virtMethod1(int a) =  0;
    virtual void virtMethod2() =  0;

    classBase1() :
        m_baseField( -100 )
        {}
};


class classBase2 {
    static std::string   m_stdstr;
    long    m_count;
    int     m_baseField;
public:
    classBase2() : 
        m_count( 1234 ),
        m_baseField( 100 )
        {}
    
    virtual int virtMethod3() {
        return 23;
    }

    virtual int virtMethod4() {
        return 24;
    }

    float overloadMethod(float a, float b) {
        return a*b;
    }
};



class classChild : public classBase1, public classBase2 {

public:

    static const int  m_staticConst = 100;

    static int  m_staticField;

public:
    int m_childField;
    int m_childField2;
    structTest m_childField3;
    enumType m_enumField;

    int childMethod(int var) const{
        baseMethod(33);
        staticMethod(10,1);
        return m_childField * 5;
    }

    virtual int virtMethod1(int a) { return 1;}
    
    virtual void virtMethod2() {}

    virtual int virtMethod3() {
        return 123;
    }

    virtual int virtMethod4() {
        return 124;
    }

    static char staticMethod( int a, int b ) { return static_cast<char>(a/b); }

    void noBodyFunc(int a);

    int overloadMethod(int a) {
        return a*10;
    }

    int overloadMethod(int a, int b) {
        return a*b;
    }

    classChild() :
        m_enumField( THREE ),
        m_childField(1000)
        {}
};

extern classChild  g_classChild;
extern classBase2*  g_polimorphChild;


////////////////////////////////////////////////////////////////////////////////

class virtualBase1 : public virtual classBase1 {

public:

    int     m_member;

    virtualBase1() : m_member(123){}

    virtual int virtMethod1(int a) { return a * 10; }
    virtual void virtMethod2() {}
};


class virtualBase2 : public virtual classBase1
{
public:

    int     m_member;

    virtualBase2() : m_member(345){}

    virtual int virtMethod1(int a) { return a / 10; }
};

class virtualChild : public virtualBase1, public  virtualBase2
{
public:

    int     m_member;

    virtual int virtMethod1(int a) { return a + 101; }
    virtual void virtMethod2() {}
    virtual int virtChildMethod() { return 10001; }

    virtualChild() : m_member(678) {}
};

extern virtualChild       g_virtChild;
extern virtualBase2*      g_polimorphVirtChild1;
extern classBase1*        g_polimorphVirtChild2;

////////////////////////////////////////////////////////////////////////////////


struct listEntry {
    listEntry  *flink;
    listEntry  *blink;
};


struct listStruct {
    int             num;
    listEntry       next;

    listStruct(int n) : num(n) {}
};

extern listEntry  g_listHead;

extern listEntry deadlockEntry;

extern std::string  g_stdString;
extern std::wstring  g_stdWString;

////////////////////////////////////////////////////////////////////////////////

namespace testspace {


class testClass1 {

public:

    class nestedClass {
    public:
        nestedClass() : m_member(100500) {}

        int getMember() const {
            return m_member;
        }

    private:
        int  m_member;
    };


private:

    nestedClass  m_nestedMember;

};

extern  testClass1  g_testClass;


}

////////////////////////////////////////////////////////////////////////////////

#pragma pack ( pop )

