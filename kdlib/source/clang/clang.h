#pragma once

#include <boost/shared_ptr.hpp>

#include <clang/Frontend/ASTUnit.h>
#include <clang/AST/Type.h>


#include "typeinfoimp.h"


namespace kdlib {


class ClangASTSession;
typedef boost::shared_ptr<ClangASTSession>  ClangASTSessionPtr;



class ClangASTSession : public boost::enable_shared_from_this<ClangASTSession>
{
public:

    static ClangASTSessionPtr getASTSession(std::unique_ptr<clang::ASTUnit>&  astUnit) {
        return ClangASTSessionPtr( new ClangASTSession(astUnit) );
    }

    TypeInfoPtr getTypeInfo(const std::wstring& name);

    clang::ASTContext&  getASTContext() {
          return m_astUnit->getASTContext();
    }

    size_t getPtrSize() ;


private:

    ClangASTSession(std::unique_ptr<clang::ASTUnit>& astUnit) 
    {
        astUnit.swap(m_astUnit);
    }

    std::unique_ptr<clang::ASTUnit>  m_astUnit;

};


class TypeFieldClangField : public TypeField
{
public:

    static TypeFieldPtr getField(ClangASTSessionPtr& astSession, clang::RecordDecl* structDecl, clang::FieldDecl* fieldDecl, MEMOFFSET_32 startOffset);
    static TypeFieldPtr getStaticField(ClangASTSessionPtr& astSession, clang::RecordDecl* structDecl, clang::VarDecl* varDecl);

private:

    TypeFieldClangField(const std::wstring  &name) :
        TypeField(name)
        {}

    virtual TypeInfoPtr getTypeInfo();

    ClangASTSessionPtr  m_astSession;
    clang::DeclaratorDecl*  m_fieldDecl;
    clang::RecordDecl*  m_recordDecl;
};


class TypeInfoClangStruct : public TypeInfoFields
{
public:

    TypeInfoClangStruct(const std::wstring & name, ClangASTSessionPtr& session, clang::RecordDecl*  decl);

protected:

    virtual bool isUserDefined() {
        return true;
    }

    virtual std::wstring str() {
        TypeInfoPtr  selfPtr = shared_from_this();
        return printStructType(selfPtr);
    }

    virtual size_t getSize();

protected:

    virtual void getFields();

    void getRecursiveFields( clang::RecordDecl* recordDecl, MEMOFFSET_32 startOffset);

    ClangASTSessionPtr  m_astSession;

    clang::RecordDecl*  m_decl;
};

class TypeInfoClangStructNoDef : public TypeInfoImp
{
public:

    TypeInfoClangStructNoDef(const std::wstring& name, ClangASTSessionPtr& session, clang::RecordDecl*  decl) :
        m_name(name),
        m_astSession(session),
        m_decl(decl)
        {}

protected:

    virtual bool isUserDefined() {
        return true;
    }

    virtual std::wstring getName() {
        return  m_name;
    }

    virtual std::wstring str();


    virtual TypeInfoPtr getElement( const std::wstring &name ) {
        throw TypeException(L"not complete declaration");
    }

    virtual TypeInfoPtr getElement( size_t index ) {
        throw TypeException(L"not complete declaration");
    }

    virtual MEMOFFSET_REL getElementOffset( const std::wstring &name ) {
        throw TypeException(L"not complete declaration");
    }

    virtual MEMOFFSET_REL getElementOffset( size_t index ) {
        throw TypeException(L"not complete declaration");
    }

    virtual std::wstring getElementName( size_t index ) {
        throw TypeException(L"not complete declaration");
    }

    virtual size_t getElementIndex( const std::wstring &name ) {
        throw TypeException(L"not complete declaration");
    }

    virtual size_t getElementCount() {
        throw TypeException(L"not complete declaration");
    }

    virtual  MEMOFFSET_64 getElementVa( const std::wstring &name ) {
        throw TypeException(L"not complete declaration");
    }

