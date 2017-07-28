#pragma once

#include <kdlib/typedvar.h>
#include <kdlib/exceptions.h>
#include <kdlib/memaccess.h>

namespace kdlib {

///////////////////////////////////////////////////////////////////////////////

class TypedVarImp : public TypedVar
{

protected:


    virtual std::wstring str() 
    {
        NOT_IMPLEMENTED();
    }

    virtual NumVariant getValue() const
    {
        NOT_IMPLEMENTED();
    }

    virtual void setValue(const NumVariant& value) 
    {
        throw TypeException( m_typeInfo->getName(), L" failed to set value");
    }

    virtual VarStorage getStorage() const
    {
        return m_varData->getStorageType();
    }

    virtual std::wstring getRegisterName() const
    {
        return m_varData->getRegisterName();
    }

    virtual MEMOFFSET_64 getAddress() const 
    {
        return m_varData->getAddress();
    }

    virtual MEMOFFSET_64 getDebugStart() const
    {
        throw TypeException( L"is not a function");
    }

    virtual MEMOFFSET_64 getDebugEnd() const
    {
        throw TypeException( L"is not a function");
    }

    virtual size_t getSize() const
    {
        return m_typeInfo->getSize();
    }

    virtual std::wstring getName() const
    {
        if (m_name.empty() )
            throw TypeException( m_typeInfo->getName(), L" can not get variable name");
        return m_name;
    }

    virtual TypeInfoPtr getType() const 
    {
       return m_typeInfo;
    }

    virtual TypedVarPtr deref() 
    {
        throw TypeException( m_typeInfo->getName(), L"object can not be derefernced" );
    }

    virtual TypedVarPtr getElement( const std::wstring& fieldName ) 
    {
        throw TypeException( m_typeInfo->getName(), L" type has no named fields");
    }

    virtual TypedVarPtr getElement( size_t index ) 
    {
        throw TypeException( m_typeInfo->getName(), L" type has no fields or array elements");
    }

    virtual MEMOFFSET_REL getElementOffset( const std::wstring& fieldName ) 
    {
        throw TypeException( m_typeInfo->getName(), L" type has no named fields");
    }

    virtual MEMOFFSET_REL getElementOffset( size_t index ) 
    {
        throw TypeException( m_typeInfo->getName(), L" type has no fields or array elements");
    }

    virtual RELREG_ID getElementOffsetRelativeReg(const std::wstring& fieldName ) 
    {
        throw TypeException( m_typeInfo->getName(), L" type has no named fields");
    }

    virtual RELREG_ID getElementOffsetRelativeReg(size_t index ) 
    {
        throw TypeException( m_typeInfo->getName(), L" type has no fields or array elements");
    }

    virtual VarStorage getElementStorage(const std::wstring& fieldName)
    {
        throw TypeException(m_typeInfo->getName(), L" not implemented for this type");
    }

    virtual VarStorage getElementStorage(size_t index)
    {
        throw TypeException(m_typeInfo->getName(), L" not implemented for this type");
    }

    virtual size_t getElementCount()
    {
        throw TypeException( m_typeInfo->getName(), L" type has no fields or array elements");
    }

    virtual std::wstring getElementName( size_t index ) 
    {
        throw TypeException( m_typeInfo->getName(), L" type has no named fields");
    }

    virtual size_t getElementIndex( const std::wstring&  elementName )
    {
        throw TypeException( m_typeInfo->getName(), L" type has no named fields");
    }

    virtual TypedVarPtr getMethod( const std::wstring &name, const std::wstring&  prototype = L"")
    {
        throw TypeException( m_typeInfo->getName(), L" type has no methods");
    }
    
    virtual TypedVarPtr getMethod( const std::wstring &name, TypeInfoPtr prototype)
    {
        throw TypeException( m_typeInfo->getName(), L" type has no methods");
    }

    virtual unsigned long getElementReg(const std::wstring& fieldName)
    {
        NOT_IMPLEMENTED();
    }

    virtual unsigned long getElementReg(size_t index)
    {
        NOT_IMPLEMENTED();
    }

    virtual TypedVarPtr castTo(const std::wstring& typeName) const;

    virtual TypedVarPtr castTo(const TypeInfoPtr &typeInfo) const;

    virtual void writeBytes(DataAccessorPtr& stream, size_t pos = 0) const
    {
        std::vector<unsigned char>  buffer( getSize() );
        m_varData->readBytes(buffer, getSize() );
        stream->writeBytes(buffer, pos);
    }

    virtual TypedValue call(const TypedValueList& arglst)
    {
        throw TypeException( L" is not a function");
    }
    
    virtual void setElement( const std::wstring& fieldName, const TypedValue& value) 
    {
        throw TypeException( m_typeInfo->getName(), L" type has no fields or array elements");
    }

