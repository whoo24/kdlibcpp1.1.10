#pragma once

#include <string>

#include <boost/utility.hpp>
#include <boost/shared_ptr.hpp>

#include "kdlib/symengine.h"
#include "kdlib/variant.h"

namespace kdlib {

///////////////////////////////////////////////////////////////////////////////

class TypeInfo;
typedef boost::shared_ptr<TypeInfo>     TypeInfoPtr;

TypeInfoPtr loadType( const std::wstring &symName );
TypeInfoPtr loadType( SymbolPtr &symbol );
TypeInfoPtr loadType( SymbolPtr &symbolScope, const std::wstring &symbolName ); 

TypeInfoPtr defineStruct( const std::wstring &structName, size_t align = 0 );
TypeInfoPtr defineUnion( const std::wstring& unionName, size_t align = 0 );
TypeInfoPtr defineFunction( const TypeInfoPtr& returnType, CallingConventionType callconv = CallConv_NearC);

TypeInfoPtr compileType( const std::wstring &sourceCode, const std::wstring& typeName , const std::wstring  &options=L"");

size_t getSymbolSize( const std::wstring &name );
MEMOFFSET_64 getSymbolOffset( const std::wstring &name );
std::wstring findSymbol( MEMOFFSET_64 offset);
std::wstring findSymbol( MEMOFFSET_64 offset, MEMDISPLACEMENT &displacement );

class TypeInfo : private boost::noncopyable, public NumBehavior {

    friend TypeInfoPtr loadType( const std::wstring &symName );
    friend TypeInfoPtr loadType( SymbolPtr &symbol );
    friend TypeInfoPtr loadType( SymbolPtr &symbolScope, const std::wstring &symbolName ); 
    friend size_t getSymbolSize( const std::wstring &name );

public:

    virtual std::wstring str() = 0;

    virtual std::wstring getName() = 0;
    virtual std::pair<std::wstring, std::wstring> splitName() = 0;

    virtual size_t getSize() = 0;
    virtual size_t getPtrSize() = 0;

    virtual TypeInfoPtr ptrTo( size_t ptrSize = 0 ) = 0;
    virtual TypeInfoPtr deref() = 0;
    virtual TypeInfoPtr arrayOf( size_t size ) = 0;

    virtual bool isArray() = 0;
    virtual bool isPointer() = 0;
    virtual bool isVoid() = 0;
    virtual bool isBase() = 0;
    virtual bool isUserDefined() = 0;
    virtual bool isConstant() = 0;
    virtual bool isEnum() = 0;
    virtual bool isBitField() = 0;
    virtual bool isFunction() = 0;
    virtual bool isVtbl() = 0;
    virtual bool isNoType() = 0;

    virtual BITOFFSET getBitOffset() = 0;
    virtual BITOFFSET getBitWidth() = 0;
    virtual TypeInfoPtr getBitType() = 0;

    virtual TypeInfoPtr getElement( const std::wstring &name ) = 0;
    virtual TypeInfoPtr getElement( size_t index ) = 0;
    virtual MEMOFFSET_REL getElementOffset( const std::wstring &name ) = 0;
    virtual MEMOFFSET_REL getElementOffset( size_t index ) = 0;
    virtual std::wstring getElementName( size_t index ) = 0;
    virtual size_t getElementIndex( const std::wstring &name ) = 0;
    virtual size_t getElementCount() = 0;
    virtual  MEMOFFSET_64 getElementVa( const std::wstring &name ) = 0;
    virtual  MEMOFFSET_64 getElementVa( size_t index ) = 0;
    
    virtual bool isStaticMember( const std::wstring &name ) = 0;
    virtual bool isStaticMember( size_t index ) = 0;

    virtual bool isVirtualMember( const std::wstring &name ) = 0;
    virtual bool isVirtualMember( size_t index ) = 0;

    virtual bool isVirtual() = 0;

    virtual TypeInfoPtr getMethod( const std::wstring &name, const std::wstring&  prototype = L"") = 0;
    virtual TypeInfoPtr getMethod( const std::wstring &name, TypeInfoPtr prototype) = 0;
    virtual TypeInfoPtr getMethod( size_t index ) = 0;
    virtual size_t getMethodsCount() = 0;

