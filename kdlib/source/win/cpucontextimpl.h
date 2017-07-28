#pragma once

#include <map>

#include "kdlib/cpucontext.h"

#include "threadctx.h"

namespace kdlib {

///////////////////////////////////////////////////////////////////////////////

class CPUContextImpl : public CPUContext
{

public:

    CPUContextImpl();

protected:

    virtual CPUType getCPUType() {
        return m_cpuType;
    }

    virtual CPUType getCPUMode() {
        return m_cpuMode;
    }

    virtual NumVariant getRegisterByName( const std::wstring &name);

    virtual void setRegisterByName( const std::wstring &name, const NumVariant& value) {
        NOT_IMPLEMENTED();
    }

    virtual NumVariant getRegisterByIndex( unsigned long index );

    virtual void setRegisterByIndex( unsigned long index, const NumVariant& value) {
        NOT_IMPLEMENTED();
    }

    virtual std::wstring getRegisterName( unsigned long index );

    virtual unsigned long getRegisterNumber() {
        return static_cast<unsigned long>(m_indexedValues.size());
    }

    virtual void restore();

    virtual MEMOFFSET_64 getIP();

    virtual void setIP(MEMOFFSET_64 ip);

    virtual MEMOFFSET_64 getSP();

    virtual void setSP(MEMOFFSET_64 sp);

    virtual MEMOFFSET_64 getFP();

    virtual void setFP(MEMOFFSET_64 fp);

protected:

    CPUType  m_cpuType;
    CPUType  m_cpuMode;

    std::map<unsigned long, NumVariant> m_indexedValues;
    std::map<std::wstring, unsigned long>  m_indexedNames;
};

class CPUContextAmd64 : public CPUContext
{
public:

    CPUContextAmd64();

    CPUContextAmd64(const CONTEXT_X64& ctx) :
        m_context(ctx)
    {}

    virtual CPUType getCPUType() {
        return CPU_AMD64;
    }

    virtual CPUType getCPUMode() {
        return CPU_AMD64;
    }

    virtual void restore();

    virtual NumVariant getRegisterByName(const std::wstring &name) {
        NOT_IMPLEMENTED();
    }

    virtual void setRegisterByName( const std::wstring &name, const NumVariant& value) {
        NOT_IMPLEMENTED();
    }

    virtual NumVariant getRegisterByIndex(unsigned long index); 

    virtual void setRegisterByIndex( unsigned long index, const NumVariant& value) {
        NOT_IMPLEMENTED();
    }


    virtual std::wstring getRegisterName(unsigned long index);

    virtual unsigned long getRegisterNumber() {
        NOT_IMPLEMENTED();
    }

    virtual MEMOFFSET_64 getIP() {
        NOT_IMPLEMENTED();
    }

    virtual void setIP(MEMOFFSET_64 ip) {
        NOT_IMPLEMENTED();
    }

    virtual MEMOFFSET_64 getSP() {
        NOT_IMPLEMENTED();
    }

    virtual void setSP(MEMOFFSET_64 sp) {
        NOT_IMPLEMENTED();
    }

    virtual MEMOFFSET_64 getFP() {
        NOT_IMPLEMENTED();
    }

    virtual void setFP(MEMOFFSET_64 fp) {
        NOT_IMPLEMENTED();
    }

private:

    CONTEXT_X64  m_context;
};

///////////////////////////////////////////////////////////////////////////////

class CPUContextI386: public CPUContext
{
public:

    CPUContextI386();

    CPUContextI386(const CONTEXT_X86& ctx) :
        m_context(ctx)
    {}


    virtual CPUType getCPUType() {
        return CPU_I386;
    }

    virtual CPUType getCPUMode() {
        return CPU_I386;
    }

    virtual void restore();

    virtual NumVariant getRegisterByName(const std::wstring &name) {
        NOT_IMPLEMENTED();
    }

    virtual void setRegisterByName( const std::wstring &name, const NumVariant& value) {
        NOT_IMPLEMENTED();
    }

    virtual NumVariant getRegisterByIndex(unsigned long index);

    virtual void setRegisterByIndex( unsigned long index, const NumVariant& value) {
        NOT_IMPLEMENTED();
    }

    virtual std::wstring getRegisterName(unsigned long index);

    virtual unsigned long getRegisterNumber() {
        NOT_IMPLEMENTED();
    }

    virtual MEMOFFSET_64 getIP() {
        NOT_IMPLEMENTED();
    }

    virtual void setIP(MEMOFFSET_64 ip) {
        NOT_IMPLEMENTED();
    }

    virtual MEMOFFSET_64 getSP() {
        NOT_IMPLEMENTED();
    }

    virtual void setSP(MEMOFFSET_64 sp) {
        NOT_IMPLEMENTED();
    }

    virtual MEMOFFSET_64 getFP() {
        NOT_IMPLEMENTED();
    }

    virtual void setFP(MEMOFFSET_64 fp) {
        NOT_IMPLEMENTED();
    }

private:

    CONTEXT_X86  m_context;
};

///////////////////////////////////////////////////////////////////////////////

class CPUContextWOW64 : public CPUContext
{
public:

    CPUContextWOW64(const WOW64_CONTEXT& ctx) :
        m_context(ctx)
    {}


    virtual CPUType getCPUType() {
        return CPU_AMD64;
    }

    virtual CPUType getCPUMode() {
        return CPU_I386;
    }

    virtual void restore() {
        NOT_IMPLEMENTED();
    }


    virtual NumVariant getRegisterByName(const std::wstring &name) {
        NOT_IMPLEMENTED();
    }

    virtual void setRegisterByName( const std::wstring &name, const NumVariant& value) {
        NOT_IMPLEMENTED();
    }

    virtual NumVariant getRegisterByIndex(unsigned long index);

    virtual void setRegisterByIndex( unsigned long index, const NumVariant& value) {
        NOT_IMPLEMENTED();
    }

    virtual std::wstring getRegisterName(unsigned long index);

    virtual unsigned long getRegisterNumber() {
        NOT_IMPLEMENTED();
    }

    virtual MEMOFFSET_64 getIP() {
        NOT_IMPLEMENTED();
    }

    virtual void setIP(MEMOFFSET_64 ip) {
        NOT_IMPLEMENTED();
    }

    virtual MEMOFFSET_64 getSP() {
        NOT_IMPLEMENTED();
    }

    virtual void setSP(MEMOFFSET_64 sp) {
        NOT_IMPLEMENTED();
    }

    virtual MEMOFFSET_64 getFP() {
        NOT_IMPLEMENTED();
    }

    virtual void setFP(MEMOFFSET_64 fp) {
        NOT_IMPLEMENTED();
    }

private:

    WOW64_CONTEXT  m_context;
};

///////////////////////////////////////////////////////////////////////////////


}