    virtual void setElement( size_t index, const TypedValue& value ) 
    {
        throw TypeException( m_typeInfo->getName(), L" type has no fields or array elements");
    }

protected:

    TypedVarImp( const TypeInfoPtr& typeInfo, const DataAccessorPtr &dataSource, const std::wstring& name = L"" ) :
        m_typeInfo( typeInfo ),
        m_varData( dataSource ),
        m_name( name )
        {}

    TypeInfoPtr  m_typeInfo;

    DataAccessorPtr  m_varData;

    std::wstring  m_name;
};

///////////////////////////////////////////////////////////////////////////////

class TypedVarBase : public TypedVarImp
{
public:

    TypedVarBase( const TypeInfoPtr& typeInfo, const DataAccessorPtr &dataSource, const std::wstring& name = L"" ) :
        TypedVarImp( typeInfo, dataSource, name )
        {}

    std::wstring printValue() const;

protected:

    virtual NumVariant getValue() const;

    virtual void setValue(const NumVariant& value);

    virtual std::wstring str();
};

///////////////////////////////////////////////////////////////////////////////

class TypedVarUdt :  public TypedVarImp
{
public:

       TypedVarUdt( const TypeInfoPtr& typeInfo, const DataAccessorPtr &dataSource, const std::wstring& name = L"" ) :
        TypedVarImp( typeInfo, dataSource, name )
        {}

protected:

    virtual NumVariant getValue() const {
        return NumVariant( m_varData->getAddress() );
    }

    virtual TypedVarPtr getElement( const std::wstring& fieldName );

    virtual void setElement( const std::wstring& fieldName, const TypedValue& value);

    virtual  TypedVarPtr getElement( size_t index );

    virtual void setElement( size_t index, const TypedValue& value );

    virtual MEMOFFSET_REL getElementOffset( const std::wstring& fieldName ) {
        return m_typeInfo->getElementOffset( fieldName );
    }

    virtual MEMOFFSET_REL getElementOffset( size_t index ) {
        return m_typeInfo->getElementOffset( index );
    }

    virtual size_t getElementCount() {
        return m_typeInfo->getElementCount();
    }

    virtual std::wstring getElementName( size_t index ) {
        return m_typeInfo->getElementName( index );
    }

    size_t getElementIndex(const std::wstring&  elementName) {
        return m_typeInfo->getElementIndex(elementName);
    }

    MEMDISPLACEMENT getVirtualBaseDisplacement( const std::wstring &fieldName );
    MEMDISPLACEMENT getVirtualBaseDisplacement( size_t index );

    virtual std::wstring str();

    virtual TypedVarPtr getMethod( const std::wstring &name, const std::wstring&  prototype = L"");
    
    virtual TypedVarPtr getMethod( const std::wstring &name, TypeInfoPtr prototype)
    {
        NOT_IMPLEMENTED();
    }


protected:

    TypedVarPtr getMethodRecursive( 
        const std::wstring& methodName,
        const std::wstring& methodPrototype,
        TypeInfoPtr&  baseClass,
        MEMOFFSET_REL startOffset
        );

    TypedVarPtr getVirtualMethodRecursive( 
        const std::wstring& methodName, 
        const std::wstring& methodPrototype,
        TypeInfoPtr&  baseClass,
        MEMOFFSET_REL startOffset
        );

    
};


///////////////////////////////////////////////////////////////////////////////

class TypedVarPointer : public TypedVarImp
{
public:

    TypedVarPointer( const TypeInfoPtr& typeInfo, const DataAccessorPtr &dataSource, const std::wstring& name = L"" ) :
        TypedVarImp( typeInfo, dataSource, name )
        {}


public:

    virtual NumVariant getValue() const {
        return NumVariant( addr64(getSize() == 4 ? m_varData->readDWord() : m_varData->readQWord() ) );
    }

    virtual void setValue(const NumVariant& value) {
        getSize() == 4 ? m_varData->writeDWord(value.asULong()) : m_varData->writeQWord(value.asULongLong());
    }

    virtual TypedVarPtr deref();

    virtual TypedVarPtr getElement( const std::wstring& fieldName ) {
        TypedVarPtr  derefPtr = deref();
        return derefPtr->getElement( fieldName );
    }

    virtual  TypedVarPtr getElement(size_t index);

    virtual std::wstring str();


    std::wstring printValue() const;
};

///////////////////////////////////////////////////////////////////////////////

class TypedVarArray : public TypedVarImp
{
public:

    TypedVarArray( const TypeInfoPtr& typeInfo, const DataAccessorPtr &dataSource, const std::wstring& name = L"") :
        TypedVarImp( typeInfo, dataSource, name )
        {}

    virtual NumVariant getValue() const {
        return NumVariant(m_varData->getAddress() );
    }

    virtual void setElement( size_t index, const TypedValue& value ) {
        getElement(index)->setValue(value);
    }

