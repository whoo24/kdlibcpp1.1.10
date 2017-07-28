#include "stdafx.h"

#include <iomanip>

#include <stdarg.h>

#include <boost/regex.hpp>

#include "kdlib/typedvar.h"
#include "kdlib/module.h"
#include "kdlib/stack.h"
#include "kdlib/cpucontext.h"
#include "kdlib/breakpoint.h"

#include "typedvarimp.h"
#include "typeinfoimp.h"


///////////////////////////////////////////////////////////////////////////////

namespace {

using namespace kdlib;

std::wstring printFieldValue( const TypeInfoPtr& fieldType, const TypedVarPtr& fieldVar )
{
    std::wstringstream  sstr;

    if ( fieldType->isBase() )
    {
        return dynamic_cast<const kdlib::TypedVarBase*>( fieldVar.get() )->printValue();
    }

    if ( fieldType->isPointer() )
    {
        return dynamic_cast<const kdlib::TypedVarPointer*>( fieldVar.get() )->printValue();
    }

    if (fieldType->isBitField())
    {
        return dynamic_cast<const kdlib::TypedVarBitField*>(fieldVar.get())->printValue();
    }

    if (fieldType->isEnum())
    {
        return dynamic_cast<const kdlib::TypedVarEnum*>(fieldVar.get())->printValue();
    }

    return L"";
}


std::wstring getSymbolName( kdlib::SymbolPtr& symbol )
{
    try {
        return symbol->getName();
    }
    catch( kdlib::SymbolException& )
    {}

    return L"";

}

TypedValue castBaseArg(TypeInfoPtr& destType, const TypedValue& arg)
{
    NumVariant  var;

    if ( arg.getType()->isBase() || arg.getType()->isEnum() || arg.getType()->isPointer() )
    {
        var = arg.getValue();
    }
    else
    {
        throw TypeException(L"failed to cast argument");
    }

    if (destType->getName() == L"Char")
        return TypedValue( var.asChar() );

    if (destType->getName() == L"WChar")
        return TypedValue( var.asUShort() );

    if (destType->getName() == L"Int1B")
        return TypedValue( var.asChar() );

    if (destType->getName() == L"UInt1B")
        return TypedValue( var.asUChar() );

    if (destType->getName() == L"Int2B")
        return TypedValue( var.asShort() );

    if (destType->getName() == L"UInt2B") 
        return TypedValue( var.asUShort() );

    if (destType->getName() == L"Int4B")
        return TypedValue( var.asLong() );

    if (destType->getName() == L"UInt4B")
        return TypedValue( var.asULong() );

    if (destType->getName() == L"Int8B" )
        return TypedValue( var.asLongLong() );

    if (destType->getName() == L"UInt8B" )
        return TypedValue( var.asULongLong() );

    if (destType->getName() == L"Long")
        return TypedValue( var.asLong() );

    if (destType->getName() == L"ULong")
        return TypedValue( var.asULong() );

    if (destType->getName() == L"Float")
        return TypedValue( var.asFloat() );

    if (destType->getName() == L"Double")
        return TypedValue( var.asDouble() );

    if (destType->getName() == L"Bool")
        return TypedValue( var.asChar() );

    throw TypeException(L"failed to cast argument");
}


TypedValue castPtrArg(TypeInfoPtr& destType, const TypedValue& arg)
{

    if ( arg.getType()->isPointer() || arg.getType()->isBase() )
    {
        NumVariant  var = arg.getValue();

        if ( destType->getPtrSize() == 4 )
            return TypedValue( var.asULong() );

        if ( destType->getPtrSize() == 8 )
            return TypedValue( var.asULongLong() );
    }
    else if ( arg.getType()->isArray() )
    {
        MEMOFFSET_64  addr = stackAlloc(arg.getSize());
        DataAccessorPtr  dataRange = getMemoryAccessor( addr, arg.getSize() );
        arg.writeBytes(dataRange);

        if ( destType->getPtrSize() == 4 )
            return TypedValue( static_cast<unsigned long>(addr) );

        if ( destType->getPtrSize() == 8 )
            return TypedValue( static_cast<unsigned long long>(addr) );
    }

    throw TypeException(L"failed to cast argument");
}

TypedValue castUdt(TypeInfoPtr& destType, const TypedValue& arg)
{
    if (destType->getName() == arg.getType()->getName() )
    {
        if ( destType->getPtrSize() == 4 && destType->getSize() <= 4 )
        {
            DataAccessorPtr  data = getCacheAccessor(sizeof(int));
            arg.writeBytes(data);
            return loadTypedVar(L"UInt4B", data);
        }

        if ( destType->getPtrSize() == 8 && destType->getSize() <= 8 )
        {
            DataAccessorPtr  data = getCacheAccessor(sizeof(__int64));
            arg.writeBytes(data);
            return loadTypedVar(L"UInt8B", data);
        }

        return arg;
    }

    NOT_IMPLEMENTED();
}


///////////////////////////////////////////////////////////////////////////////

} // end noname namespace

