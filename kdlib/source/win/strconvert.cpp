#include "stdafx.h"

#include "strconvert.h"


#include <comutil.h>

namespace kdlib {

std::string  wstrToStr(const std::wstring& str)
{
    return std::string( _bstr_t(str.c_str()) );
}


std::wstring  strToWStr(const std::string& str)
{
    return std::wstring( _bstr_t(str.c_str()) );
}

}