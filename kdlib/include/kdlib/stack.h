#pragma once

#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/noncopyable.hpp>

#include <kdlib/dbgengine.h>
#include <kdlib/typedvar.h>
#include <kdlib/exceptions.h>
#include <kdlib/cpucontext.h>

namespace kdlib {

///////////////////////////////////////////////////////////////////////////////

class StackFrame;
typedef boost::shared_ptr<StackFrame>  StackFramePtr;

class StackFrame
{
public:

    virtual unsigned long getNumber() const = 0;

    virtual MEMOFFSET_64 getIP() const = 0;
    virtual MEMOFFSET_64 getRET() const = 0;
    virtual MEMOFFSET_64  getFP() const = 0;
    virtual MEMOFFSET_64  getSP() const = 0;

    virtual CPUContextPtr getCPUContext() = 0;

    virtual unsigned long getTypedParamCount() = 0;
    virtual TypedVarPtr getTypedParam(unsigned long index) = 0;
    virtual std::wstring getTypedParamName(unsigned long index) = 0;
    virtual TypedVarPtr getTypedParam(const std::wstring& paramName) = 0;
    virtual bool findParam(const std::wstring& paramName) = 0;

    virtual unsigned long getLocalVarCount() = 0;
    virtual TypedVarPtr getLocalVar(unsigned long index) = 0;
    virtual TypedVarPtr getLocalVar(const std::wstring& varName) = 0;
    virtual std::wstring  getLocalVarName(unsigned long index) = 0;
    virtual bool findLocalVar(const std::wstring& varName) = 0;

    virtual unsigned long getStaticVarCount() = 0;
    virtual TypedVarPtr getStaticVar(unsigned long index) = 0;
    virtual TypedVarPtr getStaticVar(const std::wstring& paramName) = 0;
    virtual std::wstring  getStaticVarName(unsigned long index) = 0;
    virtual bool findStaticVar(const std::wstring& varName) = 0;
};

///////////////////////////////////////////////////////////////////////////////

class Stack;
typedef boost::shared_ptr<Stack>  StackPtr;

class Stack {

public:
    
    virtual unsigned long getFrameCount() = 0;

    virtual StackFramePtr getFrame(unsigned long index) = 0;

};

///////////////////////////////////////////////////////////////////////////////

StackPtr getStack();
StackFramePtr getCurrentStackFrame();
unsigned long getCurrentStackFrameNumber();
void setCurrentStackFrame(StackFramePtr& stackFrame);
void setCurrentStackFrameByIndex(unsigned long frameIndex);
void resetCurrentStackFrame();

///////////////////////////////////////////////////////////////////////////////

} // kdlib namespace end

