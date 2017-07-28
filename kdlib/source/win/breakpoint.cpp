#include "stdafx.h"

#include <boost/enable_shared_from_this.hpp>

#include <kdlib/memaccess.h>

#include "win/dbgmgr.h"

#include "processmon.h"

namespace kdlib {


class BaseBreakpointImpl : public Breakpoint,  public boost::enable_shared_from_this<BaseBreakpointImpl>
{
public:

    BaseBreakpointImpl(MEMOFFSET_64 offset, BreakpointCallback *callback =0) :
        m_id(BREAKPOINT_UNSET),
        m_offset(addr64(offset)),
        m_callback(callback)
        {}

    virtual BREAKPOINT_ID getId() const {
        return m_id;
    }

    virtual MEMOFFSET_64 getOffset() const {
        return m_offset;
    }

    virtual BreakpointCallback* getCallback() {
        return m_callback;
    }

    virtual void remove();

protected:

    BREAKPOINT_ID  m_id;
    MEMOFFSET_64  m_offset;
    BreakpointCallback*  m_callback;
};



class SoftwareBreakpointImpl : public BaseBreakpointImpl
{

public:

    explicit SoftwareBreakpointImpl(MEMOFFSET_64 offset) : BaseBreakpointImpl(offset, NULL){}

    SoftwareBreakpointImpl(MEMOFFSET_64 offset, BreakpointCallback *callback) : BaseBreakpointImpl(offset,callback){}

    SoftwareBreakpointImpl(MEMOFFSET_64 offset, BREAKPOINT_ID id) : BaseBreakpointImpl(offset) {
        m_id = id;
    }

    virtual void set();

    virtual BreakpointType getType() const {
        return SoftwareBreakpoint;
    }

};

class HardwareBreakpointImpl : public BaseBreakpointImpl
{
public:

    HardwareBreakpointImpl(MEMOFFSET_64 offset, size_t size, ACCESS_TYPE accessType, BreakpointCallback *callback) 
        : BaseBreakpointImpl(offset,callback),
        m_size(size), 
        m_accessType(accessType)
        {}

    HardwareBreakpointImpl(MEMOFFSET_64 offset, size_t size, ACCESS_TYPE accessType) 
        : BaseBreakpointImpl(offset, NULL),
        m_size(size), 
        m_accessType(accessType)
        {}

    HardwareBreakpointImpl(MEMOFFSET_64 offset, size_t size, ACCESS_TYPE accessType, BREAKPOINT_ID id) 
        : BaseBreakpointImpl(offset, NULL),
        m_size(size), 
        m_accessType(accessType)
        {
            m_id = id;
        }

    virtual void set();

    virtual BreakpointType getType() const {
        return DataAccessBreakpoint;
    }

private:

