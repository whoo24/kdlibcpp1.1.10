#include "stdafx.h"
#include <string.h>
#include <excpt.h>

#pragma optimize("g", off)

#include "test/testfunc.h"

#include <exception>


void
__cdecl
CdeclVoidFunc()
{
    static int a = 0;
    a++;
}

double
static
LocalStaticFunc( int a, float b )
{
   return a + b;
}

void 
__cdecl
CdeclFunc( int a, float b)
{
   int c = a*10;
   float d = b/10;

   LocalStaticFunc(c,d);
}

int
__cdecl
CdeclFuncReturn( int a, char* b )
{
    return a + strlen(b);
}

long long
__cdecl
CdeclFuncLong( unsigned long long a )
{
    return a + 5;
}

float
__cdecl
CdeclFuncFloat( float a, float b )
{
    return a*b;
}

double
__cdecl
CdeclFuncDouble( double a, double b )
{
    return (a+b)/2.0;
}

void
__stdcall
StdcallFunc( int a, float b )
{
   LocalStaticFunc(a + 10, b / 2 );
}

short
__stdcall
StdcallFuncRet( char a, long b)
{
    return b/a;
}

long long
__stdcall
StdcallFuncLong( unsigned long long a, unsigned long long b)
{
    return a & b;
}

float
__stdcall
StdcallFuncFloat(float a, float b)
{
    return a/b;
}

double
__stdcall
StdcallFuncDouble(double a, double b)
{
    return a + b;
}

double
__fastcall
FastcallFunc( int a, float b )
{
   return LocalStaticFunc(a,b);
}


bool
OverloadedFunc(int a)
{
    return a > 10;
}

bool
OverloadedFunc(int a, int b)
{
    return a < b;
}

void
_UnderscoreFunc( int a, float b )
{
   LocalStaticFunc(a,b);
}

void FuncTestClass::method()
{
    LocalStaticFunc(1,2);
}

void __cdecl FuncTestClass::staticMethod()
{
    LocalStaticFunc(2,3);
}

void ( *CdeclFuncPtr)( int a, float b) = &CdeclFunc;

void (FuncTestClass::*MethodPtr)() = &FuncTestClass::method;
void (__cdecl*CdeclStaticMethodPtr)() = &FuncTestClass::staticMethod;

void (*ArrayOfCdeclFuncPtr[])(int, float) = { &CdeclFunc, &CdeclFunc, &CdeclFunc };
void (FuncTestClass::*ArrayOfMethodPtr[])() = { &FuncTestClass::method, &FuncTestClass::method };



FuncTestClass FuncReturnClass()
{
    return FuncTestClass();
}

extern "C" 
{

void __stdcall PureCStdcallFunc( int a, float b )
{
    LocalStaticFunc(a,b);
}
void __cdecl PureCCdeclFunc( int a, float b )
{
    LocalStaticFunc(a,b);
}
void __fastcall PureCFastcallFunc( int a, float b )
{
    LocalStaticFunc(a,b);
}

}

void VariadicFunc(int, ...)
{
}

void FuncAccessViolation()
{
    size_t  ptr = 0;
    *reinterpret_cast<int*>(ptr) = 10;
}

int FuncWithSeh(long long a)
{
    __try {

        FuncAccessViolation();
    }
    __except(EXCEPTION_EXECUTE_HANDLER)
    {
    }

    return a/8;
}

void FuncStdException()
{
    throw std::exception("exception");
}

int FuncWithTry(int a)
{
    try {
        FuncStdException();
    } 
    catch(std::exception& )
    {
        return a*3;
    }

    return a*4;
}


