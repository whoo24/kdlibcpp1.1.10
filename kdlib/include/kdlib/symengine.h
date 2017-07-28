#pragma once

//#include "variant.h"

#include <list>

#include <boost/smart_ptr/shared_ptr.hpp>

#include "kdlib/dbgtypedef.h"
#include "kdlib/variant.h"

namespace kdlib {

////////////////////////////////////////////////////////////////////////////////

class Symbol;
typedef boost::shared_ptr< Symbol > SymbolPtr;
typedef std::list< SymbolPtr > SymbolPtrList;

class SymbolSession;
typedef boost::shared_ptr<SymbolSession> SymbolSessionPtr;

////////////////////////////////////////////////////////////////////////////////

enum SymTags
{
    SymTagNull,
    SymTagExe,
    SymTagCompiland,
    SymTagCompilandDetails,
    SymTagCompilandEnv,
    SymTagFunction,
    SymTagBlock,
    SymTagData,
    SymTagAnnotation,
    SymTagLabel,
    SymTagPublicSymbol,
    SymTagUDT,
    SymTagEnum,
    SymTagFunctionType,
    SymTagPointerType,
    SymTagArrayType,
    SymTagBaseType,
    SymTagTypedef,
    SymTagBaseClass,
    SymTagFriend,
    SymTagFunctionArgType,
    SymTagFuncDebugStart,
    SymTagFuncDebugEnd,
    SymTagUsingNamespace,
    SymTagVTableShape,
    SymTagVTable,
    SymTagCustom,
    SymTagThunk,
    SymTagCustomType,
    SymTagManagedType,
    SymTagDimension,
    SymTagMax
};

////////////////////////////////////////////////////////////////////////////////

enum LocationTypes
{
    LocIsNull,
    LocIsStatic,
    LocIsTLS,
    LocIsRegRel,
    LocIsThisRel,
    LocIsEnregistered,
    LocIsBitField,
    LocIsSlot,
    LocIsIlRel,
    LocInMetaData,
    LocIsConstant,
    LocTypeMax
};

////////////////////////////////////////////////////////////////////////////////

enum DataKinds
{
    DataIsUnknown,
    DataIsLocal,
    DataIsStaticLocal,
    DataIsParam,
    DataIsObjectPtr,
    DataIsFileStatic,
    DataIsGlobal,
    DataIsMember,
    DataIsStaticMember,
    DataIsConstant
};

////////////////////////////////////////////////////////////////////////////////

enum BasicTypes
{
    btNoType = 0,
    btVoid = 1,
    btChar = 2,
    btWChar = 3,
    btInt = 6,
    btUInt = 7,
    btFloat = 8,
    btBCD = 9,
    btBool = 10,
    btLong = 13,
    btULong = 14,
    btCurrency = 25,
    btDate = 26,
    btVariant = 27,
    btComplex = 28,
    btBit = 29,
    btBSTR = 30,
    btHresult = 31
};

////////////////////////////////////////////////////////////////////////////////

enum RegRealativeIds
{
    rriInstructionPointer,
    rriStackFrame,
    rriStackPointer
};

////////////////////////////////////////////////////////////////////////////////

enum MachineTypes {
    machine_I386 = 0x014c,
    machine_AMD64 = 0x8664
};

////////////////////////////////////////////////////////////////////////////////

enum CallingConventionType
{
    CallConv_NearC = 0x00,
    CallConv_FarC = 0x01,
    CallConv_NearPascal = 0x02,
    CallConv_FarPascal = 0x03,
    CallConv_NearFast = 0x04,
    CallConv_FarFast = 0x05,
    CallConv_Skipped = 0x06,
    CallConv_NearStd = 0x07,
    CallConv_FarStd = 0x08,
    CallConv_NearSys = 0x09,
    CallConv_FarSys = 0x0a,
    CallConv_ThisCall = 0x0b,
    CallConv_MipsCall = 0x0c,
    CallConv_Generic = 0x0d,
    CallConv_AlphaCall = 0x0e,
    CallConv_PpcCall = 0x0f,
    CallConv_ShCall = 0x10,
    CallConv_ArmCall = 0x11,
    CallConv_Am33Call = 0x12,
    CallConv_TriCall = 0x13,
    CallConv_Sh5Call = 0x14,
    CallConv_M32RCall = 0x15,
    CallConv_ClrCall = 0x16,
    CallConv_Inline = 0x17
};

////////////////////////////////////////////////////////////////////////////////

class Symbol {

public:

