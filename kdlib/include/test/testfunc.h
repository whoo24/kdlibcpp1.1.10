#pragma once

void
__cdecl
CdeclVoidFunc();

void 
__cdecl
CdeclFunc( int a, float b);

int
__cdecl
CdeclFuncReturn( int a, char* b );

long long
__cdecl
CdeclFuncLong( unsigned long long a );

float
__cdecl
CdeclFuncFloat( float a, float b );

double
__cdecl
CdeclFuncDouble( double a, double b );

void
__stdcall
StdcallFunc( int a, float b );

short
__stdcall
StdcallFuncRet( char a, long b);

long long
__stdcall
StdcallFuncLong( unsigned long long a, unsigned long long b);

float
__stdcall
StdcallFuncFloat(float a, float b);

double
__stdcall
StdcallFuncDouble(double a, double b);


double
__fastcall
FastcallFunc( int a, float b );

bool
OverloadedFunc(int a);

bool
OverloadedFunc(int a, int b);

class FuncTestClass
{
public:
    void method();
    static void __cdecl staticMethod();
};

extern void ( *CdeclFuncPtr)( int a, float b);

extern void (FuncTestClass::*MethodPtr)();
extern void (__cdecl*CdeclStaticMethodPtr)();

extern FuncTestClass FuncReturnClass();

void FuncAccessViolation();
int FuncWithSeh(long long a);
void FuncStdException();
int FuncWithTry(int a);


template<typename T>
struct complexval {
    T  re;
    T  im;
};

typedef complexval<short>      shortcomplex;
typedef complexval<int>        intcomplex;
typedef complexval<long long>  longcomplex;

template <typename T>
inline bool operator==(const complexval<T>& a, const complexval<T>& b) {
    return a.re == b.re && a.im == b.im;
}

template<typename T>
complexval<T> __cdecl FuncWithStructCdecl( complexval<T> a )
{
    return { a.im, a.re };
}

template<typename T>
complexval<T> __stdcall FuncWithStructStd( complexval<T> a )
{
    return { 2 * a.re, 2 * a.im };
}

template<typename T>
complexval<T> __fastcall FuncWithStructFast( complexval<T> a )
{
    return { 2 * a.re, 2 * a.im };
}



