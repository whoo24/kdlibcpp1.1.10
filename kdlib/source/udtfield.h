#pragma once 

#include <string>
#include <vector>

#include <boost/smart_ptr.hpp>

#include "kdlib/dbgengine.h"
#include "kdlib/typeinfo.h"
#include "kdlib/exceptions.h"

namespace kdlib {

///////////////////////////////////////////////////////////////////////////////

class TypeField;
typedef boost::shared_ptr<TypeField>  TypeFieldPtr;

class ClangException : public TypeException
{
public:

    ClangException()
        :  TypeException( L"clang parse error")
        {}

    ClangException( const std::wstring  &errorStr )
        :  TypeException(errorStr)
        {}

    ClangException( const std::wstring &typeName, const std::wstring  &errorStr )
        : TypeException(  typeName, errorStr )
        {}

};

class TypeField
{

public:

    const std::wstring& getName() const
    {
        return m_name;
    }

    const std::wstring& getVirtualBaseClassName() const 
    {
        return m_virtualBaseName;
    }

    MEMOFFSET_REL getOffset() const
    {
        if ( m_staticOffset != 0 )
            throw TypeException( m_name, L"static field has only virtual address" );

        return m_offset;
    }

    bool isVirtualMember() const 
    {
        return m_virtualMember;
    }

    bool isStaticMember() const
    {
        return m_staticMember;
    }

    bool isMethod() const 
    {
        return m_methodMember;
    }

    MEMOFFSET_64 getStaticOffset() const
    {
        if ( m_staticMember )
            return m_staticOffset;
  
        throw TypeException( m_name, L"this is not a static field" );
    }

    void getVirtualDisplacement( MEMOFFSET_32 &virtualBasePtr, size_t &virtualDispIndex, size_t &virtualDispSize )
    {
        if ( !m_virtualMember )
            throw TypeException( m_name, L"this is not a virtual member" );

       virtualBasePtr = m_virtualBasePtr;
       virtualDispIndex = m_virtualDispIndex;
       virtualDispSize = m_virtualDispSize;
    }

    virtual TypeInfoPtr getTypeInfo() = 0;

    virtual NumVariant getValue() const {
        throw TypeException(  m_name, L"filed has no value" );
    }

protected:

    TypeField( const std::wstring  &name ) :
         m_name( name ),
         m_offset( 0 ),
         m_staticOffset( 0 ),
         m_virtualBasePtr( 0 ),
         m_virtualDispIndex( 0 ),
         m_virtualDispSize( 0 ),
         m_staticMember( false ),
         m_virtualMember( false ),
         m_methodMember(false)
         {}

    std::wstring  m_name;

    std::wstring  m_virtualBaseName;

    MEMOFFSET_32  m_offset;

    MEMOFFSET_64  m_staticOffset;

    MEMOFFSET_32  m_virtualBasePtr;
    size_t  m_virtualDispIndex;
    size_t  m_virtualDispSize;

    bool  m_staticMember;
    bool  m_virtualMember;
    bool  m_methodMember;
};

///////////////////////////////////////////////////////////////////////////////////

class SymbolUdtField : public TypeField 
{
public:

    static TypeFieldPtr getField( 
        const SymbolPtr &sym, 
        const std::wstring& name,
        MEMOFFSET_32 offset )
    {
        SymbolUdtField *p = new SymbolUdtField( sym, name );
        p->m_offset = offset;
        return TypeFieldPtr(p);
    }

    static TypeFieldPtr getStaticField(
        const SymbolPtr &sym, 
        const std::wstring& name,
        MEMOFFSET_64 offset )
    {
        SymbolUdtField *p = new SymbolUdtField( sym, name );
        p->m_staticOffset = offset;
        p->m_staticMember = true;
        return TypeFieldPtr(p);
    }

    static TypeFieldPtr getVirtualField(
        const SymbolPtr &sym, 
        const std::wstring& name,
        MEMOFFSET_32 offset,
        MEMOFFSET_32 virtualBasePtr, 
        size_t virtualDispIndex, 
        size_t virtualDispSize )
    {
        SymbolUdtField *p = new SymbolUdtField( sym, name );
        p->m_offset = offset;
        p->m_virtualBasePtr = virtualBasePtr;
        p->m_virtualDispIndex = virtualDispIndex;
        p->m_virtualDispSize = virtualDispSize;
        p->m_virtualMember = true;
        return TypeFieldPtr(p);
    }

    static TypeFieldPtr getVirtualMethodField(
        const SymbolPtr& sym,
        const std::wstring& name 
        )
    {
        SymbolUdtField *p = new SymbolUdtField( sym, name );
        p->m_methodMember = true;
        p->m_virtualMember = true;
        return TypeFieldPtr(p);
    }



public:

    SymbolPtr& getSymbol() {
        return m_symbol;
    }

private:

    SymbolUdtField( const SymbolPtr &sym, const std::wstring& name ) :
        TypeField( name ),
        m_symbol( sym )
        {}

    virtual TypeInfoPtr getTypeInfo();

    SymbolPtr  m_symbol;
};

///////////////////////////////////////////////////////////////////////////////

class EnumField : public TypeField 
{
public:
    EnumField( const SymbolPtr &sym ) :
        TypeField( sym->getName() ),
        m_symbol( sym )
        {}

private:

    virtual TypeInfoPtr getTypeInfo() {
        return loadType(m_symbol);
    }

    NumVariant getValue() const;

    SymbolPtr  m_symbol;
};

///////////////////////////////////////////////////////////////////////////////

class FieldCollection 
{
public:

    FieldCollection( const std::wstring &name ) :
      m_name( name )
      {}

    const TypeFieldPtr &lookup(size_t index) const;
    const TypeFieldPtr &lookup(const std::wstring &name) const;

    TypeFieldPtr &lookup(size_t index);
    TypeFieldPtr &lookup(const std::wstring &name);

    size_t getIndex(const std::wstring &name) const; 

    void push_back( const TypeFieldPtr& field ) {
        m_fields.push_back( field );
    }

    size_t count() const {
        return m_fields.size();
    }

private:

    typedef std::vector<TypeFieldPtr>  FieldList;
    FieldList  m_fields;
    std::wstring  m_name;
};

///////////////////////////////////////////////////////////////////////////////

} // end kdlib namespace

