#include "stdafx.h"

#include "kdlib\typeinfo.h"

#include "typeinfoimp.h"
#include "udtfield.h"

namespace kdlib {

/////////////////////////////////////////////////////////////////////////////////

class CustomBase : public TypeInfoFields {

protected:

    CustomBase( const std::wstring &name, size_t align ) :
        TypeInfoFields(name)
        {
            m_align = align ? align : ptrSize();
            m_size = 0;
        }
    

    virtual bool isUserDefined() {
        return true;
    }

    void throwIfFiledExist(const std::wstring &fieldName);
    void throwIfTypeRecursive(TypeInfoPtr type);

    void checkAppendField(const std::wstring &fieldName, TypeInfoPtr &fieldType);

protected:

    static size_t alignUp(size_t val, size_t align) {
        return val + (val % align ? align - (val % align) : 0);
    }

    virtual size_t getSize() {
        const size_t alignReq = getAlignReq();
        return alignUp(m_size, alignReq < m_align ? alignReq : m_align);
    }

    virtual void getFields() {}

protected:

    size_t  m_align;
    MEMOFFSET_32  m_size;
};


/////////////////////////////////////////////////////////////////////////////////

class CustomStruct : public CustomBase 
{
public:
    
    CustomStruct( const std::wstring &name, size_t align = 0) :
        CustomBase( name, align )
        {}

    virtual std::wstring str() {
        return std::wstring(L"struct: ") + TypeInfoFields::print();
    }

private:

    virtual void appendField(const std::wstring &fieldName, TypeInfoPtr &fieldType);
};

///////////////////////////////////////////////////////////////////////////////

class CustomUnion : public CustomBase 
{
public:
    
    CustomUnion( const std::wstring &name, size_t align = 0 ) :
        CustomBase( name, align )
        {}

    virtual std::wstring str() {
        return std::wstring(L"union: ") + TypeInfoFields::print();
    }

private:

    virtual void appendField(const std::wstring &fieldName, TypeInfoPtr &fieldType);
};

///////////////////////////////////////////////////////////////////////////////

class CustomUdtField : public TypeField
{
public:

    CustomUdtField( const TypeInfoPtr &typeInfo, const std::wstring& name ) :
        TypeField( name ),
        m_type( typeInfo )
        {}

    void setOffset( MEMOFFSET_32 offset ) {
      m_offset = offset;
    }

private:

    virtual TypeInfoPtr getTypeInfo() {
        return m_type;
    }

    TypeInfoPtr  m_type;
};

///////////////////////////////////////////////////////////////////////////////

class CustomFunction : public TypeInfoFunctionPrototype
{
public:
     CustomFunction(const TypeInfoPtr& returnType, CallingConventionType  callconv)
     {
         m_returnType = returnType;
         m_callconv = callconv;
     }

protected:


    virtual TypeInfoPtr getReturnType() {
        return m_returnType;
    }

    virtual TypeInfoPtr getClassParent() {
        return TypeInfoPtr();
    }

    virtual CallingConventionType getCallingConvention()
    {
        return m_callconv;
    }

    virtual void appendField(const std::wstring &fieldName, TypeInfoPtr &fieldType ) {
        m_args.push_back(fieldType);
    }

};

///////////////////////////////////////////////////////////////////////////////

TypeInfoPtr defineStruct( const std::wstring &structName, size_t align )
{
    return TypeInfoPtr( new CustomStruct(structName, align) );
}

///////////////////////////////////////////////////////////////////////////////

TypeInfoPtr defineUnion( const std::wstring& unionName, size_t align )
{
    return TypeInfoPtr( new CustomUnion(unionName, align) );
}

///////////////////////////////////////////////////////////////////////////////

TypeInfoPtr defineFunction(const TypeInfoPtr& returnType, CallingConventionType  callconv)
{
    return TypeInfoPtr( new CustomFunction(returnType, callconv) );
}

///////////////////////////////////////////////////////////////////////////////

void CustomBase::throwIfFiledExist(const std::wstring &fieldName)
{
    try
    {
        m_fields.lookup(fieldName);
    }
    catch (const TypeException&)
    {
        return;
    }
    throw TypeException(getName(), L"duplicate field name: " + fieldName);
}

////////////////////////////////////////////////////////////////////////////////

void CustomBase::throwIfTypeRecursive(TypeInfoPtr type)
{
    if ( type.get() == this )
        throw TypeException(getName(), L"recursive type definition");

    if ( !type->isUserDefined() )
        return;

    size_t  fieldsCount = type->getElementCount();
    for (size_t i = 0; i < fieldsCount; ++i)
        throwIfTypeRecursive(type->getElement(i));
}

////////////////////////////////////////////////////////////////////////////////

void CustomBase::checkAppendField(const std::wstring &fieldName, TypeInfoPtr &fieldType)
{
    if ( !fieldType )
         throw DbgException( "typeInfo can not be None" );

    throwIfFiledExist(fieldName);
    throwIfTypeRecursive(fieldType);
}

////////////////////////////////////////////////////////////////////////////////

void CustomStruct::appendField(const std::wstring &fieldName, TypeInfoPtr &fieldType )
{
    checkAppendField(fieldName, fieldType);

    CustomUdtField  *field = new CustomUdtField( fieldType, fieldName );

    const size_t fieldAlignReq = fieldType->getAlignReq();
    const size_t align = fieldAlignReq < m_align ? fieldAlignReq : m_align;
    MEMOFFSET_32  fieldOffset = static_cast<MEMOFFSET_32>(alignUp(m_size, align));

    field->setOffset( fieldOffset );

    m_size = static_cast<MEMOFFSET_32>( fieldOffset + fieldType->getSize() );

    m_fields.push_back( TypeFieldPtr( field ) );
}

////////////////////////////////////////////////////////////////////////////////

void CustomUnion::appendField(const std::wstring &fieldName, TypeInfoPtr &fieldType )
{
    checkAppendField(fieldName, fieldType);

    CustomUdtField  *field = new CustomUdtField( fieldType, fieldName );

    size_t  fieldSize = fieldType->getSize();

    m_size = static_cast<MEMOFFSET_32>(fieldSize > m_size ? fieldSize : m_size);

    m_fields.push_back( TypeFieldPtr( field ) );
}

////////////////////////////////////////////////////////////////////////////////

} // kdlib namespace


