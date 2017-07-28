#pragma once

#include <string>
#include <sstream>
#include <exception>

#include "kdlib/dbgtypedef.h"
#include "kdlib/dbgengine.h"

namespace kdlib {

///////////////////////////////////////////////////////////////////////////////

class DbgException : public std::exception
{
public:

    DbgException( const std::string  &desc ) :
        std::exception( desc.c_str() )
        {}

};

///////////////////////////////////////////////////////////////////////////////

class DbgWideException : public DbgException
{
public:

    DbgWideException( const std::wstring  &desc ) : 
        DbgException( getCStrDesc(desc) )
        {}

private:

    static std::string getCStrDesc( const std::wstring &desc );

};

///////////////////////////////////////////////////////////////////////////////

class MemoryException : public DbgWideException
{
public:

    MemoryException( MEMOFFSET_64 targetAddr, bool phyAddr = false ) :
        m_targetAddress( targetAddr ),
        DbgWideException( buildDesc( targetAddr, phyAddr ) )
        {}    
    
    MEMOFFSET_64
    getAddress() const {
        return m_targetAddress;
    }
    
private:    
        
    MEMOFFSET_64     m_targetAddress;

    static std::wstring buildDesc( MEMOFFSET_64 addr, bool phyAddr )
    {
        std::wstringstream   sstr;
        if ( phyAddr )
            sstr << L"Memory exception at 0x" << std::hex << addr << L" target physical address";
        else
            sstr << L"Memory exception at 0x" << std::hex << addr << L" target virtual address";
       
        return sstr.str();
    }   
};

/////////////////////////////////////////////////////////////////////////////////

class SymbolException : public DbgWideException 
{
public:

    SymbolException( const std::wstring  &desc ) :
        DbgWideException( desc )
        {}    

};

/////////////////////////////////////////////////////////////////////////////////

class TypeException : public SymbolException
{
public:

    TypeException( const std::wstring  &errorStr )
        : SymbolException(errorStr)
        {}

    TypeException( const std::wstring &typeName, const std::wstring  &errorStr )
        : SymbolException( buildDesc( typeName, errorStr ) )
        {}

private:

    static std::wstring buildDesc( const std::wstring &typeName, const std::wstring  &errorStr )
    {
        std::wstringstream   sstr;
        sstr << typeName << L" : " << errorStr;
        return sstr.str();
    }
};

/////////////////////////////////////////////////////////////////////////////////

class CPUException : public DbgWideException
{
public:

    CPUException( const std::wstring  &desc ) :
        DbgWideException( desc )
        {}
};

/////////////////////////////////////////////////////////////////////////////////

class CallException : public DbgWideException
{
public:

    CallException( const std::wstring  &desc ) :
        DbgWideException( desc )
        {}
};

/////////////////////////////////////////////////////////////////////////////////


class IndexException : public DbgException
{
public:

    IndexException( size_t index ) 
        : DbgException(buildDesc(index))
    {}

private:

    static std::string buildDesc( size_t index )
    {
        std::stringstream   sstr;
        sstr << "index is out of range";
        return sstr.str();
    }
};

/////////////////////////////////////////////////////////////////////////////////

class ImplementException : public DbgException
{
public:

    ImplementException( const std::string &file, int line, const std::string &msg ) :
         DbgException( buildDesc(file,line, msg) )    
         {}

private:

    static std::string buildDesc( const std::string &file, int line, const std::string &msg )
    {
        std::stringstream sstream;
        sstream << "File: " << file << " Line: " << line << "  " << msg;
        return sstream.str();
    }

};

//#define WIDEN2(x)
//#define WIDEN(x) L#x
//#define __WFILE__ WIDEN(__FILE__)

#define NOT_IMPLEMENTED()   throw ImplementException( __FILE__, __LINE__, "Not Implemented" );
#define TODO(x) throw ImplementException( __FILE__, __LINE__, x );

///////////////////////////////////////////////////////////////////////////////

} // kdlib namespace end
