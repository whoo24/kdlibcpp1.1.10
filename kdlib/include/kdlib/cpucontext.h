#pragma once

#include <boost/shared_ptr.hpp>
#include <boost/noncopyable.hpp>

#include "kdlib/dbgtypedef.h"
#include "kdlib/variant.h"

namespace kdlib {

///////////////////////////////////////////////////////////////////////////////

class CPUContext;
typedef boost::shared_ptr<CPUContext>  CPUContextPtr;

class  CPUContext 
{
public:

    virtual CPUType getCPUType() = 0;
    virtual CPUType getCPUMode() = 0;

    virtual NumVariant getRegisterByName( const std::wstring &name) = 0;
    virtual void setRegisterByName( const std::wstring &name, const NumVariant& value) = 0;

    virtual NumVariant getRegisterByIndex( unsigned long index ) = 0;
    virtual void setRegisterByIndex( unsigned long index, const NumVariant& value) = 0;

    virtual std::wstring getRegisterName( unsigned long index ) = 0;
    virtual unsigned long getRegisterNumber() = 0;

    virtual MEMOFFSET_64 getIP() = 0;
    virtual void setIP(MEMOFFSET_64 ip) = 0;

    virtual MEMOFFSET_64 getSP() = 0;
    virtual void setSP(MEMOFFSET_64 sp) = 0;

    virtual MEMOFFSET_64 getFP() = 0;
    virtual void setFP(MEMOFFSET_64 fp) = 0;

    virtual void restore() = 0;
};

CPUContextPtr loadCPUContext();

///////////////////////////////////////////////////////////////////////////////

class CPUContextAutoRestore
{
public:

    CPUContextAutoRestore() :
        m_context(loadCPUContext())
    {}

    ~CPUContextAutoRestore()
    {
        m_context->restore();
    }


private:

    CPUContextPtr   m_context;
};

///////////////////////////////////////////////////////////////////////////////

NumVariant getRegisterByName(const std::wstring& regName);
NumVariant getRegisterByIndex(unsigned long regIndex);

void setRegisterByName(const std::wstring& regName, const NumVariant& value);
void setRegisterByIndex(unsigned long regIndex, const NumVariant& value);

void pushInStack(const NumVariant& value);
void popFromStack(NumVariant& value);

MEMOFFSET_64 stackAlloc(unsigned short byteCount);
MEMOFFSET_64 stackFree(unsigned short byteCount);

///////////////////////////////////////////////////////////////////////////////


} // kdlib namespace end
