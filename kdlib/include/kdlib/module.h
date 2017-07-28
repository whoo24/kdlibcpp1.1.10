#pragma once

#include <string>


#include <boost/utility.hpp>
#include <boost/smart_ptr.hpp>

#include "kdlib/dbgtypedef.h"
#include "kdlib/dbgengine.h"
#include "kdlib/symengine.h"
#include "kdlib/typeinfo.h"
#include "kdlib/variant.h"
#include "kdlib/typedvar.h"

namespace kdlib {

///////////////////////////////////////////////////////////////////////////////

class Module;
typedef boost::shared_ptr<Module>  ModulePtr;

ModulePtr loadModule( const std::wstring &name );
ModulePtr loadModule( MEMOFFSET_64 offset );

void splitSymName( const std::wstring &fullName, std::wstring &moduleName, std::wstring &symbolName );

typedef std::pair< std::wstring, MEMOFFSET_64 > SymbolOffset;
typedef std::list< SymbolOffset > SymbolOffsetList;
typedef std::list< std::wstring > TypeNameList;


class Module : private boost::noncopyable, public NumBehavior {
    
public:

    virtual std::wstring  getName() = 0;

    virtual MEMOFFSET_64  getBase() const = 0;

    virtual MEMOFFSET_64  getEnd() const = 0;

    virtual size_t getSize() const = 0;

    virtual std::wstring getImageName() const = 0;
    
    virtual unsigned long getCheckSum() const = 0;

    virtual unsigned long getTimeDataStamp() const = 0;

    virtual bool isUnloaded() const = 0;

    virtual bool isUserMode() const = 0;

    virtual bool isSymbolLoaded() const = 0;

    virtual std::wstring getSymFile() = 0;

    virtual void reloadSymbols() = 0;

    virtual void resetSymbols() = 0;

    virtual MEMOFFSET_64 getSymbolVa( const std::wstring &symbolName ) = 0;
    
    virtual MEMOFFSET_32 getSymbolRva( const std::wstring &symbolName ) = 0;

    virtual SymbolPtr getSymbolByVa( MEMOFFSET_64 offset, MEMDISPLACEMENT* displacement = 0 ) = 0;

    virtual SymbolPtr getSymbolByVa( MEMOFFSET_64 offset, unsigned long  symTag, MEMDISPLACEMENT* displacement ) = 0;

    virtual SymbolPtr getSymbolByRva( MEMOFFSET_32 offset, MEMDISPLACEMENT* displacement = 0 ) = 0;

    virtual SymbolPtr getSymbolByRva( MEMOFFSET_32 offset, unsigned long  symTag, MEMDISPLACEMENT* displacement ) = 0;

    virtual SymbolPtr getSymbolByName( const std::wstring &symbolName ) = 0;

    virtual SymbolPtr getSymbolScope() = 0;

    virtual size_t getSymbolSize( const std::wstring &symName ) = 0;

    virtual TypeInfoPtr getTypeByName( const std::wstring &typeName ) = 0;

    virtual TypedVarPtr getTypedVarByAddr( MEMOFFSET_64 offset ) = 0;

    virtual TypedVarPtr getTypedVarByName( const std::wstring &symName ) = 0;

    virtual TypedVarPtr getTypedVarByTypeName( const std::wstring &typeName, MEMOFFSET_64 addr ) = 0;

    virtual TypedVarPtr getTypedVarWithPrototype( const std::wstring &symName, const std::wstring &prototype) = 0;

    virtual TypedVarPtr containingRecord( MEMOFFSET_64 offset, const std::wstring &typeName, const std::wstring &fieldName ) = 0;
    
    virtual TypedVarPtr getFunctionByAddr( MEMOFFSET_64 offset ) = 0;

    virtual TypedVarList loadTypedVarList( MEMOFFSET_64 addr, const std::wstring &typeName, const std::wstring &fieldName ) = 0;

    virtual TypedVarList loadTypedVarArray( MEMOFFSET_64 addr, const std::wstring &typeName, size_t count ) = 0;

    virtual SymbolOffsetList  enumSymbols( const std::wstring  &mask = L"*" ) = 0;

    virtual TypeNameList enumTypes(const std::wstring  &mask = L"*") = 0;

    virtual std::wstring findSymbol( MEMOFFSET_64 offset, MEMDISPLACEMENT &displacement ) = 0;

    virtual std::wstring getSourceFile( MEMOFFSET_64 offset ) = 0;

    virtual std::wstring  getSourceFileFromSrcSrv(MEMOFFSET_64 offset) = 0;

    virtual void getSourceLine( MEMOFFSET_64 offset, std::wstring &fileName, unsigned long &lineno, long &displacement ) = 0;

    virtual std::string getVersionInfo( const std::string &value ) = 0;

    virtual void getFileVersion(unsigned long& majorVersion, unsigned long& minorVerion, unsigned long& revision, unsigned long& build) = 0;

    virtual void getFixedFileInfo( FixedFileInfo &fixedFileInfo ) = 0;
};

///////////////////////////////////////////////////////////////////////////////

} // kdlib namespace end

