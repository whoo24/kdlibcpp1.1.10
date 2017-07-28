#include "stdafx.h"

#include <DbgEng.h>

#include "kdlib/windbg.h"
#include "kdlib/kdlib.h"

using namespace kdlib;
using namespace kdlib::windbg;


static const std::string  stdStringName = "std::basic_string<char,std::char_traits<char>,std::allocator<char> >";
static const std::wstring  wstdStringName = L"std::basic_string<char,std::char_traits<char>,std::allocator<char> >";

class StlStrFormatter : public WindbgStructFormatter
{

public:
    virtual std::string  getName() 
    {
        return stdStringName;
    }

    virtual std::string  printValue( MEMOFFSET_64 offset ) 
    {
        TypedVarPtr tp = loadTypedVar( wstdStringName, offset );

        if ( *tp->getElement(L"_Mysize") <= 10 )
        {
             loadChars( tp->getElement(L"_Bx._Buf")->getAddress(), *tp->getElement(L"_Mysize") );
        }

        return loadChars( *tp->getElement(L"_Bx._Ptr"), *tp->getElement(L"_Mysize") );
    }
};

KDLIB_WINDBG_REGISTER_FORMATER(StlStrFormatter);
