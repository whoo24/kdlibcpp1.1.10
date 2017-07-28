#include "stdafx.h"

#include <cvconst.h>

#include "kdlib/dbgengine.h"
#include "kdlib/exceptions.h"
#include "kdlib/memaccess.h"

#include "stackimpl.h"
#include "cpucontextimpl.h"
#include "dbgmgr.h"


namespace kdlib {

///////////////////////////////////////////////////////////////////////////////

NumVariant getRegisterByName(const std::wstring& regName)
{
    unsigned long index = getRegisterIndex(regName);
    return getRegisterByIndex(index);
}

///////////////////////////////////////////////////////////////////////////////

NumVariant getRegisterByIndex(unsigned long index)
{
    CPURegType  regType = getRegisterType(index);

    switch( regType )
    {
    case  RegInt8:
        {
            unsigned char  val;
            getRegisterValue( index, &val, sizeof(val) );
            return NumVariant(val);
        }
        break;

    case  RegInt16:
        {
            unsigned short  val;
            getRegisterValue( index, &val, sizeof(val) );
            return NumVariant(val);
        }
        break;

    case  RegInt32:
        {
            unsigned long  val;
            getRegisterValue( index, &val, sizeof(val) );
            return NumVariant(val);
        }
        break;


    case  RegInt64:
        {
            unsigned long long  val;
            getRegisterValue( index, &val, sizeof(val) );
            return NumVariant(val);
        }
        break;

    case  RegFloat32:
        {
            float  val;
            getRegisterValue( index, &val, sizeof(val) );
            return NumVariant(val);
        }
        break;


    case  RegFloat64:
        {
            double  val;
            getRegisterValue( index, &val, sizeof(val) );
            return NumVariant(val);
        }
        break;

    case RegFloat80:
    case RegFloat128:
    case RegVector64:
    case RegVector128:
        throw DbgException( "unsupported registry type");
    }

    throw DbgException( "unsupported registry type");
}

///////////////////////////////////////////////////////////////////////////////

void setRegisterByName(const std::wstring& regName, const NumVariant& value)
{
    unsigned long index = getRegisterIndex(regName);
    return setRegisterByIndex(index, value);
}

///////////////////////////////////////////////////////////////////////////////

void setRegisterByIndex(unsigned long index, const NumVariant& value)
{
    CPURegType  regType = getRegisterType(index);

    switch( regType )
    {
    case  RegInt8:
        {
            unsigned char  val = value.asUChar();
            setRegisterValue( index, &val, sizeof(val) );
            return;
        }
        break;

    case  RegInt16:
        {
            unsigned short  val = value.asUShort();
            setRegisterValue( index, &val, sizeof(val) );
            return;
        }
        break;

    case  RegInt32:
        {
            unsigned long  val = value.asULong();
            setRegisterValue( index, &val, sizeof(val) );
            return;
        }
        break;


    case  RegInt64:
        {
            unsigned long long  val = value.asULongLong();
            setRegisterValue( index, &val, sizeof(val) );
            return;
        }
        break;

    case  RegFloat32:
        {
            float  val = value.asFloat();
            setRegisterValue( index, &val, sizeof(val) );
            return;
        }
        break;


    case  RegFloat64:
        {
            double  val = value.asDouble();
            setRegisterValue( index, &val, sizeof(val) );
            return;
        }
        break;

    case RegFloat80:
    case RegFloat128:
    case RegVector64:
    case RegVector128:
        throw DbgException( "unsupported registry type");
    }

    throw DbgException( "unsupported registry type");
}

///////////////////////////////////////////////////////////////////////////////

int alignStackPointer( int byteCount )
{
    int machineWord;

    switch( getCPUMode() )
    {
    case CPU_I386:
        machineWord = 4;
        break;

    case CPU_AMD64:
        machineWord = 8;
        break;

    default:
        throw DbgException( "Unknown processor type" );
    }

    return ( ( byteCount - 1) / machineWord + 1 ) * machineWord;
}

///////////////////////////////////////////////////////////////////////////////

MEMOFFSET_64 stackAlloc(unsigned short byteCount)
{
    byteCount = alignStackPointer(byteCount);

    MEMOFFSET_64  offset = getStackOffset() - byteCount;
    setStackOffset(offset);
    return offset;
}

///////////////////////////////////////////////////////////////////////////////

MEMOFFSET_64 stackFree(unsigned short byteCount)
{
    byteCount = alignStackPointer(byteCount);

    MEMOFFSET_64  offset = getStackOffset() + byteCount;
    setStackOffset(offset);
    return offset;
}

///////////////////////////////////////////////////////////////////////////////

void pushInStack(const NumVariant& value)
{
    if ( value.isChar() || value.isUChar() )
    {
        setByte( stackAlloc(1), value.asUChar() );
        return;
    }

    if ( value.isShort() || value.isUShort() )
    {
        setWord( stackAlloc(2), value.asUShort() );
        return;
    }

    if ( value.isLong() || value.isULong() || value.isInt()  || value.isUInt() )
    {
        setDWord( stackAlloc(4), value.asULong() );
        return;
    }

    if ( value.isLongLong() || value.isULongLong() )
    {
        setQWord( stackAlloc(8), value.asULongLong() );
        return;
    }

    if ( value.isFloat() )
    {
        setSingleFloat( stackAlloc(4), value.asFloat() );
        return;
    }

    if ( value.isDouble() )
    {
        setDoubleFloat( stackAlloc(8), value.asDouble() );
        return;
    }

    throw DbgException("unsupported argument type");
}

///////////////////////////////////////////////////////////////////////////////

void popFromStack(NumVariant& value)
{
    NOT_IMPLEMENTED();
}

///////////////////////////////////////////////////////////////////////////////

//void CallArg::pushInStack() const
//{
//    writeBytes(stackAlloc(m_rawBuffer.size()), m_rawBuffer);
//}
//
/////////////////////////////////////////////////////////////////////////////////
//
//void CallArg::saveToRegister(const std::wstring& regName) const
//{
//    unsigned long   regIndex  = getRegsiterIndex(regName);
//    setRegisterValue(regIndex,const_cast<unsigned char*>(&m_rawBuffer[0]), m_rawBuffer.size() );
//}

///////////////////////////////////////////////////////////////////////////////

CPUContextPtr loadCPUContext()
{
    switch( kdlib::getCPUType() )
    {
    case CPU_AMD64:

        if ( kdlib::getCPUMode() == CPU_AMD64 )
            return CPUContextPtr( new CPUContextAmd64() );
        else if ( kdlib::getCPUMode() == CPU_I386 )
            return CPUContextPtr( new CPUContextImpl() );

        throw DbgException("Unknown CPU Mode");

    case CPU_I386:

        return CPUContextPtr( new CPUContextI386() ); 
    }

    return CPUContextPtr( new CPUContextImpl() );
}

///////////////////////////////////////////////////////////////////////////////

CPUContextImpl::CPUContextImpl()
{
    m_cpuType = kdlib::getCPUType();

    m_cpuMode = kdlib::getCPUMode();

    unsigned long  registerNumber = kdlib::getRegisterNumber();

    for ( unsigned long  i = 0; i < registerNumber; ++i)
    {
        const std::wstring name = kdlib::getRegisterName(i);
        try
        {
            m_indexedValues.insert( std::make_pair( i, kdlib::getRegisterByIndex(i) ) );
            m_indexedNames.insert( std::make_pair( name, i ) );
        }
        catch (const DbgException &)
        {
        }
    }
}

///////////////////////////////////////////////////////////////////////////////

NumVariant CPUContextImpl::getRegisterByName( const std::wstring &name)
{
    std::map<std::wstring, unsigned long>::iterator  foundName = m_indexedNames.find(name);

    if ( foundName == m_indexedNames.end() )
        throw DbgException("register not found");

    return m_indexedValues[foundName->second];
}

///////////////////////////////////////////////////////////////////////////////

NumVariant CPUContextImpl::getRegisterByIndex( unsigned long index )
{
    std::map<unsigned long, NumVariant>::iterator  found = m_indexedValues.find(index);

    if ( found == m_indexedValues.end() )
        throw IndexException(index);

    return m_indexedValues[index];
}

///////////////////////////////////////////////////////////////////////////////

std::wstring CPUContextImpl::getRegisterName( unsigned long index )
{
    for ( std::map<std::wstring, unsigned long>::iterator it = m_indexedNames.begin(); it != m_indexedNames.end(); ++it)
    {
        if ( it->second == index )
            return it->first;
    }

    throw IndexException(index);
}

///////////////////////////////////////////////////////////////////////////////

void CPUContextImpl::restore()
{
   for(  std::map<unsigned long, NumVariant>::iterator it = m_indexedValues.begin(); it != m_indexedValues.end(); ++it )
   {
        try 
        {
            kdlib::setRegisterByIndex( it->first, it->second);
        }
        catch (const DbgException &)
        {
        }
   }
}

///////////////////////////////////////////////////////////////////////////////

MEMOFFSET_64 CPUContextImpl::getIP()
{
    if ( m_cpuMode == CPU_I386 )
    {
        return addr64( getRegisterByName(L"eip").asULong() );
    }
    else if ( m_cpuMode == CPU_AMD64 )
    {
        return addr64( getRegisterByName(L"rip").asULongLong() );
    }

    throw DbgException("Unknown CPU type");
}

///////////////////////////////////////////////////////////////////////////////

void CPUContextImpl::setIP(MEMOFFSET_64 ip)
{
    if ( m_cpuMode == CPU_I386 )
    {
        setRegisterByName(L"eip", NumVariant(ip));
    }
    else if ( m_cpuMode == CPU_AMD64 )
    {
        setRegisterByName(L"rip", NumVariant(ip));
    }

    assert(0);
}

///////////////////////////////////////////////////////////////////////////////

MEMOFFSET_64 CPUContextImpl::getSP()
{
    if ( m_cpuMode == CPU_I386 )
    {
        return addr64( getRegisterByName(L"esp").asULong() );
    }
    else if ( m_cpuMode == CPU_AMD64 )
    {
        return addr64( getRegisterByName(L"rsp").asULongLong() );
    }

    throw DbgException("Unknown CPU type");
}

///////////////////////////////////////////////////////////////////////////////

void CPUContextImpl::setSP(MEMOFFSET_64 sp)
{
    if ( m_cpuMode == CPU_I386 )
    {
        setRegisterByName(L"esp", NumVariant(sp));
    }
    else if ( m_cpuMode == CPU_I386 )
    {
        setRegisterByName(L"esp", NumVariant(sp));
    }

    throw DbgException("Unknown CPU type");
}

///////////////////////////////////////////////////////////////////////////////

MEMOFFSET_64 CPUContextImpl::getFP()
{
    if ( m_cpuMode == CPU_I386 )
    {
        return addr64( getRegisterByName(L"ebp").asULong() );
    }
    else if ( m_cpuMode == CPU_AMD64 )
    {
        return addr64( getRegisterByName(L"rbp").asULongLong() );
    }

    throw DbgException("Unknown CPU type");
}

///////////////////////////////////////////////////////////////////////////////

void CPUContextImpl::setFP(MEMOFFSET_64 fp)
{
    if ( m_cpuMode == CPU_I386 )
    {
        setRegisterByName(L"ebp", NumVariant(fp));
    }
    else if (m_cpuMode == CPU_AMD64 )
    {
        setRegisterByName(L"rbp", NumVariant(fp));
    }

    assert(0);
}

///////////////////////////////////////////////////////////////////////////////

StackPtr getStackAmd64()
{
    HRESULT  hres;

    ULONG   filledFrames = 1024;
    std::vector<DEBUG_STACK_FRAME>  frames(filledFrames);
    std::vector<CONTEXT_X64>  contexts(filledFrames);

    g_dbgMgr->setQuietNotiification(true);

    hres = 
        g_dbgMgr->control->GetContextStackTrace(
            NULL,
            0,
            &frames[0],
            filledFrames,
            &contexts[0],
            filledFrames*sizeof(CONTEXT_X64),
            sizeof(CONTEXT_X64),
            &filledFrames 
            );

    g_dbgMgr->setQuietNotiification(false);

    if (S_OK != hres)
        throw DbgEngException( L"IDebugControl::GetContextStackTrace", hres );


    std::vector<StackFramePtr>  stackFrames;

    for (ULONG i = 0; i < filledFrames; ++i)
    {
        stackFrames.push_back(StackFramePtr(new StackFrameImpl(
            i,
            frames[i].InstructionOffset,
            frames[i].ReturnOffset,
            frames[i].FrameOffset,
            frames[i].StackOffset,
            CPUContextPtr(new CPUContextAmd64(contexts[i])))));
    }

    return StackPtr(new StackImpl(stackFrames));
}

///////////////////////////////////////////////////////////////////////////////

void ReadWow64Context(WOW64_CONTEXT& wow64Context)
{
    // 
    //  *** undoc ***
    // !wow64exts.r
    // http://www.woodmann.com/forum/archive/index.php/t-11162.html
    // http://www.nynaeve.net/Code/GetThreadWow64Context.cpp
    // 

    HRESULT     hres;

    ULONG64 teb64Address;

    ULONG debugClass, debugQualifier;
    hres = g_dbgMgr->control->GetDebuggeeType(&debugClass, &debugQualifier);

    if (S_OK != hres)
        throw DbgEngException(L"IDebugControl::GetDebuggeeType", hres);

    if (debugClass == DEBUG_CLASS_KERNEL)
    {
        DEBUG_VALUE  debugValue = {};
        ULONG        remainderIndex = 0;

        hres =
            g_dbgMgr->control->EvaluateWide(
            L"@@C++(#FIELD_OFFSET(nt!_KTHREAD, Teb))",
            DEBUG_VALUE_INT64,
            &debugValue,
            &remainderIndex);
        if (FAILED(hres))
            throw  DbgEngException(L"IDebugControl::Evaluate", hres);

        ULONG64 tebOffset = debugValue.I64;

        hres = g_dbgMgr->system->GetImplicitThreadDataOffset(&teb64Address);
        if (S_OK != hres)
            throw DbgEngException(L"IDebugSystemObjects::GetImplicitThreadDataOffset", hres);

        ULONG readedBytes;

        hres =
            g_dbgMgr->dataspace->ReadVirtual(
            teb64Address + tebOffset,
            &teb64Address,
            sizeof(teb64Address),
            &readedBytes);
        if (FAILED(hres))
            throw MemoryException(teb64Address + tebOffset);
    }
    else
    {
        hres = g_dbgMgr->system->GetImplicitThreadDataOffset(&teb64Address);
        if (S_OK != hres)
            throw DbgEngException(L"IDebugSystemObjects::GetImplicitThreadDataOffset", hres);
    }

    // ? @@C++(#FIELD_OFFSET(nt!_TEB64, TlsSlots))
    // hardcoded in !wow64exts.r (6.2.8250.0)
    static const ULONG teb64ToTlsOffset = 0x01480;
    static const ULONG WOW64_TLS_CPURESERVED = 1;
    ULONG64 cpuAreaAddress;
    ULONG readedBytes;

    hres = g_dbgMgr->dataspace->ReadVirtual(
        teb64Address + teb64ToTlsOffset + (sizeof(ULONG64) * WOW64_TLS_CPURESERVED),
        &cpuAreaAddress,
        sizeof(cpuAreaAddress),
        &readedBytes);

    if (S_OK != hres)
        throw MemoryException(teb64Address + teb64ToTlsOffset + (sizeof(ULONG64) * WOW64_TLS_CPURESERVED));

    // CPU Area is:
    // +00 unknown ULONG
    // +04 WOW64_CONTEXT struct
    static const ULONG cpuAreaToWow64ContextOffset = sizeof(ULONG);

    hres =
        g_dbgMgr->dataspace->ReadVirtual(
        cpuAreaAddress + cpuAreaToWow64ContextOffset,
        &wow64Context,
        sizeof(wow64Context),
        &readedBytes);

    if (S_OK != hres)
        throw MemoryException(cpuAreaAddress + cpuAreaToWow64ContextOffset);
}

///////////////////////////////////////////////////////////////////////////////

StackPtr getStackWow64()
{
    HRESULT  hres;

    ULONG   filledFrames = 1024;
    std::vector<DEBUG_STACK_FRAME>  frames(filledFrames);
   // std::vector<WOW64_CONTEXT>  contexts(filledFrames);

    WOW64_CONTEXT  wow64Context;
    ReadWow64Context( wow64Context);

    g_dbgMgr->setQuietNotiification(true);

    hres =
        g_dbgMgr->control->GetContextStackTrace(
            &wow64Context,
            sizeof(WOW64_CONTEXT),
            &frames[0],
            filledFrames,
            NULL, //&contexts[0],
            filledFrames*sizeof(WOW64_CONTEXT),
            sizeof(WOW64_CONTEXT),
            &filledFrames
            );

    g_dbgMgr->setQuietNotiification(false);

    if (S_OK != hres)
        throw DbgEngException(L"IDebugControl::GetContextStackTrace", hres);

    std::vector<StackFramePtr>  stackFrames;

    for (ULONG i = 0; i < filledFrames; ++i)
    {
        stackFrames.push_back(StackFramePtr(new StackFrameImpl(
            i,
            frames[i].InstructionOffset,
            frames[i].ReturnOffset,
            frames[i].FrameOffset,
            frames[i].StackOffset,
            CPUContextPtr(new CPUContextWOW64(wow64Context)))));
    }

    return StackPtr(new StackImpl(stackFrames));
}

///////////////////////////////////////////////////////////////////////////////

StackPtr getStackX86()
{
    HRESULT  hres;

    ULONG   filledFrames = 1024;
    std::vector<DEBUG_STACK_FRAME>  frames(filledFrames);
    std::vector<CONTEXT_X86>  contexts(filledFrames);

    g_dbgMgr->setQuietNotiification(true);

    hres =
        g_dbgMgr->control->GetContextStackTrace(
        NULL,
        0,
        &frames[0],
        filledFrames,
        &contexts[0],
        filledFrames*sizeof(CONTEXT_X86),
        sizeof(CONTEXT_X86),
        &filledFrames
        );

    g_dbgMgr->setQuietNotiification(false);

    if (S_OK != hres)
        throw DbgEngException(L"IDebugControl::GetContextStackTrace", hres);


    std::vector<StackFramePtr>  stackFrames;

    for (ULONG i = 0; i < filledFrames; ++i)
    {
        stackFrames.push_back(StackFramePtr(new StackFrameImpl(
            i,
            frames[i].InstructionOffset,
            frames[i].ReturnOffset,
            frames[i].FrameOffset,
            frames[i].StackOffset,
            CPUContextPtr(new CPUContextI386(contexts[i])))));
    }

    return StackPtr(new StackImpl(stackFrames));
}

///////////////////////////////////////////////////////////////////////////////


StackPtr getStack()
{
    HRESULT  hres;
    
    ULONG  cpuType;
    hres = g_dbgMgr->control->GetActualProcessorType((PULONG)&cpuType);

    if (FAILED(hres))
        throw DbgEngException(L"IDebugControl::GetActualProcessorType", hres);

    ULONG  cpuMode;
    hres = g_dbgMgr->control->GetEffectiveProcessorType((PULONG)&cpuMode);
    if (FAILED(hres))
        throw DbgEngException(L"IDebugControl::GetEffectiveProcessorType", hres);

    if (cpuType == IMAGE_FILE_MACHINE_AMD64 && cpuMode == IMAGE_FILE_MACHINE_AMD64)
        return getStackAmd64();

    if (cpuType == IMAGE_FILE_MACHINE_AMD64 && cpuMode == IMAGE_FILE_MACHINE_I386)
        return getStackWow64();

    if (cpuType == IMAGE_FILE_MACHINE_I386)
        return getStackX86(); 
    
    throw DbgException("Unknown CPU type");
}

///////////////////////////////////////////////////////////////////////////////

StackFramePtr getStackFrameAmd64()
{
    HRESULT  hres;
    DEBUG_STACK_FRAME  stackFrame = {};
    ULONG  frameNumber;

    ULONG   filledFrames = 1024;
    std::vector<DEBUG_STACK_FRAME>  frames(filledFrames);
    std::vector<CONTEXT_X64>  contexts(filledFrames);
    
    g_dbgMgr->setQuietNotiification(true);

    hres = g_dbgMgr->symbols->GetScope(NULL, &stackFrame, NULL, 0);

    frameNumber = stackFrame.FrameNumber;

    if (FAILED(hres))
    {
        g_dbgMgr->setQuietNotiification(false);
        throw DbgEngException(L"IDebugSymbols::GetScope", hres);
    }

    hres =
        g_dbgMgr->control->GetContextStackTrace(
        NULL,
        0,
        &frames[0],
        filledFrames,
        &contexts[0],
        filledFrames*sizeof(CONTEXT_X64),
        sizeof(CONTEXT_X64),
        &filledFrames
        );

    if (FAILED(hres))
    {
        g_dbgMgr->setQuietNotiification(false);
        throw DbgEngException(L"IDebugControl4::GetContextStackTrace", hres);
    }

    g_dbgMgr->setQuietNotiification(false);

    return StackFramePtr(new StackFrameImpl(
        frameNumber,
        frames[frameNumber].InstructionOffset,
        frames[frameNumber].ReturnOffset,
        frames[frameNumber].FrameOffset,
        frames[frameNumber].StackOffset,
        CPUContextPtr(new CPUContextAmd64(contexts[frameNumber]))));
}

///////////////////////////////////////////////////////////////////////////////

StackFramePtr getStackFrameWow64()
{
    HRESULT  hres;
    DEBUG_STACK_FRAME  stackFrame = {};
    ULONG  frameNumber;

    ULONG   filledFrames = 1024;
    std::vector<DEBUG_STACK_FRAME>  frames(filledFrames);
    //std::vector<WOW64_CONTEXT>  contexts(filledFrames);

    g_dbgMgr->setQuietNotiification(true);

    hres = g_dbgMgr->symbols->GetScope(NULL, &stackFrame, NULL, 0);

    frameNumber = stackFrame.FrameNumber;

    if (FAILED(hres))
    {
        g_dbgMgr->setQuietNotiification(false);
        throw DbgEngException(L"IDebugSymbols::GetScope", hres);
    }

    WOW64_CONTEXT  wow64Context;
    ReadWow64Context(wow64Context);

    hres =
        g_dbgMgr->control->GetContextStackTrace(
            &wow64Context,
            sizeof(wow64Context),
            &frames[0],
            filledFrames,
            NULL, //&contexts[0],
            filledFrames*sizeof(WOW64_CONTEXT),
            sizeof(WOW64_CONTEXT),
            &filledFrames
            );

    if (FAILED(hres))
    {
        g_dbgMgr->setQuietNotiification(false);
        throw DbgEngException(L"IDebugControl4::GetContextStackTrace", hres);
    }

    g_dbgMgr->setQuietNotiification(false);

    return StackFramePtr(new StackFrameImpl(
        frameNumber,
        frames[frameNumber].InstructionOffset,
        frames[frameNumber].ReturnOffset,
        frames[frameNumber].FrameOffset,
        frames[frameNumber].StackOffset,
        CPUContextPtr(new CPUContextWOW64(wow64Context))));
}

///////////////////////////////////////////////////////////////////////////////

StackFramePtr getStackFrameX86()
{
    HRESULT  hres;
    DEBUG_STACK_FRAME  stackFrame = {};
    ULONG  frameNumber;

    ULONG   filledFrames = 1024;
    std::vector<DEBUG_STACK_FRAME>  frames(filledFrames);
    std::vector<CONTEXT_X86>  contexts(filledFrames);
    
    g_dbgMgr->setQuietNotiification(true);

    hres = g_dbgMgr->symbols->GetScope(NULL, &stackFrame, NULL, 0);

    frameNumber = stackFrame.FrameNumber;

    if (FAILED(hres))
    {
        g_dbgMgr->setQuietNotiification(false);
        throw DbgEngException(L"IDebugSymbols::GetScope", hres);
    }

    hres =
        g_dbgMgr->control->GetContextStackTrace(
        NULL,
        0,
        &frames[0],
        filledFrames,
        &contexts[0],
        filledFrames*sizeof(CONTEXT_X86),
        sizeof(CONTEXT_X86),
        &filledFrames
        );

    if (FAILED(hres))
    {
        g_dbgMgr->setQuietNotiification(false);
        throw DbgEngException(L"IDebugControl4::GetContextStackTrace", hres);
    }

    g_dbgMgr->setQuietNotiification(false);

    return StackFramePtr(new StackFrameImpl(
        frameNumber,
        frames[frameNumber].InstructionOffset,
        frames[frameNumber].ReturnOffset,
        frames[frameNumber].FrameOffset,
        frames[frameNumber].StackOffset,
        CPUContextPtr(new CPUContextI386(contexts[frameNumber]))));
}

///////////////////////////////////////////////////////////////////////////////

StackFramePtr getCurrentStackFrame()
{
    HRESULT  hres;

    ULONG  cpuType;
    hres = g_dbgMgr->control->GetActualProcessorType((PULONG)&cpuType);

    if (FAILED(hres))
        throw DbgEngException(L"IDebugControl::GetActualProcessorType", hres);

    ULONG  cpuMode;
    hres = g_dbgMgr->control->GetEffectiveProcessorType((PULONG)&cpuMode);
    if (FAILED(hres))
        throw DbgEngException(L"IDebugControl::GetEffectiveProcessorType", hres);

    if (cpuType == IMAGE_FILE_MACHINE_AMD64 && cpuMode == IMAGE_FILE_MACHINE_AMD64)
        return getStackFrameAmd64();

    if (cpuType == IMAGE_FILE_MACHINE_AMD64 && cpuMode == IMAGE_FILE_MACHINE_I386)
        return getStackFrameWow64();

    if (cpuType == IMAGE_FILE_MACHINE_I386)
        return getStackFrameX86();

    throw DbgException("Unknown CPU type");
}

///////////////////////////////////////////////////////////////////////////////

unsigned long getCurrentStackFrameNumber()
{
    HRESULT  hres;

    ULONG  frameIndex;
    hres = g_dbgMgr->symbols->GetCurrentScopeFrameIndex(&frameIndex);
    if (FAILED(hres))
        throw DbgEngException(L"IDebugSymbols::GetCurrentScopeFrameIndex", hres);

    return frameIndex;
}

///////////////////////////////////////////////////////////////////////////////

void setCurrentStackFrame(StackFramePtr& stackFrame)
{
    HRESULT  hres;
    
    DEBUG_STACK_FRAME  debugStackFrame = {};
    
    debugStackFrame.InstructionOffset = stackFrame->getIP();
    debugStackFrame.ReturnOffset = stackFrame->getRET();
    debugStackFrame.FrameOffset = stackFrame->getFP();
    debugStackFrame.StackOffset = stackFrame->getSP();
    debugStackFrame.FrameNumber = stackFrame->getNumber();
    
    hres = g_dbgMgr->symbols->SetScope(0, &debugStackFrame, NULL, 0);
    if ( FAILED(hres) )
        throw DbgEngException(L"IDebugSymbols::SetScope", hres);
}

///////////////////////////////////////////////////////////////////////////////

void setCurrentStackFrameByIndex(unsigned long frameIndex)
{
    HRESULT  hres;

    hres = g_dbgMgr->symbols->SetScopeFrameByIndex(frameIndex);
    if (FAILED(hres))
        throw DbgEngException(L"IDebugSymbols::SetScopeFrameByIndex", hres);
}

///////////////////////////////////////////////////////////////////////////////

void resetCurrentStackFrame()
{
    HRESULT  hres;
    
    hres = g_dbgMgr->symbols->ResetScope();
    if ( FAILED(hres) )
        throw DbgEngException(L"IDebugSymbols::ResetScope", hres);
}

///////////////////////////////////////////////////////////////////////////////

CPUContextI386::CPUContextI386()
{
    HRESULT  hres;

    hres = g_dbgMgr->advanced->GetThreadContext(&m_context, sizeof(m_context) );
    if ( FAILED(hres) )
        throw DbgEngException(L"IDebugAdvanced::GetThreadContext", hres);
}

///////////////////////////////////////////////////////////////////////////////

void CPUContextI386::restore()
{
    HRESULT  hres;

    hres = g_dbgMgr->advanced->SetThreadContext(&m_context, sizeof(m_context) );
    if ( FAILED(hres) )
        throw DbgEngException(L"IDebugAdvanced::GetThreadContext", hres);
}

///////////////////////////////////////////////////////////////////////////////

NumVariant CPUContextI386::getRegisterByIndex(unsigned long index)
{
    switch (index)
    {
    case CV_REG_AL:
        return NumVariant(static_cast<unsigned char>(m_context.Eax & 0xFF));
    case CV_REG_CL:
        return NumVariant(static_cast<unsigned char>(m_context.Ecx & 0xFF));
    case CV_REG_DL:
        return NumVariant(static_cast<unsigned char>(m_context.Edx & 0xFF));
    case CV_REG_BL:
        return NumVariant(static_cast<unsigned char>(m_context.Ebx & 0xFF));
    case CV_REG_AH:
        return NumVariant(static_cast<unsigned char>((m_context.Eax & 0xFF00) >> 8));
    case CV_REG_CH:
        return NumVariant(static_cast<unsigned char>((m_context.Ecx & 0xFF00) >> 8));
    case CV_REG_DH:
        return NumVariant(static_cast<unsigned char>((m_context.Edx & 0xFF00) >> 8));
    case CV_REG_BH:
       return NumVariant(static_cast<unsigned char>((m_context.Ebx & 0xFF00) >> 8));
    case CV_REG_AX:
        return NumVariant(static_cast<unsigned short>(m_context.Eax & 0xFFFF));
    case CV_REG_CX:
        return NumVariant(static_cast<unsigned short>(m_context.Ecx & 0xFFFF));
    case CV_REG_DX:
        return NumVariant(static_cast<unsigned short>(m_context.Edx & 0xFFFF));
    case CV_REG_BX:
        return NumVariant(static_cast<unsigned short>(m_context.Ebx & 0xFFFF));
    case CV_REG_SP:
        return NumVariant(static_cast<unsigned short>(m_context.Esp & 0xFFFF));
    case CV_REG_BP:
        return NumVariant(static_cast<unsigned short>(m_context.Ebp & 0xFFFF));
    case CV_REG_SI:
        return NumVariant(static_cast<unsigned short>(m_context.Esi & 0xFFFF));
    case CV_REG_DI:
        return NumVariant(static_cast<unsigned short>(m_context.Edi & 0xFFFF));
    case CV_REG_EAX:
        return NumVariant(m_context.Eax);
    case CV_REG_ECX:
        return NumVariant(m_context.Ecx);
    case CV_REG_EDX:
        return NumVariant(m_context.Edx);
    case CV_REG_EBX:
        return NumVariant(m_context.Ebx);
    case CV_REG_ESP:
        return NumVariant(m_context.Esp);
    case CV_REG_EBP:
        return NumVariant(m_context.Ebp);
    case CV_REG_ESI:
        return NumVariant(m_context.Esi);
    case CV_REG_EDI:
       return NumVariant(m_context.Edi);
    case CV_REG_ES:
       return NumVariant(m_context.SegEs);
    case CV_REG_CS:
       return NumVariant(m_context.SegCs);
    case CV_REG_SS:
       return NumVariant(m_context.SegSs);
    case CV_REG_DS:
       return NumVariant(m_context.SegDs);
    case CV_REG_FS:
        return NumVariant(m_context.SegFs);
    case CV_REG_GS:
        return NumVariant(m_context.SegGs);
    case CV_REG_IP:
        return NumVariant(static_cast<unsigned short>(m_context.Eip & 0xFFFF));
    case CV_REG_FLAGS:
        return NumVariant(static_cast<unsigned short>(m_context.EFlags & 0xFFFF));
    case CV_REG_EIP:
        return NumVariant(m_context.Eip);
    case CV_REG_EFLAGS:
        return NumVariant(m_context.EFlags);
    }

    std::stringstream sstr;
    sstr << "I386 context: unsupported register index " << std::dec << index;
    throw DbgException(sstr.str());
}

///////////////////////////////////////////////////////////////////////////////

std::wstring  CPUContextI386::getRegisterName(unsigned long index)
{
    switch (index)
    {
    case CV_REG_AL:
        return L"al";
    case CV_REG_CL:
        return L"cl";
    case CV_REG_DL:
        return L"dl";
    case CV_REG_BL:
        return L"bl";
    case CV_REG_AH:
        return L"ah";
    case CV_REG_CH:
        return L"ch";
    case CV_REG_DH:
        return L"dh";
    case CV_REG_BH:
        return L"bh";
    case CV_REG_AX:
        return L"ax";
    case CV_REG_CX:
        return L"cx";
    case CV_REG_DX:
        return L"dx";
    case CV_REG_BX:
        return L"bx";
    case CV_REG_SP:
        return L"sp";
    case CV_REG_BP:
        return L"bp";
    case CV_REG_SI:
        return L"si";
    case CV_REG_DI:
        return L"di";
    case CV_REG_EAX:
        return L"eax";
    case CV_REG_ECX:
        return L"ecx";
    case CV_REG_EDX:
        return L"edx";
    case CV_REG_EBX:
        return L"ebx";
    case CV_REG_ESP:
        return L"esp";
    case CV_REG_EBP:
        return L"ebp";
    case CV_REG_ESI:
        return L"esi";
    case CV_REG_EDI:
        return L"edi";
    case CV_REG_ES:
        return L"es";
    case CV_REG_CS:
        return L"cs";
    case CV_REG_SS:
        return L"ss";
    case CV_REG_DS:
        return L"ds";
    case CV_REG_FS:
        return L"fs";
    case CV_REG_GS:
        return L"gs";
    case CV_REG_IP:
        return L"ip";
    case CV_REG_FLAGS:
        return L"flags";
    case CV_REG_EIP:
        return L"eip";
    case CV_REG_EFLAGS:
        return L"eflags";
    }

    std::stringstream sstr;
    sstr << "I386 context: unsupported register index " << std::dec << index;
    throw DbgException(sstr.str());
}

///////////////////////////////////////////////////////////////////////////////

NumVariant CPUContextWOW64::getRegisterByIndex(unsigned long index)
{
    switch (index)
    {
    case CV_REG_AL:
        return NumVariant(static_cast<unsigned char>(m_context.Eax & 0xFF));
    case CV_REG_CL:
        return NumVariant(static_cast<unsigned char>(m_context.Ecx & 0xFF));
    case CV_REG_DL:
        return NumVariant(static_cast<unsigned char>(m_context.Edx & 0xFF));
    case CV_REG_BL:
        return NumVariant(static_cast<unsigned char>(m_context.Ebx & 0xFF));
    case CV_REG_AH:
        return NumVariant(static_cast<unsigned char>((m_context.Eax & 0xFF00) >> 8));
    case CV_REG_CH:
        return NumVariant(static_cast<unsigned char>((m_context.Ecx & 0xFF00) >> 8));
    case CV_REG_DH:
        return NumVariant(static_cast<unsigned char>((m_context.Edx & 0xFF00) >> 8));
    case CV_REG_BH:
        return NumVariant(static_cast<unsigned char>((m_context.Ebx & 0xFF00) >> 8));
    case CV_REG_AX:
        return NumVariant(static_cast<unsigned short>(m_context.Eax & 0xFFFF));
    case CV_REG_CX:
        return NumVariant(static_cast<unsigned short>(m_context.Ecx & 0xFFFF));
    case CV_REG_DX:
        return NumVariant(static_cast<unsigned short>(m_context.Edx & 0xFFFF));
    case CV_REG_BX:
        return NumVariant(static_cast<unsigned short>(m_context.Ebx & 0xFFFF));
    case CV_REG_SP:
        return NumVariant(static_cast<unsigned short>(m_context.Esp & 0xFFFF));
    case CV_REG_BP:
        return NumVariant(static_cast<unsigned short>(m_context.Ebp & 0xFFFF));
    case CV_REG_SI:
        return NumVariant(static_cast<unsigned short>(m_context.Esi & 0xFFFF));
    case CV_REG_DI:
        return NumVariant(static_cast<unsigned short>(m_context.Edi & 0xFFFF));
    case CV_REG_EAX:
        return NumVariant(m_context.Eax);
    case CV_REG_ECX:
        return NumVariant(m_context.Ecx);
    case CV_REG_EDX:
        return NumVariant(m_context.Edx);
    case CV_REG_EBX:
        return NumVariant(m_context.Ebx);
    case CV_REG_ESP:
        return NumVariant(m_context.Esp);
    case CV_REG_EBP:
        return NumVariant(m_context.Ebp);
    case CV_REG_ESI:
        return NumVariant(m_context.Esi);
    case CV_REG_EDI:
        return NumVariant(m_context.Edi);
    case CV_REG_ES:
        return NumVariant(m_context.SegEs);
    case CV_REG_CS:
        return NumVariant(m_context.SegCs);
    case CV_REG_SS:
        return NumVariant(m_context.SegSs);
    case CV_REG_DS:
        return NumVariant(m_context.SegDs);
    case CV_REG_FS:
        return NumVariant(m_context.SegFs);
    case CV_REG_GS:
        return NumVariant(m_context.SegGs);
    case CV_REG_IP:
        return NumVariant(static_cast<unsigned short>(m_context.Eip & 0xFFFF));
    case CV_REG_FLAGS:
        return NumVariant(static_cast<unsigned short>(m_context.EFlags & 0xFFFF));
    case CV_REG_EIP:
        return NumVariant(m_context.Eip);
    case CV_REG_EFLAGS:
        return NumVariant(m_context.EFlags);
    }

    std::stringstream sstr;
    sstr << "WOW64 context: unsupported register index " << std::dec << index;
    throw DbgException(sstr.str());
}

///////////////////////////////////////////////////////////////////////////////

std::wstring  CPUContextWOW64::getRegisterName(unsigned long index)
{
    switch (index)
    {
    case CV_REG_AL:
        return L"al";
    case CV_REG_CL:
        return L"cl";
    case CV_REG_DL:
        return L"dl";
    case CV_REG_BL:
        return L"bl";
    case CV_REG_AH:
        return L"ah";
    case CV_REG_CH:
        return L"ch";
    case CV_REG_DH:
        return L"dh";
    case CV_REG_BH:
        return L"bh";
    case CV_REG_AX:
        return L"ax";
    case CV_REG_CX:
        return L"cx";
    case CV_REG_DX:
        return L"dx";
    case CV_REG_BX:
        return L"bx";
    case CV_REG_SP:
        return L"sp";
    case CV_REG_BP:
        return L"bp";
    case CV_REG_SI:
        return L"si";
    case CV_REG_DI:
        return L"di";
    case CV_REG_EAX:
        return L"eax";
    case CV_REG_ECX:
        return L"ecx";
    case CV_REG_EDX:
        return L"edx";
    case CV_REG_EBX:
        return L"ebx";
    case CV_REG_ESP:
        return L"esp";
    case CV_REG_EBP:
        return L"ebp";
    case CV_REG_ESI:
        return L"esi";
    case CV_REG_EDI:
        return L"edi";
    case CV_REG_ES:
        return L"es";
    case CV_REG_CS:
        return L"cs";
    case CV_REG_SS:
        return L"ss";
    case CV_REG_DS:
        return L"ds";
    case CV_REG_FS:
        return L"fs";
    case CV_REG_GS:
        return L"gs";
    case CV_REG_IP:
        return L"ip";
    case CV_REG_FLAGS:
        return L"flags";
    case CV_REG_EIP:
        return L"eip";
    case CV_REG_EFLAGS:
        return L"eflags";
    }

    std::stringstream sstr;
    sstr << "I386 context: unsupported register index " << std::dec << index;
    throw DbgException(sstr.str());
}

///////////////////////////////////////////////////////////////////////////////

CPUContextAmd64::CPUContextAmd64()
{
    HRESULT  hres;

    hres = g_dbgMgr->advanced->GetThreadContext(&m_context, sizeof(m_context) );
    if ( FAILED(hres) )
        throw DbgEngException(L"IDebugAdvanced::GetThreadContext", hres);
}

///////////////////////////////////////////////////////////////////////////////

void CPUContextAmd64::restore()
{
    HRESULT  hres;

    hres = g_dbgMgr->advanced->SetThreadContext(&m_context, sizeof(m_context) );
    if ( FAILED(hres) )
        throw DbgEngException(L"IDebugAdvanced::GetThreadContext", hres);
}

///////////////////////////////////////////////////////////////////////////////

NumVariant CPUContextAmd64::getRegisterByIndex(unsigned long index)
{

    switch (index)
    {

    case CV_AMD64_AL:
        return NumVariant(static_cast<unsigned char>(m_context.Rax & 0xFF));
    case CV_AMD64_CL:
        return NumVariant(static_cast<unsigned char>(m_context.Rcx & 0xFF));
    case CV_AMD64_DL:
        return NumVariant(static_cast<unsigned char>(m_context.Rdx & 0xFF));
    case CV_AMD64_BL:
        return NumVariant(static_cast<unsigned char>(m_context.Rbx & 0xFF));
    case CV_AMD64_AH:
        return NumVariant(static_cast<unsigned char>((m_context.Rax & 0xFF00) >> 8));
    case CV_AMD64_CH:
        return NumVariant(static_cast<unsigned char>((m_context.Rcx & 0xFF00) >> 8));
    case CV_AMD64_DH:
        return NumVariant(static_cast<unsigned char>((m_context.Rdx & 0xFF00) >> 8));
    case CV_AMD64_BH:
        return NumVariant(static_cast<unsigned char>((m_context.Rbx & 0xFF00) >> 8));
    case CV_AMD64_AX:
        return NumVariant(static_cast<unsigned short>(m_context.Rax & 0xFFFF));
    case CV_AMD64_CX:
        return NumVariant(static_cast<unsigned short>(m_context.Rcx & 0xFFFF));
    case CV_AMD64_DX:
        return NumVariant(static_cast<unsigned short>(m_context.Rdx & 0xFFFF));
    case CV_AMD64_BX:
        return NumVariant(static_cast<unsigned short>(m_context.Rbx & 0xFFFF));
    case CV_AMD64_SP:
        return NumVariant(static_cast<unsigned short>(m_context.Rsp & 0xFFFF));
    case CV_AMD64_BP:
        return NumVariant(static_cast<unsigned short>(m_context.Rbp & 0xFFFF));
    case CV_AMD64_SI:
        return NumVariant(static_cast<unsigned short>(m_context.Rsi & 0xFFFF));
    case CV_AMD64_DI:
        return NumVariant(static_cast<unsigned short>(m_context.Rdi & 0xFFFF));
    case CV_AMD64_EAX:
        return NumVariant(static_cast<unsigned long>(m_context.Rax & 0xFFFFFFFF));
    case CV_AMD64_ECX:
        return NumVariant(static_cast<unsigned long>(m_context.Rcx & 0xFFFFFFFF));
    case CV_AMD64_EDX:
        return NumVariant(static_cast<unsigned long>(m_context.Rdx & 0xFFFFFFFF));
    case CV_AMD64_EBX:
        return NumVariant(static_cast<unsigned long>(m_context.Rbx & 0xFFFFFFFF));
    case CV_AMD64_ESP:
        return NumVariant(static_cast<unsigned long>(m_context.Rsp & 0xFFFFFFFF));
    case CV_AMD64_EBP:
        return NumVariant(static_cast<unsigned long>(m_context.Rbp & 0xFFFFFFFF));
    case CV_AMD64_ESI:
        return NumVariant(static_cast<unsigned long>(m_context.Rsi & 0xFFFFFFFF));
    case CV_AMD64_EDI:
        return NumVariant(static_cast<unsigned long>(m_context.Rdi & 0xFFFFFFFF));
    case CV_AMD64_ES:
        return NumVariant(m_context.SegEs);
    case CV_AMD64_CS:
        return NumVariant(m_context.SegCs);
    case CV_AMD64_SS:
        return NumVariant(m_context.SegSs);
    case CV_AMD64_DS:
        return NumVariant(m_context.SegDs);
    case CV_AMD64_FS:
        return NumVariant(m_context.SegFs);
    case CV_AMD64_GS:
        return NumVariant(m_context.SegGs);
    case CV_AMD64_FLAGS:
        return NumVariant(m_context.EFlags);
    case CV_AMD64_RIP:
        return NumVariant(m_context.Rip);
    case CV_AMD64_SIL:
        return NumVariant(static_cast<unsigned char>(m_context.Rsi & 0xFF));
    case CV_AMD64_DIL:
        return NumVariant(static_cast<unsigned char>(m_context.Rdi & 0xFF));
    case CV_AMD64_BPL:
        return NumVariant(static_cast<unsigned char>(m_context.Rbp & 0xFF));
    case CV_AMD64_SPL:
       return NumVariant(static_cast<unsigned char>(m_context.Rsp & 0xFF));
    case CV_AMD64_RAX:
        return NumVariant(m_context.Rax);
    case CV_AMD64_RBX:
        return NumVariant(m_context.Rbx);
    case  CV_AMD64_RDX:
        return NumVariant(m_context.Rdx);
    case CV_AMD64_RCX:
        return NumVariant(m_context.Rcx);
    case CV_AMD64_RSI:
        return NumVariant(m_context.Rsi);
    case CV_AMD64_RDI:
        return NumVariant(m_context.Rdi);
    case CV_AMD64_RBP:
        return NumVariant(m_context.Rbp);
    case CV_AMD64_RSP:
        return NumVariant(m_context.Rsp);
    case CV_AMD64_R8:
        return NumVariant(m_context.R8);
    case CV_AMD64_R9:
        return NumVariant(m_context.R9);
    case CV_AMD64_R10:
        return NumVariant(m_context.R10);
    case CV_AMD64_R11:
        return NumVariant(m_context.R11);
    case CV_AMD64_R12:
        return NumVariant(m_context.R12);
    case CV_AMD64_R13:
        return NumVariant(m_context.R13);
    case CV_AMD64_R14:
        return NumVariant(m_context.R14);
    case CV_AMD64_R15:
        return NumVariant(m_context.R15);
    case CV_AMD64_R8B:
        return NumVariant(static_cast<unsigned char>(m_context.R8 & 0xFF));
    case CV_AMD64_R9B:
        return NumVariant(static_cast<unsigned char>(m_context.R9 & 0xFF));
    case CV_AMD64_R10B:
        return NumVariant(static_cast<unsigned char>(m_context.R10 & 0xFF));
    case CV_AMD64_R11B:
        return NumVariant(static_cast<unsigned char>(m_context.R11 & 0xFF));
    case CV_AMD64_R12B:
        return NumVariant(static_cast<unsigned char>(m_context.R12 & 0xFF));
    case CV_AMD64_R13B:
        return NumVariant(static_cast<unsigned char>(m_context.R13 & 0xFF));
    case CV_AMD64_R14B:
        return NumVariant(static_cast<unsigned char>(m_context.R14 & 0xFF));
    case CV_AMD64_R15B:
        return NumVariant(static_cast<unsigned char>(m_context.R15 & 0xFF));
    case CV_AMD64_R8W:
        return NumVariant(static_cast<unsigned short>(m_context.R8 & 0xFFFF));
    case CV_AMD64_R9W:
        return NumVariant(static_cast<unsigned short>(m_context.R9 & 0xFFFF));
    case CV_AMD64_R10W:
        return NumVariant(static_cast<unsigned short>(m_context.R10 & 0xFFFF));
    case CV_AMD64_R11W:
        return NumVariant(static_cast<unsigned short>(m_context.R11 & 0xFFFF));
    case CV_AMD64_R12W:
        return NumVariant(static_cast<unsigned short>(m_context.R12 & 0xFFFF));
    case CV_AMD64_R13W:
        return NumVariant(static_cast<unsigned short>(m_context.R13 & 0xFFFF));
    case CV_AMD64_R14W:
        return NumVariant(static_cast<unsigned short>(m_context.R14 & 0xFFFF));
    case CV_AMD64_R15W:
        return NumVariant(static_cast<unsigned short>(m_context.R15 & 0xFFFF));
    case CV_AMD64_R8D:
        return NumVariant(static_cast<unsigned long>(m_context.R8 & 0xFFFFFFFF));
    case CV_AMD64_R9D:
        return NumVariant(static_cast<unsigned long>(m_context.R9 & 0xFFFFFFFF));
    case CV_AMD64_R10D:
        return NumVariant(static_cast<unsigned long>(m_context.R10 & 0xFFFFFFFF));
    case CV_AMD64_R11D:
        return NumVariant(static_cast<unsigned long>(m_context.R11 & 0xFFFFFFFF));
    case CV_AMD64_R12D:
        return NumVariant(static_cast<unsigned long>(m_context.R12 & 0xFFFFFFFF));
    case CV_AMD64_R13D:
        return NumVariant(static_cast<unsigned long>(m_context.R13 & 0xFFFFFFFF));
    case CV_AMD64_R14D:
        return NumVariant(static_cast<unsigned long>(m_context.R14 & 0xFFFFFFFF));
    case CV_AMD64_R15D:
        return NumVariant(static_cast<unsigned long>(m_context.R15 & 0xFFFFFFFF));
    }

    std::stringstream sstr;
    sstr << "AMD64 context: unsupported register index " << std::dec << index;
    throw DbgException(sstr.str());
}


std::wstring CPUContextAmd64::getRegisterName(unsigned long index)
{
    switch (index)
    {
    case CV_AMD64_AL:
        return L"al";
    case CV_AMD64_CL:
        return L"cl";
    case CV_AMD64_DL:
        return L"dl";
    case CV_AMD64_BL:
        return L"bl";
    case CV_AMD64_AH:
        return L"ah";
    case CV_AMD64_CH:
        return L"ch";
    case CV_AMD64_DH:
        return L"dh";
    case CV_AMD64_BH:
        return L"bh";
    case CV_AMD64_AX:
        return L"ax";
    case CV_AMD64_CX:
        return L"cx";
    case CV_AMD64_DX:
        return L"dx";
    case CV_AMD64_BX:
        return L"bx";
    case CV_AMD64_SP:
        return L"sp";
    case CV_AMD64_BP:
        return L"bp";
    case CV_AMD64_SI:
        return L"si";
    case CV_AMD64_DI:
        return L"di";
    case CV_AMD64_EAX:
        return L"eax";
    case CV_AMD64_ECX:
        return L"ecx";
    case CV_AMD64_EDX:
        return L"edx";
    case CV_AMD64_EBX:
        return L"ebx";
    case CV_AMD64_ESP:
        return L"esp";
    case CV_AMD64_EBP:
        return L"ebp";
    case CV_AMD64_ESI:
        return L"esi";
    case CV_AMD64_EDI:
        return L"edi";
    case CV_AMD64_ES:
        return L"es";
    case CV_AMD64_CS:
        return L"cs";
    case CV_AMD64_SS:
        return L"ss";
    case CV_AMD64_DS:
        return L"ds";
    case CV_AMD64_FS:
        return L"fs";
    case CV_AMD64_GS:
        return L"gs";
    case CV_AMD64_FLAGS:
        return L"eflags";
    case CV_AMD64_RIP:
        return L"rip";
    case CV_AMD64_SIL:
        return L"sil";
    case CV_AMD64_DIL:
        return L"dil";
    case CV_AMD64_BPL:
        return L"bpl";
    case CV_AMD64_SPL:
        return L"spl";
    case CV_AMD64_RAX:
        return L"rax";
    case CV_AMD64_RBX:
        return L"rbx";
    case  CV_AMD64_RDX:
        return L"rdx";
    case CV_AMD64_RCX:
        return L"rcx";
    case CV_AMD64_RSI:
        return L"rsi";
    case CV_AMD64_RDI:
        return L"rdi";
    case CV_AMD64_RBP:
        return L"rbp";
    case CV_AMD64_RSP:
        return L"rsp";
    case CV_AMD64_R8:
        return L"r8";
    case CV_AMD64_R9:
        return L"r9";
    case CV_AMD64_R10:
        return L"r10";
    case CV_AMD64_R11:
        return L"r11";
    case CV_AMD64_R12:
        return L"r12";
    case CV_AMD64_R13:
        return L"r13";
    case CV_AMD64_R14:
        return L"r14";
    case CV_AMD64_R15:
        return L"r15";
    case CV_AMD64_R8B:
        return L"r8b";
    case CV_AMD64_R9B:
        return L"r9b";
    case CV_AMD64_R10B:
        return L"r10b";
    case CV_AMD64_R11B:
        return L"r11b";
    case CV_AMD64_R12B:
        return L"r12b";
    case CV_AMD64_R13B:
        return L"r13b";
    case CV_AMD64_R14B:
        return L"r14b";
    case CV_AMD64_R15B:
        return L"r15b";
    case CV_AMD64_R8W:
        return L"r8w";
    case CV_AMD64_R9W:
        return L"r9w";
    case CV_AMD64_R10W:
        return L"r10w";
    case CV_AMD64_R11W:
        return L"r11w";
    case CV_AMD64_R12W:
        return L"r12w";
    case CV_AMD64_R13W:
        return L"r13w";
    case CV_AMD64_R14W:
        return L"r14w";
    case CV_AMD64_R15W:
        return L"r15w";
    case CV_AMD64_R8D:
        return L"r8d";
    case CV_AMD64_R9D:
        return L"r9d";
    case CV_AMD64_R10D:
        return L"r10d";
    case CV_AMD64_R11D:
        return L"r11d";
    case CV_AMD64_R12D:
        return L"r12d";
    case CV_AMD64_R13D:
        return L"r13d";
    case CV_AMD64_R14D:
        return L"r14d";
    case CV_AMD64_R15D:
        return L"r15d";
    }

    std::stringstream sstr;
    sstr << "AMD64 context: unsupported register index " << std::dec << index;
    throw DbgException(sstr.str());
}

///////////////////////////////////////////////////////////////////////////////


} // kdlib namespace end