    virtual  MEMOFFSET_64 getElementVa( size_t index ) {
        throw TypeException(L"not complete declaration");
    }

    virtual bool isStaticMember( const std::wstring &name ) {
        throw TypeException(L"not complete declaration");
    }

    virtual bool isStaticMember( size_t index ) {
        throw TypeException(L"not complete declaration");
    }

    virtual bool isVirtualMember( const std::wstring &name ) {
        throw TypeException(L"not complete declaration");
    }

    virtual bool isVirtualMember( size_t index ) {
        throw TypeException(L"not complete declaration");
    }

protected:

    std::wstring  m_name;

    ClangASTSessionPtr  m_astSession;

    clang::RecordDecl*  m_decl;

};


class TypeInfoClangPointer : public TypeInfoPointer
{
public:

    TypeInfoClangPointer( ClangASTSessionPtr& session, const clang::PointerType* ptrType) :
        TypeInfoPointer( TypeInfoClangPointer::getDerefType(session, ptrType), session->getPtrSize() )
        {}

protected:

    static TypeInfoPtr  getDerefType( ClangASTSessionPtr& session, const clang::PointerType* refType);
};


class TypeInfoClangArray: public TypeInfoArray
{
public:
    TypeInfoClangArray(ClangASTSessionPtr& session, const clang::ArrayType* arrayType) :
        TypeInfoArray(TypeInfoClangArray::getDerefType(session, arrayType), getElementCount(arrayType))
        {}

protected:

    static TypeInfoPtr getDerefType( ClangASTSessionPtr& session, const clang::ArrayType* arrayType);

    size_t getElementCount(const clang::ArrayType* arrayType);

};

class TypeInfoClangRef : public TypeInfoImp
{
public:
    TypeInfoClangRef(ClangASTSessionPtr& session, const clang::ReferenceType* refType) :
        m_astSession(session),
        m_refType(refType)
        {}

protected:

    virtual TypeInfoPtr deref();

    virtual std::wstring getName() {
        return deref()->getName() + L"&";
    }

    virtual std::wstring str() {
        return L"ref to " + deref()->getName();
    }

protected:


    ClangASTSessionPtr  m_astSession;

    const clang::ReferenceType*  m_refType;
};

class TypeInfoClangFunc : public TypeInfoFunctionPrototype
{

public:

    TypeInfoClangFunc(ClangASTSessionPtr& session, const clang::FunctionProtoType* funcProto);

};



class TypeFieldClangEnumField : public TypeField
{
public:

    static TypeFieldPtr getField(ClangASTSessionPtr& astSession, clang::EnumConstantDecl*  EnumDecl);

private:

    TypeFieldClangEnumField(const std::wstring  &name) :
        TypeField(name)
        {}

    virtual TypeInfoPtr getTypeInfo();

    NumVariant getValue() const;

    clang::EnumConstantDecl*   m_decl;
  
    ClangASTSessionPtr  m_astSession;
};


class TypeInfoClangEnum : public TypeInfoFields
{
public:

     TypeInfoClangEnum(ClangASTSessionPtr& session, clang::EnumDecl* decl) :
        TypeInfoFields(strToWStr(decl->getNameAsString())),
        m_astSession(session),
        m_decl(decl)
     {}

protected:
    
    virtual bool isEnum() {
        return true;
    }

    virtual std::wstring str() {
        TypeInfoPtr  selfPtr = shared_from_this();
        return printEnumType(selfPtr);
    }

protected:

    virtual void getFields();

    ClangASTSessionPtr  m_astSession;

    clang::EnumDecl*  m_decl;
};


class TypeInfoProviderClang : public TypeInfoProvider
{
public:

    TypeInfoProviderClang( const std::wstring&  sourceCode, const std::wstring&  compileOptions);

private:

    virtual TypeInfoPtr getTypeByName(const std::wstring& name);

private:

    ClangASTSessionPtr  m_astSession;

};



}