    virtual TypeInfoPtr getBaseClass( const std::wstring& className) = 0;
    virtual TypeInfoPtr getBaseClass( size_t index ) = 0;
    virtual size_t getBaseClassesCount() = 0;
    virtual MEMOFFSET_REL getBaseClassOffset( const std::wstring &name ) = 0;
    virtual MEMOFFSET_REL getBaseClassOffset( size_t index ) = 0;
    virtual bool isBaseClassVirtual( const std::wstring &name ) = 0;
    virtual bool isBaseClassVirtual( size_t index ) = 0;

    virtual void getBaseClassVirtualDisplacement( const std::wstring &name, MEMOFFSET_32 &virtualBasePtr, size_t &virtualDispIndex, size_t &virtualDispSize ) = 0;
    virtual void getBaseClassVirtualDisplacement( size_t index, MEMOFFSET_32 &virtualBasePtr, size_t &virtualDispIndex, size_t &virtualDispSize ) = 0;
    
    virtual NumVariant getValue() const = 0;

    virtual void getVirtualDisplacement( const std::wstring& fieldName, MEMOFFSET_32 &virtualBasePtr, size_t &virtualDispIndex, size_t &virtualDispSize ) = 0;
    virtual void getVirtualDisplacement( size_t fieldIndex, MEMOFFSET_32 &virtualBasePtr, size_t &virtualDispIndex, size_t &virtualDispSize ) = 0;

    virtual CallingConventionType getCallingConvention() = 0; // <- CallingConventionType
    virtual bool hasThis() = 0;
    virtual TypeInfoPtr getReturnType() = 0;

    virtual void appendField(const std::wstring &fieldName, TypeInfoPtr &fieldType ) = 0;

    virtual TypeInfoPtr getClassParent() = 0;

    // http://msdn.microsoft.com/en-us/library/hx1b6kkd.aspx
    // "Padding and Alignment of Structure Members"
    virtual size_t getAlignReq() = 0;

    virtual TypeInfoPtr getVTBL() = 0;

    virtual MEMOFFSET_REL getVtblOffset() = 0;

protected:

    static bool isBaseType( const std::wstring &typeName );
    static bool isComplexType( const std::wstring &typeName );
    static TypeInfoPtr getTypeInfoFromCache(const std::wstring &typeName );

    static TypeInfoPtr getBaseTypeInfo( const std::wstring &typeName, size_t ptrSize );
    static TypeInfoPtr getBaseTypeInfo( SymbolPtr &symbolScope );

    static TypeInfoPtr getComplexTypeInfo( const std::wstring &typeName, SymbolPtr &symbolScope );
    static TypeInfoPtr getRecursiveComplexType( const std::wstring &typeName, TypeInfoPtr &lowestType, std::wstring &suffix, size_t ptrSize );

    TypeInfo() 
    {}

    virtual ~TypeInfo() 
    {}
};

///////////////////////////////////////////////////////////////////////////////

class TypeInfoProvider;
typedef boost::shared_ptr<TypeInfoProvider>     TypeInfoProviderPtr;

class TypeInfoProvider 
{
public:

    virtual TypeInfoPtr getTypeByName(const std::wstring& name) = 0;

};

TypeInfoProviderPtr  getTypeInfoProviderFromSource( const std::wstring&  source, const std::wstring&  opts = L"" );
TypeInfoProviderPtr  getTypeInfoProviderFromPdb( const std::wstring&  pdbFile, MEMOFFSET_64  loadBase = 0 );

///////////////////////////////////////////////////////////////////////////////

TypeInfoPtr makeCharConst(char val);
TypeInfoPtr makeShortConst(short val);
TypeInfoPtr makeLongConst(long val);
TypeInfoPtr makeLongLongConst(long long val);
TypeInfoPtr makeUCharConst(unsigned char val);
TypeInfoPtr makeUShortConst(unsigned short val);
TypeInfoPtr makeULongConst(unsigned long val);
TypeInfoPtr makeULongLongConst(unsigned long long val);
TypeInfoPtr makeIntConst(int val);
TypeInfoPtr makeUIntConst(unsigned int val);
TypeInfoPtr makeFloatConst(float val);
TypeInfoPtr makeDoubleConst(double val);
TypeInfoPtr makeWCharConst(wchar_t val);
TypeInfoPtr makeBoolConst(bool val);

///////////////////////////////////////////////////////////////////////////////

}; // kdlib namespace end
