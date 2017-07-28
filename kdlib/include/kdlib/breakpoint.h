#pragma once 

#include <boost/smart_ptr.hpp>
#include <boost/noncopyable.hpp>
#include <boost/enable_shared_from_this.hpp>

#include "kdlib/dbgtypedef.h"

namespace kdlib {

///////////////////////////////////////////////////////////////////////////////

#define BREAKPOINT_UNSET  (~0UL)

///////////////////////////////////////////////////////////////////////////////

class Breakpoint;
typedef boost::shared_ptr<Breakpoint>  BreakpointPtr;

///////////////////////////////////////////////////////////////////////////////


class BreakpointCallback {

public:

    virtual DebugCallbackResult onHit() = 0;
    virtual void onRemove() = 0;
};


class Breakpoint {

public:

    virtual BREAKPOINT_ID getId() const = 0;

    virtual MEMOFFSET_64 getOffset() const = 0;

    virtual BreakpointType getType() const = 0;

    virtual void set() = 0;

    virtual void remove() = 0;

    virtual BreakpointCallback* getCallback() = 0;
};

///////////////////////////////////////////////////////////////////////////////

BreakpointPtr softwareBreakPointSet(MEMOFFSET_64 offset, BreakpointCallback *callback =0);
BreakpointPtr hardwareBreakPointSet(MEMOFFSET_64 offset, size_t size, ACCESS_TYPE accessType, BreakpointCallback *callback =0);

unsigned long getNumberBreakpoints();
BreakpointPtr getBreakpointByIndex(unsigned long index);

///////////////////////////////////////////////////////////////////////////////

class ScopedBreakpoint {

public:

    ScopedBreakpoint() {
    }

    ScopedBreakpoint(BreakpointPtr& bp) {
        m_internalBp = bp;
    }

    ScopedBreakpoint& operator= (BreakpointPtr& bp) 
    {
        if ( m_internalBp )
             m_internalBp->remove();

        m_internalBp = bp;
        return *this;
    }

    ~ScopedBreakpoint() {
        if (m_internalBp)
            m_internalBp->remove();
    }

    BreakpointPtr operator->() {
        return m_internalBp;
    }

    const BreakpointPtr operator->() const {
        return m_internalBp;
    }


    BreakpointPtr get() {
        return m_internalBp;
    }

    void release() {
       m_internalBp->remove();
       m_internalBp = 0;
    }

private:

    ScopedBreakpoint(ScopedBreakpoint& bp) {
        throw;
    }

    ScopedBreakpoint& operator= (ScopedBreakpoint& bp) {
        throw;
    }

    BreakpointPtr  m_internalBp;
};



template<typename T>
class AutoBreakpoint : public T, public Breakpoint, public BreakpointCallback
{
public:

    virtual BREAKPOINT_ID getId() const {
        return m_internalBp->getId();
    }

    virtual MEMOFFSET_64 getOffset() const {
        return m_internalBp->getOffset();
    }

    virtual BreakpointType getType() const {
        return m_internalBp->getType();
    }

    virtual void set() {
        m_internalBp->set();
    }

    virtual void remove() {
        m_internalBp->remove();
    }

    virtual BreakpointCallback* getCallback() {
        return this;
    }

    virtual DebugCallbackResult onHit() {
        return T::onHit();
    }

    virtual void onRemove() {
        T::onRemove();
    }

public:

    AutoBreakpoint()
    {}

    explicit AutoBreakpoint(MEMOFFSET_64 offset) {
        m_internalBp = softwareBreakPointSet(offset,this);
    }

    AutoBreakpoint(MEMOFFSET_64 offset, size_t size, ACCESS_TYPE accessType) {
        m_internalBp = hardwareBreakPointSet(offset, size, accessType, this);
    }

    AutoBreakpoint(AutoBreakpoint& bp) {
        m_internalBp = bp.m_internalBp;
        bp.m_internalBp = 0;
    }

    AutoBreakpoint& operator= (AutoBreakpoint& bp) 
    {
        if ( m_internalBp )
             m_internalBp->remove();

        m_internalBp = bp.m_internalBp;
        bp.m_internalBp = 0;
        return *this;
    }

    virtual ~AutoBreakpoint() {
        if ( m_internalBp )
            m_internalBp->remove();
    }

    void release() {
        if ( m_internalBp )
        {
            m_internalBp->remove();
            m_internalBp = 0;
        }
    }

private:

    BreakpointPtr  m_internalBp;
};



///////////////////////////////////////////////////////////////////////////////


} // kdlib
