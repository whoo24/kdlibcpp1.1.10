#include "stdafx.h"

#include <DbgEng.h>

#include "kdlib/windbg.h"
#include "kdlib/kdlib.h"

using namespace kdlib;
using namespace kdlib::windbg;

class SKdlibExt : public WindbgExtension
{
public:
    KDLIB_EXT_COMMAND_METHOD(help);
    KDLIB_EXT_COMMAND_METHOD(input);
};

KDLIB_WINDBG_EXTENSION_INIT( SKdlibExt );

KDLIB_EXT_COMMAND_METHOD_IMPL(SKdlibExt, help)
{
    dprintln( L"KDlib Sample Extension" );
}

KDLIB_EXT_COMMAND_METHOD_IMPL(SKdlibExt, input)
{
    dprintln( L"What is your name?" );
    std::wstring answer = dreadline();
    dprintln( L"Your name is " + answer );
}