    virtual SymbolPtrList findChildren( unsigned long symTag, const std::wstring &name = L"", bool caseSensitive = false ) = 0;
    virtual SymbolPtrList findChildrenByRVA(unsigned long symTag, unsigned long rva) = 0;
    virtual unsigned long getBaseType() = 0;
    virtual BITOFFSET getBitPosition() = 0;
    virtual SymbolPtr getChildByIndex(unsigned long  index ) = 0;
    virtual SymbolPtr getChildByName(const std::wstring &name ) = 0;
    virtual size_t getChildCount() = 0;
    virtual size_t getChildCount(unsigned long symTag ) = 0;
    virtual size_t getCount() = 0;
    virtual unsigned long getDataKind() = 0;
    virtual SymbolPtr getIndexType() = 0;
    virtual unsigned long getLocType() = 0;
    virtual MachineTypes getMachineType() = 0;
    virtual std::wstring getName() = 0;
    virtual MEMOFFSET_REL getOffset() = 0;
    virtual unsigned long getRva() = 0;
    virtual size_t getSize() = 0;
    virtual SymTags getSymTag() = 0;
    virtual SymbolPtr getType() = 0;
    virtual unsigned long getUdtKind() = 0;
    virtual MEMOFFSET_64 getVa() = 0;
    virtual void getValue( NumVariant &vtValue ) = 0;
    virtual unsigned long getVirtualBaseDispIndex() = 0;
    virtual int getVirtualBasePointerOffset() = 0;
    virtual unsigned long getVirtualBaseDispSize() = 0;
    virtual bool isBasicType() = 0;
    virtual bool isConstant() = 0;
    virtual bool isIndirectVirtualBaseClass() = 0;
    virtual bool isVirtualBaseClass() = 0;
    virtual bool isVirtual() = 0;
    virtual unsigned long getRegisterId() = 0;
    virtual unsigned long getRegRealativeId() = 0;  // <- RegRealativeId
    virtual SymbolPtr getObjectPointerType() = 0; // "this" for method
    virtual unsigned long getCallingConvention() = 0; // <- CallingConventionType
    virtual SymbolPtr getClassParent() = 0; // "this" for method
    virtual SymbolPtr getVirtualTableShape() = 0;
    virtual unsigned long getVirtualBaseOffset() = 0;
};

///////////////////////////////////////////////////////////////////////////////

class SymbolSession {

public:

    virtual SymbolPtr getSymbolScope() = 0;

    virtual SymbolPtr findByRva( MEMOFFSET_32 rva, unsigned long symTag = SymTagNull, long* displacement = NULL ) = 0;

    virtual void getSourceLine( MEMOFFSET_64 offset, std::wstring &fileName, unsigned long &lineNo, long &displacement ) = 0;

    virtual std::wstring getSymbolFileName() = 0;
};

///////////////////////////////////////////////////////////////////////////////

std::wstring getBasicTypeName( unsigned long basicType );

SymbolSessionPtr loadSymbolFile(const std::wstring &filePath, MEMOFFSET_64 loadBase = 0);

SymbolSessionPtr loadSymbolFile(
    __in MEMOFFSET_64 loadBase,
    __in const std::wstring &executable,
    __in_opt std::wstring symbolSearchPath = std::wstring()
);

void setSymSrvDir(const std::wstring &symSrvDirectory);

SymbolSessionPtr loadSymbolFromExports(MEMOFFSET_64 loadBase); 

SymbolSessionPtr loadNoSymbolSession();

///////////////////////////////////////////////////////////////////////////////

}; // end kdlib namespace