    virtual size_t getElementCount() {
        return m_typeInfo->getElementCount();
    }

    virtual  TypedVarPtr getElement( size_t index );

    virtual std::wstring str();
};

///////////////////////////////////////////////////////////////////////////////

class TypedVarBitField : public TypedVarImp
{
public:
    TypedVarBitField( const TypeInfoPtr& typeInfo, const DataAccessorPtr &dataSource, const std::wstring& name = L"" ) :
      TypedVarImp( typeInfo, dataSource, name)
      {}

    virtual NumVariant getValue() const;

    virtual void setValue(const NumVariant& value);

    virtual std::wstring str();

    std::wstring printValue() const;
};

///////////////////////////////////////////////////////////////////////////////

class TypedVarEnum : public TypedVarImp
{
public:
    TypedVarEnum( const TypeInfoPtr& typeInfo, const DataAccessorPtr &dataSource, const std::wstring& name = L"" ) :
      TypedVarImp( typeInfo, dataSource, name )
      {}

    virtual NumVariant getValue() const {
        return NumVariant( m_varData->readDWord() );
    }

    virtual std::wstring str();

    std::wstring printValue() const;
};

///////////////////////////////////////////////////////////////////////////////

class TypedVarFunction : public TypedVarImp
{
public:

    TypedVarFunction( const TypeInfoPtr& typeInfo, const DataAccessorPtr &dataSource, const std::wstring& name = L"" ) :
        TypedVarImp( typeInfo, dataSource, name )
    {}

    virtual NumVariant getValue() const {
        return NumVariant( getAddress() );
    }

    virtual size_t getElementCount() {
        return m_typeInfo->getElementCount();
    }

    virtual MEMOFFSET_64 getDebugStart() const  {
        return 0;
    }

    virtual MEMOFFSET_64 getDebugEnd() const  {
        return 0;
    }

    virtual size_t getSize() const  {
        return 0;
    }

    virtual std::wstring str();

    virtual TypedValue call(const TypedValueList& arglst);

protected:

    TypedValueList castArgs(const TypedValueList& arglst);

    TypedValue callCdecl(const TypedValueList& arglst);
    TypedValue callStd(const TypedValueList& arglst);
    TypedValue callFast(const TypedValueList& arglst);
    TypedValue callThis(const TypedValueList& arglst);
    TypedValue callX64(const TypedValueList& arglst);

    void makeCallx86();
    void makeCallx64();
};

///////////////////////////////////////////////////////////////////////////////

class TypedVarVtbl : public TypedVarImp
{
public:

    TypedVarVtbl(const TypeInfoPtr& typeInfo, const DataAccessorPtr &dataSource, const std::wstring& name = L"") :
        TypedVarImp(typeInfo, dataSource, name)
    {}

    virtual NumVariant getValue() const 
    {
        return NumVariant(getAddress());
    }

    virtual size_t getElementCount()
    {
        return m_typeInfo->getElementCount();
    }

    virtual TypedVarPtr getElement(size_t index);

    virtual std::wstring str(); 
};

//////////////////////////////////////////////////////////////////////////////

class TypedVarVoid : public TypedVar
{
public:

    TypedVarVoid()
    {}

    virtual std::wstring str() 
    {
       throw TypeException(L"Not applicable for Void");
    }

    virtual NumVariant getValue() const
    {
       throw TypeException(L"Not applicable for Void");
    } 

    virtual void setValue(const NumVariant& value) 
    {
       throw TypeException(L"Not applicable for Void");
    }

    virtual VarStorage getStorage() const
    {
       throw TypeException(L"Not applicable for Void");
    }

    virtual std::wstring getRegisterName() const
    {
       throw TypeException(L"Not applicable for Void");
    }

    virtual MEMOFFSET_64 getAddress() const 
    {
       throw TypeException(L"Not applicable for Void");
    }

    virtual MEMOFFSET_64 getDebugStart() const
    {
       throw TypeException(L"Not applicable for Void");
    }

    virtual MEMOFFSET_64 getDebugEnd() const
    {
       throw TypeException(L"Not applicable for Void");
    }

    virtual size_t getSize() const
    {
       throw TypeException(L"Not applicable for Void");
    }

    virtual std::wstring getName() const
    {
       throw TypeException(L"Not applicable for Void");
    }

    virtual TypeInfoPtr getType() const;

    virtual TypedVarPtr deref() 
    {
       throw TypeException(L"Not applicable for Void");
    }

    virtual TypedVarPtr getElement( const std::wstring& fieldName ) 
    {
       throw TypeException(L"Not applicable for Void");
    }

    virtual TypedVarPtr getElement( size_t index ) 
    {
       throw TypeException(L"Not applicable for Void");
    }

    virtual MEMOFFSET_REL getElementOffset( const std::wstring& fieldName ) 
    {
       throw TypeException(L"Not applicable for Void");
    }