    size_t  m_size;
    ACCESS_TYPE  m_accessType;
};

///////////////////////////////////////////////////////////////////////////////

BreakpointPtr  getBreakpoint(IDebugBreakpoint* bp)
{
    HRESULT  hres;

    ULONG  breakType;
    ULONG  procType;
    hres = bp->GetType(&breakType, &procType);
    if ( FAILED(hres) )
    {
        throw DbgEngException(L"IDebugBreakpoint::GetType", hres); 
    }

    ULONG64  bpoffset = 0;
    hres = bp->GetOffset(&bpoffset);
    if ( FAILED(hres) )
    {
        throw DbgEngException(L"IDebugBreakpoint::GetOffset", hres); 
    }
    bpoffset = addr64(bpoffset);

    ULONG bpid;
    hres = bp->GetId(&bpid);

    if ( breakType == DEBUG_BREAKPOINT_CODE )
    {
        return BreakpointPtr( new SoftwareBreakpointImpl(bpoffset, bpid) );
    }

    ULONG  dataSize;
    ULONG  dataAccessType;
    hres = bp->GetDataParameters(&dataSize, &dataAccessType);
    if ( FAILED(hres) )
    {
        throw DbgEngException(L"IDebugBreakpoint::GetDataParameters", hres); 
    }

    return BreakpointPtr( new HardwareBreakpointImpl(bpoffset, dataSize, dataAccessType, bpid) );
}

///////////////////////////////////////////////////////////////////////////////

BreakpointPtr softwareBreakPointSet(MEMOFFSET_64 offset, BreakpointCallback *callback)
{
    BreakpointPtr  bp = BreakpointPtr( new SoftwareBreakpointImpl(offset,callback) );
    bp->set();
    return bp;
}

///////////////////////////////////////////////////////////////////////////////

BreakpointPtr hardwareBreakPointSet(MEMOFFSET_64 offset, size_t size, ACCESS_TYPE accessType, BreakpointCallback *callback)
{
    BreakpointPtr  bp = BreakpointPtr( new HardwareBreakpointImpl(offset, size, accessType, callback) );
    bp->set();
    return bp;
}

///////////////////////////////////////////////////////////////////////////////

unsigned long getNumberBreakpoints()
{
    HRESULT  hres;
    ULONG  numberBreakpoints = 0;

    hres = g_dbgMgr->control->GetNumberBreakpoints(&numberBreakpoints);
    if (FAILED(hres))
    {
        throw DbgEngException(L"IDebugControl::GetNumberBreakpoints", hres);
    }

    return numberBreakpoints;
}

///////////////////////////////////////////////////////////////////////////////

BreakpointPtr getBreakpointByIndex(unsigned long index)
{
    HRESULT  hres;

    IDebugBreakpoint  *bp = NULL;
    hres = g_dbgMgr->control->GetBreakpointByIndex(index, &bp);
    if ( FAILED(hres) )
        throw IndexException( index );

    return getBreakpoint(bp);
}

///////////////////////////////////////////////////////////////////////////////

void BaseBreakpointImpl::remove() 
{
    HRESULT  hres;

    IDebugBreakpoint  *bp = NULL;
    hres = g_dbgMgr->control->GetBreakpointById(m_id, &bp);
    if (SUCCEEDED(hres))
    {
        ProcessMonitor::removeBreakpoint(getBreakpoint(bp));

        g_dbgMgr->control->RemoveBreakpoint(bp);
    }
}

///////////////////////////////////////////////////////////////////////////////

void SoftwareBreakpointImpl::set()
{
    HRESULT  hres;

    IDebugBreakpoint  *bp;
    hres = g_dbgMgr->control->AddBreakpoint(
        DEBUG_BREAKPOINT_CODE,
        DEBUG_ANY_ID,
        &bp);

    if (S_OK != hres)
        throw DbgEngException(L"IDebugControl::AddBreakpoint", hres);

    hres = bp->SetOffset(m_offset);
    if (S_OK != hres)
    {
        g_dbgMgr->control->RemoveBreakpoint(bp);
        throw DbgEngException(L"IDebugBreakpoint::SetOffset", hres);
    }

    ULONG bpFlags;
    hres = bp->GetFlags(&bpFlags);
    if (S_OK != hres)
    {
        g_dbgMgr->control->RemoveBreakpoint(bp);
        throw DbgEngException(L"IDebugBreakpoint::GetFlags", hres);
    }
    
    bpFlags |= DEBUG_BREAKPOINT_ENABLED | DEBUG_BREAKPOINT_ADDER_ONLY | DEBUG_BREAKPOINT_GO_ONLY;
    hres = bp->SetFlags(bpFlags);
    if (S_OK != hres)
    {
        g_dbgMgr->control->RemoveBreakpoint(bp);
        throw DbgEngException( L"IDebugBreakpoint::SetFlags", hres);
    }

    hres = bp->GetId(&m_id);
    if ( FAILED(hres) )
    {
        g_dbgMgr->control->RemoveBreakpoint(bp);
        throw DbgEngException( L"IDebugBreakpoint::GetId", hres);
    }

    BreakpointPtr  ptr = shared_from_this();
    ProcessMonitor::registerBreakpoint(ptr);
}

///////////////////////////////////////////////////////////////////////////////

void HardwareBreakpointImpl::set()
{
    HRESULT  hres;

    IDebugBreakpoint  *bp;
    hres = g_dbgMgr->control->AddBreakpoint(
        DEBUG_BREAKPOINT_DATA,
        DEBUG_ANY_ID,
        &bp);
    if (S_OK != hres)
        throw DbgEngException(L"IDebugControl::AddBreakpoint", hres);

    hres = bp->SetOffset(m_offset);
    if (S_OK != hres)
    {
        g_dbgMgr->control->RemoveBreakpoint(bp);
        throw DbgEngException(L"IDebugBreakpoint::SetOffset", hres);
    }

    ULONG bpFlags;
    hres = bp->GetFlags(&bpFlags);
    if (S_OK != hres)
    {
        g_dbgMgr->control->RemoveBreakpoint(bp);
        throw DbgEngException(L"IDebugBreakpoint::GetFlags", hres);
    }

    hres = bp->SetDataParameters(static_cast<ULONG>(m_size), m_accessType);
    if (S_OK != hres)
    {
        g_dbgMgr->control->RemoveBreakpoint(bp);
        throw DbgEngException(L"IDebugBreakpoint::SetDataParameters", hres);
    }

    bpFlags |= DEBUG_BREAKPOINT_ENABLED | DEBUG_BREAKPOINT_ADDER_ONLY | DEBUG_BREAKPOINT_GO_ONLY;
    hres = bp->SetFlags(bpFlags);
    if (S_OK != hres)
    {
        g_dbgMgr->control->RemoveBreakpoint(bp);
        throw DbgEngException(L"IDebugBreakpoint::SetFlags", hres);
    }

    hres = bp->GetId(&m_id);
    if ( FAILED(hres) )
    {
        g_dbgMgr->control->RemoveBreakpoint(bp);
        throw DbgEngException( L"IDebugBreakpoint::GetId", hres);
    }

    BreakpointPtr  ptr = shared_from_this();
    ProcessMonitor::registerBreakpoint(ptr);
}

///////////////////////////////////////////////////////////////////////////////

DebugCallbackResult BreakpointCallbackHandler(IDebugBreakpoint2 *bp2)
{
    CComQIPtr<IDebugBreakpoint>  bp1 = bp2;
    
    BreakpointPtr  bpptr = getBreakpoint(bp1);

    return ProcessMonitor::breakpointHit( getCurrentProcessId(), bpptr);
}

///////////////////////////////////////////////////////////////////////////////


} // namespace kdlib