namespace kdlib {

///////////////////////////////////////////////////////////////////////////////

TypedVarPtr getTypedVar( const TypeInfoPtr& typeInfo, const DataAccessorPtr &dataSource, const std::wstring& name = L"" )
{
    if ( typeInfo->isBase() )
        return TypedVarPtr( new TypedVarBase( typeInfo, dataSource, name ) );

    if ( typeInfo->isUserDefined() )
        return TypedVarPtr( new TypedVarUdt( typeInfo, dataSource, name ) );

    if ( typeInfo->isPointer() )
        return TypedVarPtr( new TypedVarPointer( typeInfo, dataSource, name ) );

    if ( typeInfo->isArray() )
        return TypedVarPtr( new TypedVarArray( typeInfo, dataSource, name ) );

    if ( typeInfo->isBitField() )
        return TypedVarPtr( new TypedVarBitField( typeInfo, dataSource, name ) );

    if ( typeInfo->isEnum() )
        return TypedVarPtr( new TypedVarEnum( typeInfo, dataSource, name ) );

    if ( typeInfo->isFunction() )
        return TypedVarPtr( new TypedVarFunction( typeInfo, dataSource, name ) );

    if (typeInfo->isVtbl() )
        return TypedVarPtr( new TypedVarVtbl( typeInfo, dataSource, name ) );

    NOT_IMPLEMENTED();
}

///////////////////////////////////////////////////////////////////////////////

TypedVarPtr loadTypedVar( SymbolPtr &symbol )
{
    if ( SymTagFunction == symbol->getSymTag() )
    {
        return TypedVarPtr( new SymbolFunction( symbol ) );
    }

    if ( LocIsConstant != symbol->getLocType() )
    {
        MEMOFFSET_64 offset = symbol->getVa();

        TypeInfoPtr varType = loadType( symbol );

        return getTypedVar( varType, getMemoryAccessor(offset,varType->getSize()), ::getSymbolName(symbol) );
    }

    NOT_IMPLEMENTED();
}

///////////////////////////////////////////////////////////////////////////////

TypedVarPtr loadTypedVar( const std::wstring &varName )
{
    std::wstring     moduleName;
    std::wstring     symName;

    splitSymName( varName, moduleName, symName );

    ModulePtr  module;

    if ( moduleName.empty() )
    {
        StackFramePtr  frame = getCurrentStackFrame();
        if (frame->findLocalVar(varName))
            return frame->getLocalVar(varName);

        if (frame->findParam(varName))
            return frame->getTypedParam(varName);

        if (frame->findStaticVar(varName))
            return frame->getStaticVar(varName);

        MEMOFFSET_64 moduleOffset = findModuleBySymbol( symName );
        module = loadModule( moduleOffset );
    }
    else
    {
        module = loadModule( moduleName );
    }

    SymbolPtr  symVar = module->getSymbolScope()->getChildByName( symName );

    return loadTypedVar( symVar );
}

///////////////////////////////////////////////////////////////////////////////

TypedVarPtr loadTypedVar( const std::wstring &typeName, MEMOFFSET_64 offset )
{
    offset = addr64(offset);

    TypeInfoPtr varType = loadType( typeName );

    return getTypedVar( varType, getMemoryAccessor(offset,varType->getSize()) );
}

///////////////////////////////////////////////////////////////////////////////

TypedVarPtr loadTypedVar( const TypeInfoPtr &varType, MEMOFFSET_64 offset )
{
    offset = addr64(offset);

    if ( !varType )
        throw DbgException( "type info is null");

    return getTypedVar( varType, getMemoryAccessor(offset,varType->getSize()) );
}


///////////////////////////////////////////////////////////////////////////////

TypedVarPtr loadTypedVar( const std::wstring &typeName, const DataAccessorPtr& dataSource)
{
    TypeInfoPtr varType = loadType( typeName );

    return getTypedVar( varType, dataSource );
}

///////////////////////////////////////////////////////////////////////////////

TypedVarPtr loadTypedVar(const TypeInfoPtr &varType, const DataAccessorPtr& dataSource)
{
    if (!varType)
        throw DbgException("type info is null");

    return getTypedVar(varType, dataSource);
}

///////////////////////////////////////////////////////////////////////////////

TypedVarPtr loadTypedVar( const std::wstring &funcName, const std::wstring &prototype)
{
    std::wstring     moduleName;
    std::wstring     symName;

    splitSymName( funcName, moduleName, symName );

    ModulePtr  module;

    if ( moduleName.empty() )
    {
        MEMOFFSET_64 moduleOffset = findModuleBySymbol( symName );
        module = loadModule( moduleOffset );
    }
    else
    {
        module = loadModule( moduleName );
    }

    SymbolPtrList  functions = module->getSymbolScope()->findChildren(SymTagFunction);

    for ( SymbolPtrList::iterator  it = functions.begin(); it != functions.end(); ++it )
    {
        SymbolPtr  sym = *it;
        if ( sym->getName() == funcName  )
        {
            TypeInfoPtr  funcType = loadType(sym->getType());
            if ( isPrototypeMatch( funcType, prototype ) )
                return loadTypedVar(sym);
        }
    }

    throw TypeException(L"failed to find symbol");
}

///////////////////////////////////////////////////////////////////////////////

TypedVarPtr containingRecord( MEMOFFSET_64 offset, const std::wstring &typeName, const std::wstring &fieldName )
{
    std::wstring     moduleName;
    std::wstring     symName;

    offset = addr64(offset);

    splitSymName( typeName, moduleName, symName );

     ModulePtr  module;

    if ( moduleName.empty() )
    {
        MEMOFFSET_64 moduleOffset = findModuleBySymbol( symName );
        module = loadModule( moduleOffset );
    }
    else
    {
        module = loadModule( moduleName );
    }

    TypeInfoPtr typeInfo = module->getTypeByName( symName );

    return containingRecord( offset, typeInfo, fieldName );
}

///////////////////////////////////////////////////////////////////////////////

TypedVarPtr containingRecord( MEMOFFSET_64 offset, TypeInfoPtr &typeInfo, const std::wstring &fieldName )
{
    if ( !typeInfo )
        throw DbgException( "type info is null");

    offset = addr64( offset );

    return loadTypedVar( typeInfo, offset - typeInfo->getElementOffset( fieldName ) );
}

///////////////////////////////////////////////////////////////////////////////

TypedVarList loadTypedVarList( MEMOFFSET_64 offset, const std::wstring &typeName, const std::wstring &fieldName )
{
    std::wstring     moduleName;
    std::wstring     symName;

    splitSymName( typeName, moduleName, symName );

     ModulePtr  module;

    if ( moduleName.empty() )
    {
        MEMOFFSET_64 moduleOffset = findModuleBySymbol( symName );
        module = loadModule( moduleOffset );
    }
    else
    {
        module = loadModule( moduleName );
    }

    TypeInfoPtr typeInfo = module->getTypeByName( symName );

    return loadTypedVarList( offset, typeInfo, fieldName );
}

///////////////////////////////////////////////////////////////////////////////

TypedVarList loadTypedVarList( MEMOFFSET_64 offset, TypeInfoPtr &typeInfo, const std::wstring &fieldName )
{
    if ( !typeInfo )
        throw DbgException( "type info is null" );

    offset = addr64(offset);

    TypedVarList  lst;
    lst.reserve(100);

    MEMOFFSET_64  entryAddress = 0;
    TypeInfoPtr  fieldTypeInfo = typeInfo->getElement( fieldName );
    size_t  psize = fieldTypeInfo->getPtrSize();

    if ( fieldTypeInfo->getName() == ( typeInfo->getName() + L"*" ) )
    {
        for( entryAddress = ptrPtr(offset, psize); addr64(entryAddress) != offset && entryAddress != NULL; entryAddress = ptrPtr( entryAddress + typeInfo->getElementOffset(fieldName), psize ) )
            lst.push_back( loadTypedVar( typeInfo, entryAddress ) );
    }
    else
    {
        for( entryAddress = ptrPtr( offset, psize ); addr64(entryAddress) != offset && entryAddress != NULL; entryAddress = ptrPtr( entryAddress, psize ) )
            lst.push_back( containingRecord( entryAddress, typeInfo, fieldName ) );
    }

    return lst;
}

///////////////////////////////////////////////////////////////////////////////

TypedVarList loadTypedVarArray( MEMOFFSET_64 offset, const std::wstring &typeName, size_t number )
{
    std::wstring     moduleName;
    std::wstring     symName;

    offset = addr64(offset);

    splitSymName( typeName, moduleName, symName );

     ModulePtr  module;

    if ( moduleName.empty() )
    {
        MEMOFFSET_64 moduleOffset = findModuleBySymbol( symName );
        module = loadModule( moduleOffset );
    }
    else
    {
        module = loadModule( moduleName );
    }

    TypeInfoPtr typeInfo = module->getTypeByName( symName );

    return loadTypedVarArray( offset, typeInfo, number );
}

///////////////////////////////////////////////////////////////////////////////

TypedVarList loadTypedVarArray( MEMOFFSET_64 offset, TypeInfoPtr &typeInfo, size_t number )
{
   if ( !typeInfo )
        throw DbgException( "type info is null" );

    offset = addr64(offset); 

    TypedVarList  lst;
    lst.reserve(100);

    for( size_t i = 0; i < number; ++i )
        lst.push_back( loadTypedVar( typeInfo, offset + i * typeInfo->getSize() ) );
   
    return lst;
}

///////////////////////////////////////////////////////////////////////////////

TypedVarPtr loadCharVar( char var ) 
{
    DataAccessorPtr  accessor = getCacheAccessor( sizeof(char) );
    accessor->writeSignByte(var);
    return getTypedVar( loadType(L"Int1B"), accessor);
}

///////////////////////////////////////////////////////////////////////////////

TypedVarPtr loadShortVar( short var )
{
    DataAccessorPtr  accessor = getCacheAccessor( sizeof(short) );
    accessor->writeSignWord(var);
    return getTypedVar( loadType(L"Int2B"), accessor );
}

///////////////////////////////////////////////////////////////////////////////

TypedVarPtr loadLongVar( long var )
{
    DataAccessorPtr  accessor = getCacheAccessor( sizeof(long) );
    accessor->writeSignDWord(var);
    return getTypedVar( loadType(L"Int4B"), accessor );
}

///////////////////////////////////////////////////////////////////////////////

TypedVarPtr loadLongLongVar( long long var )
{
    DataAccessorPtr  accessor = getCacheAccessor( sizeof(long long) );
    accessor->writeSignQWord(var);
    return getTypedVar( loadType(L"Int8B"), accessor );
}

///////////////////////////////////////////////////////////////////////////////

TypedVarPtr loadUCharVar( unsigned char var ) 
{
    DataAccessorPtr  accessor = getCacheAccessor( sizeof(unsigned char) );
    accessor->writeByte(var);
    return getTypedVar( loadType(L"UInt1B"), accessor );
}

///////////////////////////////////////////////////////////////////////////////

TypedVarPtr loadUShortVar( unsigned short var )
{
    DataAccessorPtr  accessor = getCacheAccessor( sizeof(unsigned short) );
    accessor->writeWord(var);
    return getTypedVar( loadType(L"UInt2B"), accessor );
}

///////////////////////////////////////////////////////////////////////////////

TypedVarPtr loadULongVar( unsigned long var )
{
    DataAccessorPtr  accessor = getCacheAccessor( sizeof(unsigned long) );
    accessor->writeDWord(var);
    return getTypedVar( loadType(L"UInt4B"), accessor );
}

///////////////////////////////////////////////////////////////////////////////

TypedVarPtr loadULongLongVar( unsigned long long var )
{
    DataAccessorPtr  accessor = getCacheAccessor( sizeof(unsigned long long) );
    accessor->writeQWord(var);
    return getTypedVar( loadType(L"UInt8B"), accessor );
}

///////////////////////////////////////////////////////////////////////////////

TypedVarPtr loadIntVar( int var )
{
    DataAccessorPtr  accessor = getCacheAccessor( sizeof(int) );
    accessor->writeSignDWord(var);
    return getTypedVar( loadType(L"Int4B"), accessor );
}

///////////////////////////////////////////////////////////////////////////////

TypedVarPtr loadUIntVar( unsigned int var )
{
    DataAccessorPtr  accessor = getCacheAccessor( sizeof(unsigned int) );
    accessor->writeDWord(var);
    return getTypedVar( loadType(L"UInt4B"), accessor );
}

///////////////////////////////////////////////////////////////////////////////

TypedVarPtr loadWCharVar(wchar_t var) 
{
    DataAccessorPtr  accessor = getCacheAccessor( sizeof(wchar_t) );
    accessor->writeWord(var);
    return getTypedVar( loadType(L"WChar"), accessor );
}

///////////////////////////////////////////////////////////////////////////////

TypedVarPtr loadBoolVar(bool var) 
{
    DataAccessorPtr  accessor = getCacheAccessor( sizeof(char) );
    accessor->writeByte(var);
    return getTypedVar( loadType(L"Bool"), accessor );
}

///////////////////////////////////////////////////////////////////////////////

TypedVarPtr loadFloatVar( float var )
{
    DataAccessorPtr  accessor = getCacheAccessor( sizeof(float) );
    accessor->writeFloat(var);
    return getTypedVar( loadType(L"Float"), accessor );
}

///////////////////////////////////////////////////////////////////////////////

TypedVarPtr loadDoubleVar( double var )
{
    DataAccessorPtr  accessor = getCacheAccessor( sizeof(double) );
    accessor->writeDouble(var);
    return getTypedVar( loadType(L"Double"), accessor );
}

///////////////////////////////////////////////////////////////////////////////

TypedVarPtr TypedVarImp::castTo(const std::wstring& typeName) const
{
    return loadTypedVar(typeName, m_varData);
}

///////////////////////////////////////////////////////////////////////////////

TypedVarPtr TypedVarImp::castTo(const TypeInfoPtr &typeInfo) const
{
    return loadTypedVar(typeInfo, m_varData);
}

///////////////////////////////////////////////////////////////////////////////

//std::wstring TypedVarImp::getLocation()
//{
//    if (m_varData->getStorageType() == kdlib::RegisterVar)
//        return std::wstring(L"@") + m_varData->getRegisterName();
//
//    std::wstringstream  sstr;
//    sstr << L"0x" << std::hex << m_varData->getAddress();
//    return sstr.str();
//}

///////////////////////////////////////////////////////////////////////////////

NumVariant TypedVarBase::getValue() const
{
    if ( m_typeInfo->getName() == L"Char" )
        return NumVariant( m_varData->readSignByte() );

    if ( m_typeInfo->getName() == L"WChar" )
        return NumVariant( m_varData->readSignWord() );

    if ( m_typeInfo->getName() == L"Long" )
        return NumVariant( m_varData->readSignDWord() );

    if ( m_typeInfo->getName() == L"ULong" )
        return NumVariant( m_varData->readDWord() );

    if ( m_typeInfo->getName() == L"Int1B" )
        return NumVariant( m_varData->readByte() );

    if ( m_typeInfo->getName() == L"UInt1B" )
        return NumVariant( m_varData->readByte() );

    if ( m_typeInfo->getName() == L"Int2B" )
        return NumVariant( m_varData->readSignWord() );

    if ( m_typeInfo->getName() == L"UInt2B" )
        return NumVariant( m_varData->readWord() );

    if ( m_typeInfo->getName() == L"Int4B" )
        return NumVariant( m_varData->readSignDWord() );

    if ( m_typeInfo->getName() == L"UInt4B" )
        return NumVariant( m_varData->readDWord() );

    if ( m_typeInfo->getName() == L"Int8B" )
        return NumVariant( m_varData->readSignQWord() );

    if ( m_typeInfo->getName() == L"UInt8B" )
        return NumVariant( m_varData->readQWord() );

    if ( m_typeInfo->getName() == L"Float" )
        return NumVariant( m_varData->readFloat() );

    if ( m_typeInfo->getName() == L"Double" )
        return NumVariant( m_varData->readDouble() );

    if ( m_typeInfo->getName() == L"Bool" )
        return NumVariant( m_varData->readByte() );

    if ( m_typeInfo->getName() == L"Hresult" )
        return NumVariant( m_varData->readDWord() );

    throw TypeException(  m_typeInfo->getName(), L" unsupported based type");
}

///////////////////////////////////////////////////////////////////////////////

void TypedVarBase::setValue(const NumVariant& value)
{
    if ( m_typeInfo->getName() == L"Char" )
        return m_varData->writeSignByte( value.asChar() );

    if ( m_typeInfo->getName() == L"WChar" )
        return m_varData->writeSignWord(value.asShort() );

    if ( m_typeInfo->getName() == L"Long" )
        return m_varData->writeSignDWord(value.asLong());

    if ( m_typeInfo->getName() == L"ULong" )
        return m_varData->writeDWord(value.asULong());

    if ( m_typeInfo->getName() == L"Int1B" )
        return m_varData->writeSignByte(value.asChar());
        
    if ( m_typeInfo->getName() == L"UInt1B" )
        return m_varData->writeByte(value.asUChar());

    if ( m_typeInfo->getName() == L"Int2B" )
        return m_varData->writeSignWord(value.asShort());

    if ( m_typeInfo->getName() == L"UInt2B" )
        return m_varData->writeWord(value.asUShort());

    if ( m_typeInfo->getName() == L"Int4B" )
        return m_varData->writeSignDWord(value.asLong());

    if ( m_typeInfo->getName() == L"UInt4B" )
        return m_varData->writeDWord(value.asULong());
        
    if ( m_typeInfo->getName() == L"Int8B" )
        return m_varData->writeSignQWord(value.asLongLong());

    if ( m_typeInfo->getName() == L"UInt8B" )
        return m_varData->writeQWord(value.asULongLong());

    if ( m_typeInfo->getName() == L"Float" )
        return m_varData->writeFloat(value.asFloat());

    if ( m_typeInfo->getName() == L"Double" )
        return m_varData->writeDouble(value.asDouble());

    if ( m_typeInfo->getName() == L"Bool" )
        return m_varData->writeByte(value.asUChar());

    if ( m_typeInfo->getName() == L"Hresult" )
        return m_varData->writeDWord(value.asULong());

    throw TypeException(  m_typeInfo->getName(), L" unsupported based type");
}

///////////////////////////////////////////////////////////////////////////////

std::wstring TypedVarBase::str()
{
    std::wstringstream  sstr;

    sstr << m_typeInfo->getName() << L" at " << m_varData->getLocationAsStr();
    sstr << " Value: ";
    try
    {
        sstr << L"0x" << getValue().asHex() <<  L" (" << getValue().asStr() <<  L")";
    }
    catch (const MemoryException &)
    {
        sstr << L"????";
    }

    return sstr.str();
}

///////////////////////////////////////////////////////////////////////////////

std::wstring TypedVarBase::printValue() const
{
    std::wstringstream  sstr;

    try {
        sstr << L"0x" << getValue().asHex() <<  L" (" << getValue().asStr() <<  L")";
        return sstr.str();
    } catch(MemoryException& )
    {}

    return L"Invalid memory";
}

///////////////////////////////////////////////////////////////////////////////

TypedVarPtr TypedVarUdt::getElement( const std::wstring& fieldName )
{
    TypeInfoPtr fieldType = m_typeInfo->getElement( fieldName );

    if ( m_typeInfo->isStaticMember(fieldName) )
    {
        MEMOFFSET_64  staticOffset = m_typeInfo->getElementVa(fieldName);

        //if ( staticOffset == 0 )
        //   NOT_IMPLEMENTED();

        return  loadTypedVar( fieldType, staticOffset );
    }

    MEMOFFSET_32   fieldOffset = m_typeInfo->getElementOffset(fieldName);

    if ( m_typeInfo->isVirtualMember( fieldName ) )
    {
        fieldOffset += getVirtualBaseDisplacement( fieldName );
    }

    return  loadTypedVar( fieldType,  m_varData->copy(fieldOffset, fieldType->getSize()) );
}
///////////////////////////////////////////////////////////////////////////////

void TypedVarUdt::setElement( const std::wstring& fieldName, const TypedValue& value)
{
    getElement(fieldName)->setValue(value);
}
///////////////////////////////////////////////////////////////////////////////

TypedVarPtr TypedVarUdt::getMethod( const std::wstring &methodName, const std::wstring&  prototype)
{

    TypedVarPtr  methodVar = getMethodRecursive(methodName, prototype, m_typeInfo, 0);
    if ( methodVar )
        return methodVar;

    std::wstringstream  sstr;
    sstr << m_typeInfo->getName() << " has no this method :" << methodName;
    throw TypeException(sstr.str() );
}

///////////////////////////////////////////////////////////////////////////////

TypedVarPtr TypedVarUdt::getMethodRecursive( 
    const std::wstring& methodName,
    const std::wstring& methodPrototype, 
    TypeInfoPtr&  typeInfo,
    MEMOFFSET_REL startOffset
    )
{
    try {   

        TypeInfoPtr methodType = typeInfo->getMethod(methodName, methodPrototype);

        if ( methodType->isVirtual() )
        {
            return getVirtualMethodRecursive(methodName, getMethodPrototype(methodType), typeInfo, startOffset);
        }

        MEMOFFSET_64  funcStart = 0;
        try {
            funcStart = methodType->getValue().asULongLong();
        }
        catch(TypeException&)
        {}

        std::wstring  name = typeInfo->getName();
        name += L"::";
        name += methodName;

        if ( !methodType->hasThis() )
        {
            return TypedVarPtr( new TypedVarMethodUnbound(methodType, getMemoryAccessor(funcStart, 0 ), name ) );
        }
        else
        {
            MEMOFFSET_64  thisValue = m_varData->getAddress();
            return TypedVarPtr( new TypedVarMethodBound(methodType, getMemoryAccessor(funcStart, 0 ), name, thisValue ) );
        }
    }
    catch( TypeException& )
    {}

    for ( size_t i = 0; i < typeInfo->getBaseClassesCount(); ++i )
    {
        TypeInfoPtr  baseClass = typeInfo->getBaseClass(i);

        TypedVarPtr  methodVar = getMethodRecursive(methodName, methodPrototype, baseClass, startOffset + typeInfo->getBaseClassOffset(i) );
        if ( methodVar )
            return methodVar;
    }

    return TypedVarPtr();
}

///////////////////////////////////////////////////////////////////////////////

TypedVarPtr TypedVarUdt::getVirtualMethodRecursive( 
    const std::wstring& methodName,
    const std::wstring& methodPrototype, 
    TypeInfoPtr&  classType,
    MEMOFFSET_REL  startOffset
    )
{
    for ( size_t i = 0; i < classType->getBaseClassesCount(); ++i )
    {
        TypedVarPtr  virtMethod;

        if ( classType->isBaseClassVirtual(i) )
        {
             TypeInfoPtr  baseClass = classType->getBaseClass(i);

             std::wstring  baseClassName = baseClass->getName();

             MEMOFFSET_32 virtualBasePtr = 0;
             size_t  virtualDispIndex = 0;
             size_t  virtualDispSize = 0;

             m_typeInfo->getBaseClassVirtualDisplacement(baseClassName, virtualBasePtr, virtualDispIndex, virtualDispSize );
    
             MEMOFFSET_64 vfnptr = m_varData->getAddress() + virtualBasePtr;
             MEMOFFSET_64 vtbl = m_typeInfo->getPtrSize() == 4 ? ptrDWord( vfnptr ) : ptrQWord(vfnptr);
             MEMDISPLACEMENT displacement =  ptrSignDWord( vtbl + virtualDispIndex*virtualDispSize );
             MEMOFFSET_REL baseClassOffset = virtualBasePtr + displacement;

             virtMethod = getVirtualMethodRecursive(methodName, methodPrototype, baseClass, baseClassOffset );
        }
        else
        {
            TypeInfoPtr  baseClass = classType->getBaseClass(i);

            std::wstring baseClassName = baseClass->getName();

            MEMOFFSET_REL  baseClassOffset = classType->getBaseClassOffset(i);

            virtMethod = getVirtualMethodRecursive(methodName, methodPrototype, baseClass, startOffset + baseClassOffset );
        }

        if ( virtMethod )
            return virtMethod;
    }

    TypeInfoPtr methodType;

    try {  

        std::wstring className = classType->getName();

        methodType = classType->getMethod(methodName, methodPrototype);

    }
    catch( TypeException& )
    {}

    if (!methodType || !methodType->isVirtual() )
        return TypedVarPtr();

    TypeInfoPtr  vtblType = classType->getVTBL();
    TypedVarPtr  vtbl = loadTypedVar( vtblType->ptrTo(), m_varData->getAddress() + startOffset )->deref();
    MEMOFFSET_REL  methodOffset = methodType->getVtblOffset();

    std::wstring  name = classType->getName();
    name += L"::";
    name += methodName;

    MEMOFFSET_64  thisValue = m_varData->getAddress();
    MEMOFFSET_64  methodAddr = vtbl->getElement( methodOffset / vtblType->getPtrSize() )->asULongLong();

    return TypedVarPtr( new TypedVarMethodBound(methodType, getMemoryAccessor(methodAddr, 0 ), name, thisValue ) );
}

///////////////////////////////////////////////////////////////////////////////

TypedVarPtr TypedVarUdt::getElement( size_t index )
{
    TypeInfoPtr fieldType = m_typeInfo->getElement( index );

    if ( m_typeInfo->isStaticMember(index) )
    {
        MEMOFFSET_64  staticOffset = m_typeInfo->getElementVa(index);

        return  loadTypedVar( fieldType, staticOffset );
    }

    MEMOFFSET_32   fieldOffset = m_typeInfo->getElementOffset(index);

    if ( m_typeInfo->isVirtualMember( index ) )
    {
        fieldOffset += getVirtualBaseDisplacement( index );
    }

    return  loadTypedVar( fieldType, m_varData->copy(fieldOffset, fieldType->getSize()) );
}

///////////////////////////////////////////////////////////////////////////////

void TypedVarUdt::setElement( size_t index, const TypedValue& value )
{
    getElement(index)->setValue(value);
}

///////////////////////////////////////////////////////////////////////////////

MEMDISPLACEMENT TypedVarUdt::getVirtualBaseDisplacement( const std::wstring &fieldName )
{
    MEMOFFSET_32 virtualBasePtr = 0;
    size_t  virtualDispIndex = 0;
    size_t  virtualDispSize = 0;
    m_typeInfo->getVirtualDisplacement( fieldName, virtualBasePtr, virtualDispIndex, virtualDispSize );

    MEMOFFSET_64 vfnptr = m_varData->getAddress() + virtualBasePtr;
    MEMOFFSET_64 vtbl = m_typeInfo->getPtrSize() == 4 ? ptrDWord( vfnptr ) : ptrQWord(vfnptr);

    MEMDISPLACEMENT     displacement =  ptrSignDWord( vtbl + virtualDispIndex*virtualDispSize );

    return virtualBasePtr + displacement;
}

///////////////////////////////////////////////////////////////////////////////

MEMDISPLACEMENT TypedVarUdt::getVirtualBaseDisplacement( size_t index )
{
    MEMOFFSET_32 virtualBasePtr = 0;
    size_t  virtualDispIndex = 0;
    size_t  virtualDispSize = 0;
    m_typeInfo->getVirtualDisplacement( index, virtualBasePtr, virtualDispIndex, virtualDispSize );

    MEMOFFSET_64 vfnptr = m_varData->getAddress() + virtualBasePtr;

    if ( !vfnptr )
        return 0; // объект может быть еще не инциализированным  или находится в процессе удаления

    MEMOFFSET_64 vtbl = m_typeInfo->getPtrSize() == 4 ? ptrDWord( vfnptr ) : ptrQWord(vfnptr);

    MEMDISPLACEMENT     displacement =  ptrSignDWord( vtbl + virtualDispIndex*virtualDispSize );
    return virtualBasePtr + displacement;
}

///////////////////////////////////////////////////////////////////////////////

std::wstring TypedVarUdt::str()
{
    std::wstringstream  sstr;

    sstr << L"struct/class: " << m_typeInfo->getName() << L" at " <<  m_varData->getLocationAsStr() << std::endl;
    
    for ( size_t i = 0; i < m_typeInfo->getElementCount(); ++i )
    {
        TypeInfoPtr     fieldType = m_typeInfo->getElement(i);
        TypedVarPtr     fieldVar;

        if ( m_typeInfo->isStaticMember(i) )
        {
            MEMOFFSET_64  staticOffset = m_typeInfo->getElementVa(i);
            if ( staticOffset != 0 )
                fieldVar = loadTypedVar( fieldType, staticOffset );

            sstr << L"   =" << std::right << std::setw(10) << std::setfill(L'0') << std::hex << staticOffset;
            sstr << L" " << std::left << std::setw(18) << std::setfill(L' ') << m_typeInfo->getElementName(i) << ':';
        }
        else
        {
            MEMOFFSET_32   fieldOffset = m_typeInfo->getElementOffset(i);

            if ( m_typeInfo->isVirtualMember(i) )
            {
                fieldOffset += getVirtualBaseDisplacement(i);
            }

            fieldVar = loadTypedVar( fieldType, m_varData->copy(fieldOffset, fieldType->getSize()) );
            sstr << L"   +" << std::right << std::setw(4) << std::setfill(L'0') << std::hex << fieldOffset;
            sstr << L" " << std::left << std::setw(24) << std::setfill(L' ') << m_typeInfo->getElementName(i) << ':';
        }

        sstr << " " << std::left << fieldType->getName();

        if ( fieldVar )
            sstr << L"   " << ::printFieldValue( fieldType, fieldVar );
        else
            sstr << L"   " << L"failed to get value";

        sstr << std::endl;
    }

    return sstr.str();
}

///////////////////////////////////////////////////////////////////////////////

TypedVarPtr TypedVarPointer::deref()
{
    return loadTypedVar( 
                m_typeInfo->deref(), 
                m_typeInfo->getPtrSize() == 4 ? m_varData->readDWord() : m_varData->readQWord());
}

///////////////////////////////////////////////////////////////////////////////

std::wstring TypedVarPointer::str()
{
    std::wstringstream   sstr;

    sstr << L"Ptr " << m_typeInfo->getName() << L" at " <<  m_varData->getLocationAsStr();
    sstr << L" Value: " <<  printValue();

    return sstr.str();
}

///////////////////////////////////////////////////////////////////////////////

std::wstring TypedVarPointer::printValue() const
{
    std::wstringstream   sstr;

    try {

         sstr << L"0x" << getValue().asHex() <<  L" (" << getValue().asStr() <<  L")";
         return sstr.str();

    }
    catch(MemoryException&)
    {}

    return L"Invalid memory";
}

///////////////////////////////////////////////////////////////////////////////

TypedVarPtr TypedVarPointer::getElement(size_t index)
{
    TypeInfoPtr     elementType = m_typeInfo->deref();

    return loadTypedVar(
        elementType,
        m_typeInfo->getPtrSize() == 4 ? m_varData->readDWord() : m_varData->readQWord() +
        elementType->getSize()*index);
}

///////////////////////////////////////////////////////////////////////////////

TypedVarPtr TypedVarArray::getElement( size_t index )
{
    TypeInfoPtr     elementType = m_typeInfo->getElement(0);

    if ( m_varData->getStorageType() == MemoryVar )
        return loadTypedVar( elementType, getMemoryAccessor(m_varData->getAddress() + elementType->getSize()*index, elementType->getSize()) );

    if ( index >= m_typeInfo->getElementCount() )
        throw IndexException( index );

    return loadTypedVar( elementType,  m_varData->copy(elementType->getSize()*index, elementType->getSize()) );
}

///////////////////////////////////////////////////////////////////////////////

std::wstring TypedVarArray::str()
{
    std::wstringstream   sstr;

    sstr << m_typeInfo->getName() << L" at " <<  m_varData->getLocationAsStr();

    return sstr.str();
}

///////////////////////////////////////////////////////////////////////////////

NumVariant TypedVarBitField::getValue() const
{
    NumVariant  var = loadTypedVar( m_typeInfo->getBitType(), m_varData)->getValue();
    NumVariant  one = ( var ^ var ) + '\1';

    BITOFFSET  bitWidth = m_typeInfo->getBitWidth();
    BITOFFSET  bitOffset = m_typeInfo->getBitOffset();

    if ( var.isSigned() )
    {
        if ( ( var & ( one << ( bitOffset + bitWidth - one ) ) ) != 0 )
        {
            var =  ( var >> bitOffset ) | ~( ( one << bitWidth ) - one );
        }
        else
        {
            var = ( var >> bitOffset ) & ( ( one << bitWidth ) - one );
        }
    }
    else
    {
        var = ( var >> bitOffset ) & ( ( one << bitWidth ) - one );
    }

    return var;
}

///////////////////////////////////////////////////////////////////////////////

void TypedVarBitField::setValue(const NumVariant& value)
{
    TypedVarPtr  var = loadTypedVar( m_typeInfo->getBitType(), m_varData);

    BITOFFSET  bitWidth = m_typeInfo->getBitWidth();
    BITOFFSET  bitOffset = m_typeInfo->getBitOffset();
    unsigned long long   bitValue = var->getValue().asULongLong();

    bitValue &= ~( ( ( 1ULL << bitWidth ) - 1ULL ) << bitOffset );
    bitValue |= ( value.asULongLong() & ( ( 1ULL <<  bitWidth ) - 1 ) ) << bitOffset;

    var->setValue(bitValue);
}

///////////////////////////////////////////////////////////////////////////////

std::wstring TypedVarBitField::str()
{
    std::wstringstream   sstr;

    sstr << L"BitField " << m_typeInfo->getName() << L" at " <<  m_varData->getLocationAsStr();
    sstr << L" Value: " <<  printValue();

    return sstr.str();
}

///////////////////////////////////////////////////////////////////////////////

std::wstring TypedVarBitField::printValue() const
{
    std::wstringstream  sstr;

    try {
        sstr << L"0x" << getValue().asHex() << L" (" << getValue().asStr() << L")";
        return sstr.str();
    }
    catch (MemoryException&)
    {
    }

    return L"Invalid memory";
}

///////////////////////////////////////////////////////////////////////////////

std::wstring TypedVarEnum::str()
{
    std::wstringstream       sstr;

    sstr << L"enum: " << m_typeInfo->getName() << L" at " <<  m_varData->getLocationAsStr();
    sstr << L" Value: " << printValue();

    return sstr.str();
}

///////////////////////////////////////////////////////////////////////////////

std::wstring TypedVarEnum::printValue() const
{
    std::wstringstream   sstr;

    try {

        unsigned long   ulongVal1 = getValue().asULong();

        for ( size_t i = 0; i < m_typeInfo->getElementCount(); ++i )
        {
            unsigned long   ulongVal2 = m_typeInfo->getElement(i)->getValue().asULong();
            if ( ulongVal1 == ulongVal2 )
            {
                sstr << m_typeInfo->getElementName(i);
                sstr << L" (" << getValue().asHex() << L")";

                return sstr.str();
            }
        }

        sstr << getValue().asHex();
        sstr << L" ( No matching name )";
    }
    catch( MemoryException& )
    {
        sstr << L"????";
    }

    return sstr.str();
}

///////////////////////////////////////////////////////////////////////////////

std::wstring TypedVarFunction::str()
{ 
    std::wstringstream  sstr;

    sstr << L"Function: " << m_typeInfo->getName() << " at " << std::hex << L"0x" << getAddress() << std::endl;
    
    return sstr.str();
}

///////////////////////////////////////////////////////////////////////////////

TypedValue  TypedVarFunction::call(const TypedValueList& arglst)
{
    if ( getAddress() == 0 )
        throw TypeException(L"function has no body");

    switch (getCPUMode() )
    {
        case CPU_I386:
        {
            switch( m_typeInfo->getCallingConvention() ) 
            {
                case CallConv_NearC:
                    return callCdecl(arglst);

                case CallConv_NearStd:
                    return callStd(arglst);

                case CallConv_NearFast:
                    return callFast(arglst);

                case CallConv_ThisCall:
                    return callThis(arglst);

                default:
                    throw TypeException(L"unsupported calling convention");
            }

            break;
        }
        case CPU_AMD64:
            return callX64(arglst);
    }

    throw DbgException( "Unknown processor type" );
}

/////////////////////////////////////////////////////////////////////////////

TypedValueList TypedVarFunction::castArgs(const TypedValueList& arglst)
{
    if ( arglst.size() != m_typeInfo->getElementCount() )
        throw TypeException(L"wrong  argument count");

    TypedValueList  castedArgs;

    for ( size_t i = 0; i < m_typeInfo->getElementCount(); ++i )
    {
         TypeInfoPtr  argType = m_typeInfo->getElement(i);

         if ( argType->isBase() )
         {
             if ( argType->getName() == L"Float" )
                 throw TypeException(L"unsupported argument type");

             if ( argType->getName() == L"Double" )
                 throw TypeException(L"unsupported argument type");

            castedArgs.push_back( castBaseArg(argType, arglst[i]) );

            continue;
         }

         if ( argType->isPointer() || argType->isArray() )
         {
             castedArgs.push_back( castPtrArg(argType, arglst[i]) );

             continue;
         }

         if ( argType->isUserDefined() )
         {
             castedArgs.push_back( castUdt(argType, arglst[i]) );

             continue;
         }

         throw TypeException(L"unsupported argument type");
    }

    return castedArgs;
}

/////////////////////////////////////////////////////////////////////////////

TypedValue TypedVarFunction::callCdecl(const TypedValueList& arglst)
{
    CPUContextAutoRestore  cpuContext;

    TypedValueList  args = castArgs(arglst);

    TypeInfoPtr  retType =  m_typeInfo->getReturnType();

    MEMOFFSET_64   retOffset = ( !retType->isVoid() && retType->getSize() > 8 ) ? stackAlloc(retType->getSize()) : 0UL;

    for ( TypedValueList::const_reverse_iterator  it = args.rbegin(); it != args.rend(); ++it)
    {
        size_t  argSize = it->getSize();
        it->writeBytes( getMemoryAccessor( stackAlloc( argSize ), argSize ) );
    }

    if ( retOffset > 0 )
    {
        if ( retType->getPtrSize() == 4 )
        {
            pushInStack( static_cast<unsigned long>(retOffset) );
        }
        else
        {
            pushInStack( retOffset );
        }
    }

    makeCallx86();

    if ( retOffset > 0 )
    {
        std::vector<char>  buffer(retType->getSize());
        DataAccessorPtr  stackRange = getMemoryAccessor(retOffset, retType->getSize());
        stackRange->readSignBytes(buffer, retType->getSize() );
        return loadTypedVar( retType, getCacheAccessor(buffer) );
    }
    else
    {
        if (retType->isBase() )
        {
            if ( retType->getName() == L"Float" || retType->getName() == L"Double" )
            {
                NOT_IMPLEMENTED();
            }

            return castBaseArg( retType, getReturnReg() );
        }
        else if ( retType->isPointer() )
        {
            return static_cast<unsigned long>(getReturnReg());
        }
        else if ( retType->isVoid() )
        {
            return TypedVarPtr( new TypedVarVoid() );
        }
        else if ( retType->isUserDefined() )
        {
            if ( retType->getSize() <= 4 )
            {
                return TypedValue( TypedValue( static_cast<unsigned long>(getReturnReg() ) ).castTo(retType) );
            }

            if ( retType->getSize() <= 8 )
            {
                return TypedValue( TypedValue(getReturnReg()).castTo(retType) );
            }
        }
    }

    throw TypeException(L"unsupported return type");
}

///////////////////////////////////////////////////////////////////////////////

TypedValue TypedVarFunction::callStd(const TypedValueList& args)
{
    return callCdecl(args);
}

///////////////////////////////////////////////////////////////////////////////

TypedValue TypedVarFunction::callFast(const TypedValueList& arglst)
{
    CPUContextAutoRestore  cpuContext;

    TypedValueList  args = castArgs(arglst);

    TypeInfoPtr  retType =  m_typeInfo->getReturnType();

    MEMOFFSET_64   retOffset = ( !retType->isVoid() && retType->getSize() > 8 ) ? stackAlloc(retType->getSize()) : 0UL;

    if ( retOffset > 0 )
    {
        args.insert(args.begin(), static_cast<unsigned long>(retOffset));
    }

    int i = args.size() - 1;
    for ( TypedValueList::reverse_iterator  it = args.rbegin(); it != args.rend(); ++it, --i)
    {
        size_t  argSize = it->getSize();

        switch(i)
        {
        case 0:
            if ( it->getSize() <= 4 )
                it->writeBytes(getRegisterAccessor(L"edx"));
            break;

        case 1:
            if ( it->getSize() <= 4 )
                it->writeBytes(getRegisterAccessor(L"ecx"));
            break;
        }

        it->writeBytes( getMemoryAccessor( stackAlloc( argSize ), argSize ) );
    }

    makeCallx86();

    if ( retOffset > 0 )
    {
        std::vector<char>  buffer(retType->getSize());
        DataAccessorPtr  stackRange = getMemoryAccessor(retOffset, retType->getSize());
        stackRange->readSignBytes(buffer, retType->getSize() );
        return loadTypedVar( retType, getCacheAccessor(buffer) );
    }
    else
    {
        if (retType->isBase() )
        {
            if ( retType->getName() == L"Float" || retType->getName() == L"Double" )
            {
                NOT_IMPLEMENTED();
            }

            return castBaseArg( retType, getReturnReg() );
        }
        else if ( retType->isPointer() )
        {
            return getReturnReg();
        }
        else if ( retType->isVoid() )
        {
            return TypedVarPtr( new TypedVarVoid() );
        }
        else if ( retType->isUserDefined() )
        {
            if ( retType->getSize() <= 4 )
            {
                return TypedValue( TypedValue( static_cast<unsigned long>(getReturnReg() ) ).castTo(retType) );
            }

            if ( retType->getSize() <= 8 )
            {
                return TypedValue( TypedValue(getReturnReg()).castTo(retType) );
            }
        }
    }

    throw TypeException(L"unsupported return type");
}

///////////////////////////////////////////////////////////////////////////////

TypedValue TypedVarFunction::callThis(const TypedValueList& arglst)
{
    CPUContextAutoRestore  cpuContext;

    TypedValueList  args = castArgs(arglst);

    TypeInfoPtr  retType =  m_typeInfo->getReturnType();

    MEMOFFSET_64   retOffset = ( !retType->isVoid() &&  retType->getSize() > 8 ) ? stackAlloc(retType->getSize()) : 0UL;

    for ( TypedValueList::const_reverse_iterator  it = args.rbegin(); it != args.rend(); ++it)
    {
        size_t  argSize = it->getSize();

        if ( it + 1 == args.rend() )
            it->writeBytes(getRegisterAccessor(L"ecx"));
        else
            it->writeBytes( getMemoryAccessor( stackAlloc( argSize ), argSize ) );
    }

    if ( retOffset > 0 )
    {
        if ( retType->getPtrSize() == 4 )
        {
            pushInStack( static_cast<unsigned long>(retOffset) );
        }
        else
        {
            pushInStack( retOffset );
        }
    }
   
    makeCallx86();

    if ( retOffset > 0 )
    {
        std::vector<char>  buffer(retType->getSize());
        DataAccessorPtr  stackRange = getMemoryAccessor(retOffset, retType->getSize());
        stackRange->readSignBytes(buffer, retType->getSize() );
        return loadTypedVar( retType, getCacheAccessor(buffer) );
    }
    else
    {
        if (retType->isBase() )
        {
            if ( retType->getName() == L"Float" || retType->getName() == L"Double" )
            {
                NOT_IMPLEMENTED();
            }

            return castBaseArg( retType, getReturnReg() );
        }
        else if ( retType->isPointer() )
        {
            return static_cast<unsigned long>(getReturnReg());
        }
        else if ( retType->isVoid() )
        {
            return TypedVarPtr( new TypedVarVoid() );
        }
        else if ( retType->isUserDefined() )
        {
            if ( retType->getSize() <= 4 )
            {
                return TypedValue( TypedValue( static_cast<unsigned long>(getReturnReg() ) ).castTo(retType) );
            }

            if ( retType->getSize() <= 8 )
            {
                return TypedValue( TypedValue(getReturnReg()).castTo(retType) );
            }
        }
    }


    throw TypeException(L"unsupported return type");
}

///////////////////////////////////////////////////////////////////////////////

TypedValue TypedVarFunction::callX64(const TypedValueList& arglst)
{
    CPUContextAutoRestore  cpuContext;

    TypedValueList  args = castArgs(arglst);

    TypeInfoPtr  retType =  m_typeInfo->getReturnType();

    int i = args.size() - 1;
    for( TypedValueList::reverse_iterator  it = args.rbegin(); it != args.rend(); ++it, --i)
    {
        if ( i < 4 && it->getSize() > 8 )
        {
            size_t  argSize = it->getSize();
            MEMOFFSET_64  stackOffset = stackAlloc( argSize );
            it->writeBytes( getMemoryAccessor( stackOffset, argSize ) );
            *it = TypedValue(stackOffset);
        }
    }

    MEMOFFSET_64   retOffset = ( !retType->isVoid() && retType->getSize() > 8 ) ? stackAlloc(retType->getSize()) : 0UL;

    if ( retOffset > 0 )
    {
        args.insert(args.begin(), retOffset );
    }

    i = args.size() - 1;
    for ( TypedValueList::reverse_iterator  it = args.rbegin(); it != args.rend(); ++it, --i)
    {
        size_t  argSize = it->getSize();

        switch(i)
        {
        case 0:
            it->writeBytes(getRegisterAccessor(L"rcx"));
            break;

        case 1:
            it->writeBytes(getRegisterAccessor(L"rdx"));
            break;

        case 2:
            it->writeBytes(getRegisterAccessor(L"r8"));
            break;

        case 3:
            it->writeBytes(getRegisterAccessor(L"r9"));
            break;

        default:
            it->writeBytes( getMemoryAccessor( stackAlloc( argSize ), argSize ) );
        }
    }

    stackAlloc(4*8);

    makeCallx64();

    if ( retOffset > 0 )
    {
        std::vector<char>  buffer(retType->getSize());
        DataAccessorPtr  stackRange = getMemoryAccessor(retOffset, retType->getSize());
        stackRange->readSignBytes(buffer, retType->getSize() );
        return loadTypedVar( retType, getCacheAccessor(buffer) );
    }
    else
    {
        if (retType->isBase() )
        {
            if ( retType->getName() == L"Float" || retType->getName() == L"Double" )
            {
                NOT_IMPLEMENTED();
            }

            return castBaseArg( retType, getReturnReg() );
        }
        else if ( retType->isPointer() )
        {
            return getReturnReg();
        }
        else if ( retType->isVoid() )
        {
            return TypedVarPtr( new TypedVarVoid() );
        }
        else if ( retType->isUserDefined() )
        {
            if ( retType->getSize() <= 4 )
            {
                return TypedValue( TypedValue( static_cast<unsigned long>(getReturnReg() ) ).castTo(retType) );
            }

            if ( retType->getSize() <= 8 )
            {
                return TypedValue( TypedValue(getReturnReg()).castTo(retType) );
            }
        }
    }

    throw TypeException(L"unsupported return type");
}

///////////////////////////////////////////////////////////////////////////////

void TypedVarFunction::makeCallx86()
{
    MEMOFFSET_64   ip = getInstructionOffset();

    ScopedBreakpoint  bp = softwareBreakPointSet(ip);

    pushInStack( static_cast<unsigned long>(ip));

    setInstructionOffset( getAddress() );

    while( getInstructionOffset() != ip )
    {
        targetGo();

        if ( getLastEventType() == EventTypeException )
        {
            ExceptionInfo  excInfo = getLastException();

            if ( !excInfo.firstChance )
            {

                std::wstringstream sstr;
                sstr << L"Exception occured during call function" << std::endl;
                sstr << L"   Exception address: 0x" << std::hex << excInfo.exceptionAddress << std::endl;
                sstr << L"   Exception code: 0x" << std::hex << excInfo.exceptionCode << std::endl;
                sstr << L"   Exception record: 0x" << std::hex << excInfo.exceptionRecord << std::endl;

                throw CallException( sstr.str() );
            }
        }
    }
}

///////////////////////////////////////////////////////////////////////////////

void TypedVarFunction::makeCallx64()
{
    StackPtr  stk = getStack();
    StackFramePtr  lastFrame = stk->getFrame(0);
    MEMOFFSET_64   ip = lastFrame->getIP();

    ScopedBreakpoint  bp = softwareBreakPointSet(ip);

    pushInStack(ip);

    setInstructionOffset( getAddress() );

    while( getInstructionOffset() != ip )
    {
        targetGo();

        if ( getLastEventType() == EventTypeException )
        {
            ExceptionInfo  excInfo = getLastException();

            if ( !excInfo.firstChance )
            {

                std::wstringstream sstr;
                sstr << L"Exception occured during call function" << std::endl;
                sstr << L"   Exception address: 0x" << std::hex << excInfo.exceptionAddress << std::endl;
                sstr << L"   Exception code: 0x" << std::hex << excInfo.exceptionCode << std::endl;
                sstr << L"   Exception record: 0x" << std::hex << excInfo.exceptionRecord << std::endl;

                throw CallException( sstr.str() );
            }
        }
    }
}

///////////////////////////////////////////////////////////////////////////////

SymbolFunction::SymbolFunction( SymbolPtr& symbol ) :
    TypedVarFunction( loadType( symbol ), getMemoryAccessor( symbol->getVa(), 0 ), ::getSymbolName(symbol) ),
    m_symbol( symbol )
{
}

///////////////////////////////////////////////////////////////////////////////

SymbolPtr SymbolFunction::getChildSymbol(size_t index)
{
    SymbolPtrList  paramLst = m_symbol->findChildren(SymTagData);
    if (paramLst.size() < index)
        throw IndexException(index);

    SymbolPtrList::iterator itVar = paramLst.begin();
    for (size_t i = 0; itVar != paramLst.end(); ++itVar)
    {
        unsigned long  dataKind = (*itVar)->getDataKind();

        if (dataKind == DataIsParam || dataKind == DataIsObjectPtr)
        {
            if (i == index)
            {
                return *itVar;
            }

            i++;
        }
    }

    throw IndexException(index);
}

///////////////////////////////////////////////////////////////////////////////

MEMOFFSET_REL SymbolFunction::getElementOffset( size_t index )
{
    SymbolPtr  paramSym = getChildSymbol(index);

    return paramSym->getOffset();
}

///////////////////////////////////////////////////////////////////////////////

RELREG_ID SymbolFunction::getElementOffsetRelativeReg(size_t index )
{
    SymbolPtr  paramSym = getChildSymbol(index);

    return paramSym->getRegRealativeId();
}

///////////////////////////////////////////////////////////////////////////////

VarStorage SymbolFunction::getElementStorage(const std::wstring& fieldName)
{
    SymbolPtr  paramSym = m_symbol->getChildByName(fieldName);
    if (paramSym->getLocType() == LocIsEnregistered)
        return RegisterVar;
    return MemoryVar;
}

///////////////////////////////////////////////////////////////////////////////

VarStorage SymbolFunction::getElementStorage(size_t index)
{
    SymbolPtr  paramSym = getChildSymbol(index);
    if (paramSym->getLocType() == LocIsEnregistered)
        return RegisterVar;
    return MemoryVar;
}

///////////////////////////////////////////////////////////////////////////////

MEMOFFSET_REL SymbolFunction::getElementOffset( const std::wstring& paramName )
{
    SymbolPtr  paramSym = m_symbol->getChildByName(paramName);

    return paramSym->getOffset();
}
 
///////////////////////////////////////////////////////////////////////////////

RELREG_ID SymbolFunction::getElementOffsetRelativeReg(const std::wstring& paramName )
{
    SymbolPtr  paramSym = m_symbol->getChildByName(paramName);

    return paramSym->getRegRealativeId();
}

///////////////////////////////////////////////////////////////////////////////

size_t SymbolFunction::getElementIndex(const std::wstring& paramName )
{
    SymbolPtrList  paramLst = m_symbol->findChildren( SymTagData );

    SymbolPtrList::iterator itVar = paramLst.begin();

    for ( size_t i = 0; itVar != paramLst.end(); ++itVar )
    {
        unsigned long  dataKind = (*itVar)->getDataKind();

        if (dataKind == DataIsParam || dataKind == DataIsObjectPtr)
        {
            if (  (*itVar)->getName() == paramName )
                return i;
            i++;
        }
    }

    throw DbgException("parameter is not found");
}

///////////////////////////////////////////////////////////////////////////////

std::wstring SymbolFunction::getElementName( size_t index )
{
    SymbolPtr  paramSym = getChildSymbol(index);
    return  paramSym->getName();
}

///////////////////////////////////////////////////////////////////////////////

MEMOFFSET_64 SymbolFunction::getDebugXImpl(SymTags symTag) const
{
    SymbolPtrList resLst = m_symbol->findChildren(symTag);
    switch (resLst.size())
    {
    case 0:
        throw DbgException("child symbol is not found");
    case 1:
        return (*resLst.begin())->getVa();
    }
    throw DbgException("unexpected count of child symbols");
}

///////////////////////////////////////////////////////////////////////////////

unsigned long SymbolFunction::getElementReg(const std::wstring& fieldName)
{
    SymbolPtr  paramSym = m_symbol->getChildByName(fieldName);

    return paramSym->getRegRealativeId();
}

///////////////////////////////////////////////////////////////////////////////

unsigned long SymbolFunction::getElementReg(size_t index)
{
    SymbolPtr  paramSym = getChildSymbol(index);
    return paramSym->getRegisterId();
}

///////////////////////////////////////////////////////////////////////////////

TypedVarPtr TypedVarVtbl::getElement(size_t index)
{
    if (index >= m_typeInfo->getElementCount())
        throw IndexException(index);

    TypeInfoPtr  elementType = loadType(L"Void*");

    return loadTypedVar(elementType, m_varData->getAddress() + elementType->getSize()*index);
}

///////////////////////////////////////////////////////////////////////////////

std::wstring TypedVarVtbl::str()
{
    std::wstringstream   sstr;

    sstr << m_typeInfo->getName() << L" at 0x" << std::hex << m_varData->getAddress() << std::endl;

    TypeInfoPtr  elementType = loadType(L"Void*");
    size_t  elementSize = elementType->getSize();
    MEMOFFSET_64  beginOffset = m_varData->getAddress();

    for (size_t i = 0; i < m_typeInfo->getElementCount(); ++i)
    {
        MEMOFFSET_64  val = ptrPtr(beginOffset + elementSize*i, elementSize);
        sstr << "   [" << std::dec << i << "] 0x" << std::hex << val << " (" << findSymbol(val) << ')' << std::endl;
    }

    return sstr.str();
}

///////////////////////////////////////////////////////////////////////////////

TypeInfoPtr TypedVarVoid::getType() const
{
    return TypeInfoPtr( new TypeInfoVoid() );
}

///////////////////////////////////////////////////////////////////////////////

TypedVarMethodBound::TypedVarMethodBound( 
    const TypeInfoPtr& typeInfo,
    const DataAccessorPtr &dataSource, 
    const std::wstring& name, 
    MEMOFFSET_64 m_thisvalue) :
        TypedVarFunction( typeInfo, dataSource, name),
        m_this( m_thisvalue )
{
}

///////////////////////////////////////////////////////////////////////////////

TypedValue TypedVarMethodBound::call(const TypedValueList& arglst)
{
    TypedValueList  argListWithThis = arglst;
    argListWithThis.insert( argListWithThis.begin(), m_this );
    return TypedVarFunction::call(argListWithThis);
}

///////////////////////////////////////////////////////////////////////////////

TypedValue::TypedValue()
{
    m_value = TypedVarPtr( new TypedVarVoid() );
}

///////////////////////////////////////////////////////////////////////////////

TypedValue::TypedValue(const NumVariant& var)
{
    if ( var.isChar() )
    {
        m_value = loadCharVar(var.asChar());
    }
    else if ( var.isShort() )
    {
        m_value = loadShortVar(var.asShort());
    }
    else if ( var.isLong() )
    {
        m_value = loadLongVar(var.asLong());
    }
    else if ( var.isLongLong() )
    {
        m_value = loadLongLongVar(var.asLongLong());
    }
    else if ( var.isUChar() )
    {
        m_value = loadUCharVar(var.asUChar() );
    }
    else if ( var.isUShort() )
    {
        m_value = loadUShortVar(var.asUShort());
    }
    else if ( var.isULong() )
    {
        m_value = loadULongVar(var.asULong() );
    }
    else if ( var.isULongLong() )
    {
        m_value = loadULongLongVar(var.asULongLong() );
    }
    else if ( var.isInt() )
    {
        m_value = loadIntVar(var.asInt() );
    }
    else if ( var.isUInt() )
    {
        m_value = loadUIntVar(var.asUInt() );
    }
    else if ( var.isFloat() )
    {
        m_value = loadFloatVar(var.asFloat() );
    }
    else if ( var.isDouble() )
    {
        m_value = loadDoubleVar(var.asDouble() );
    }
    else
    {
        throw TypeException(L"Failed to convet NumVariant to TypedVar");
    }
}

///////////////////////////////////////////////////////////////////////////////

TypedValue callRaw(MEMOFFSET_64 addr, CallingConventionType callConv, const TypedValueList& arglst)
{
    TypeInfoPtr funcType = defineFunction( loadType(L"UInt8B"), callConv);
    for ( TypedValueList::const_iterator it = arglst.begin(); it != arglst.end(); ++it)
        funcType->appendField(L"arg", it->getType() );

    return loadTypedVar(funcType, addr)->call(arglst);
}

///////////////////////////////////////////////////////////////////////////////


} // end kdlib namesapce