    virtual MEMOFFSET_REL getElementOffset( size_t index ) 
    {
       throw TypeException(L"Not applicable for Void");
    }

    virtual RELREG_ID getElementOffsetRelativeReg(const std::wstring& fieldName ) 
    {
       throw TypeException(L"Not applicable for Void");
    }

    virtual RELREG_ID getElementOffsetRelativeReg(size_t index ) 
    {
       throw TypeException(L"Not applicable for Void");
    }

    virtual VarStorage getElementStorage(const std::wstring& fieldName)
    {
       throw TypeException(L"Not applicable for Void");
    }

    virtual VarStorage getElementStorage(size_t index)
    {
       throw TypeException(L"Not applicable for Void");
    }

    virtual size_t getElementCount()
     {
       throw TypeException(L"Not applicable for Void");
    }

    virtual std::wstring getElementName( size_t index ) 
    {
       throw TypeException(L"Not applicable for Void");
    }

    virtual size_t getElementIndex( const std::wstring&  elementName )
    {
       throw TypeException(L"Not applicable for Void");
    }

    virtual TypedVarPtr getMethod( const std::wstring &name, const std::wstring&  prototype = L"")
    {
       throw TypeException(L"Not applicable for Void");
    }
    
    virtual TypedVarPtr getMethod( const std::wstring &name, TypeInfoPtr prototype)
    {
       throw TypeException(L"Not applicable for Void");
    }

    virtual unsigned long getElementReg(const std::wstring& fieldName)
    {
       throw TypeException(L"Not applicable for Void");
    }

    virtual unsigned long getElementReg(size_t index)
    {
       throw TypeException(L"Not applicable for Void");
    }

    virtual TypedVarPtr castTo(const std::wstring& typeName) const
    {
       throw TypeException(L"Not applicable for Void");
    }

    virtual TypedVarPtr castTo(const TypeInfoPtr &typeInfo) const
    {
       throw TypeException(L"Not applicable for Void");
    }

    virtual void writeBytes(DataAccessorPtr& stream, size_t bytes = 0) const
    {
       throw TypeException(L"Not applicable for Void");
    }

    virtual TypedValue call(const TypedValueList& arglst)
    {
       throw TypeException(L"Not applicable for Void");
    }

    virtual void setElement( const std::wstring& fieldName, const TypedValue& value) 
    {
       throw TypeException(L"Not applicable for Void");
    }

    virtual void setElement( size_t index, const TypedValue& value ) 
    {
       throw TypeException(L"Not applicable for Void");
    }
};

//////////////////////////////////////////////////////////////////////////////

class SymbolFunction : public TypedVarFunction
{
public:

    SymbolFunction( SymbolPtr& symbol );

    virtual MEMOFFSET_64 getDebugStart() const {
        return getDebugXImpl(SymTagFuncDebugStart);
    }
    virtual MEMOFFSET_64 getDebugEnd() const {
        return getDebugXImpl(SymTagFuncDebugEnd);
    }

    virtual MEMOFFSET_REL getElementOffset( size_t index );

    virtual RELREG_ID getElementOffsetRelativeReg(size_t index );

    virtual MEMOFFSET_REL getElementOffset( const std::wstring& paramName );
 
    virtual RELREG_ID getElementOffsetRelativeReg(const std::wstring& paramName );

    virtual unsigned long getElementReg(const std::wstring& fieldName);

    virtual unsigned long getElementReg(size_t index);

    virtual size_t getElementIndex(const std::wstring& paramName );

    virtual std::wstring getElementName( size_t index );

    virtual VarStorage getElementStorage(const std::wstring& fieldName);

    virtual VarStorage getElementStorage(size_t index);

    virtual size_t getSize() const {
        return m_symbol->getSize();
    }

protected:

    SymbolPtr getChildSymbol(size_t index);

    MEMOFFSET_64 getDebugXImpl(SymTags symTag) const;

    SymbolPtr  m_symbol;
};

///////////////////////////////////////////////////////////////////////////////

class TypedVarMethodBound : public TypedVarFunction
{

public:

    TypedVarMethodBound(
        const TypeInfoPtr& prototype,
        const DataAccessorPtr &dataSource, 
        const std::wstring& name, 
        MEMOFFSET_64 thisValue);

     virtual TypedValue call(const TypedValueList& arglst);

private:

    MEMOFFSET_64   m_this;

};

///////////////////////////////////////////////////////////////////////////////

class TypedVarMethodUnbound : public TypedVarFunction
{
public:

    TypedVarMethodUnbound(const TypeInfoPtr& typeInfo, const DataAccessorPtr &dataSource, const std::wstring& name):
       TypedVarFunction(typeInfo, dataSource, name )
    {}
};

///////////////////////////////////////////////////////////////////////////////

} // namespace kdlib
