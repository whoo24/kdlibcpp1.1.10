#pragma once

#include <string>

namespace kdlib {

///////////////////////////////////////////////////////////////////////////////v

void dprint( const std::wstring &str, bool dml = false );
void dprintln( const std::wstring &str, bool dml = false );
std::wstring dreadline();
void eprint( const std::wstring &str );
void eprintln( const std::wstring &str );
void dinput( const std::wstring &str );

///////////////////////////////////////////////////////////////////////////////

class DbgOut
{
public:

    virtual void write( const std::wstring& str) = 0;

    virtual void writedml( const std::wstring& str) = 0;
};

///////////////////////////////////////////////////////////////////////////////

class DbgIn
{
public:
    virtual std::wstring readline() = 0;
};

///////////////////////////////////////////////////////////////////////////////

extern DbgOut *dbgout;
extern DbgOut *dbgerr;
extern DbgIn  *dbgin;

///////////////////////////////////////////////////////////////////////////////

}