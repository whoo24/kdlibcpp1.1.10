#include "stdafx.h"

#include "kdlib\module.h"

#include "stackimpl.h"

namespace kdlib {

typedef std::pair<MEMOFFSET_64, MEMOFFSET_64>   DebugRange;

namespace {

///////////////////////////////////////////////////////////////////////////////

DebugRange getFuncDebugRange(SymbolPtr& sym)
{
    SymbolPtrList lstFuncDebugStart;
    SymbolPtrList lstFuncDebugEnd;

    try
    {
        lstFuncDebugStart = sym->findChildren(SymTagFuncDebugStart);
        if ( lstFuncDebugStart.empty() )
            return std::make_pair(0, 0);

        lstFuncDebugEnd = sym->findChildren(SymTagFuncDebugEnd);
        if ( lstFuncDebugEnd.empty() )
            return std::make_pair(0, 0);

        return std::make_pair( (*lstFuncDebugStart.begin())->getVa(), (*lstFuncDebugEnd.begin())->getVa());
    }
    catch (const SymbolException&)
    {
    }

    return std::make_pair(0, 0);
}

DebugRange getBlockRange(SymbolPtr& sym)
{
    MEMOFFSET_64   blockBegin = sym->getVa();
    MEMOFFSET_64   blockEnd = blockBegin + sym->getSize();

    return std::make_pair(blockBegin, blockEnd);
}

bool inDebugRange( const DebugRange& range, MEMOFFSET_64 offset)
{
    return range.first <= offset && range.second >= offset;
}

///////////////////////////////////////////////////////////////////////////////

}

/////////////////////////////////////////////////////////////////////////////////
//
//StackFramePtr getStackFrame( MEMOFFSET_64 &ip, MEMOFFSET_64 &ret, MEMOFFSET_64 &fp, MEMOFFSET_64 &sp )
//{
//    return StackFramePtr( new StackFrame(ip,ret,fp,sp) );
//}
//
/////////////////////////////////////////////////////////////////////////////////
//
//TypedVarPtr StackFrame::getFunction()
//{
//    ModulePtr mod = loadModule(m_ip);
//
//    return mod->getFunctionByAddr(m_ip);
//}
//
///////////////////////////////////////////////////////////////////////////////

unsigned long StackFrameImpl::getTypedParamCount()
{
    try {

        return static_cast<unsigned long>(getParams().size());

    }
    catch (DbgException&)
    {
    }

    return 0UL;
}

/////////////////////////////////////////////////////////////////////////////

TypedVarPtr StackFrameImpl::getTypedParam(unsigned long index)
{
    SymbolPtrList  vars = getParams();

    SymbolPtrList::iterator it = vars.begin();
    if (index >= vars.size())
        throw IndexException(index);

    std::advance(it, index);

    SymbolPtr  sym = *it;

    unsigned long  location = sym->getLocType();

    if (location == LocIsEnregistered)
    {
        unsigned long  regId = sym->getRegisterId();

        return loadTypedVar(loadType(sym), getRegisterAccessor(m_cpuContext->getRegisterName(regId)));
       // return loadTypedVar(loadType(sym), getVariantAccessor(m_cpuContext->getRegisterByIndex(regId)));
    }
    else if (location == LocIsRegRel)
    {
        MEMOFFSET_REL relOffset = sym->getOffset();

        RELREG_ID regRel = sym->getRegRealativeId();

        MEMOFFSET_64  offset = getOffset(regRel, relOffset);

        return loadTypedVar(loadType(sym), offset);
    }
    else if (location == LocIsNull)
    {
        return loadTypedVar(loadType(sym), 0);
    }

    throw DbgException("unknown variable storage");
}

/////////////////////////////////////////////////////////////////////////////

std::wstring  StackFrameImpl::getTypedParamName(unsigned long index)
{
    SymbolPtrList  vars = getParams();

    SymbolPtrList::iterator it = vars.begin();
    std::advance(it, index);

    if (it == vars.end())
        throw IndexException(index);

    return (*it)->getName();
}

/////////////////////////////////////////////////////////////////////////////

TypedVarPtr StackFrameImpl::getTypedParam(const std::wstring& paramName)
{
    SymbolPtrList  vars = getParams();

    SymbolPtrList::iterator it = vars.begin();
    for (; it != vars.end(); ++it)
    {
        if ((*it)->getName() == paramName)
        {
            SymbolPtr  sym = *it;

            unsigned long  location = sym->getLocType();

            if (location == LocIsEnregistered)
            {
                unsigned long  regId = sym->getRegisterId();

                return loadTypedVar(loadType(sym), getCacheAccessor(m_cpuContext->getRegisterByIndex(regId), L"@" + m_cpuContext->getRegisterName(regId)));
            }
            else if (location == LocIsRegRel)
            {
                MEMOFFSET_REL relOffset = sym->getOffset();

                RELREG_ID regRel = sym->getRegRealativeId();

                MEMOFFSET_64  offset = getOffset(regRel, relOffset);

                return loadTypedVar(loadType(sym), offset);
            }
            else if (location == LocIsNull)
            {
                return loadTypedVar(loadType(sym), 0);
            }

            throw DbgException("unknown variable storage");
        }
    }

    std::wstringstream  sstr;
    sstr << L'\'' << paramName << L'\'' << L" - function's parameter not found";
    throw SymbolException(sstr.str());

}

/////////////////////////////////////////////////////////////////////////////

bool  StackFrameImpl::findParam(const std::wstring& paramName)
{
    SymbolPtrList  vars = getParams();

    SymbolPtrList::iterator it = vars.begin();
    for (; it != vars.end(); ++it)
    {
        if ((*it)->getName() == paramName)
            return true;
    }
    
    return false;
}

/////////////////////////////////////////////////////////////////////////////

unsigned long StackFrameImpl::getLocalVarCount()
{
    try
    {
        return static_cast<unsigned long>(getLocalVars().size());
    }
    catch (DbgException&)
    {
    }

    return 0UL;
}

/////////////////////////////////////////////////////////////////////////////

TypedVarPtr StackFrameImpl::getLocalVar(unsigned long index)
{
    SymbolPtrList  vars = getLocalVars();

    SymbolPtrList::iterator it = vars.begin();
    if (index >= vars.size())
        throw IndexException(index);

    std::advance( it, index );

    SymbolPtr  sym = *it;

    unsigned long  location = sym->getLocType();

    if (location  == LocIsEnregistered)
    {
        unsigned long  regId = sym->getRegisterId();

        return loadTypedVar(loadType(sym), getRegisterAccessor(m_cpuContext->getRegisterName(regId)));

    }
    else if (location == LocIsRegRel)
    {
        MEMOFFSET_REL relOffset = sym->getOffset();

        RELREG_ID regRel = sym->getRegRealativeId();

        MEMOFFSET_64  offset = getOffset(regRel, relOffset);

        return loadTypedVar(loadType(sym), offset);
    }
    else if (location == LocIsNull)
    {
        return loadTypedVar(loadType(sym), 0);
    }

    throw DbgException("unknown variable storage");
}

/////////////////////////////////////////////////////////////////////////////

std::wstring  StackFrameImpl::getLocalVarName(unsigned long index)
{
    SymbolPtrList  vars = getLocalVars();

    SymbolPtrList::iterator it = vars.begin();
    std::advance( it, index );

    if ( it == vars.end() )
        throw IndexException(index);

    return (*it)->getName();
}

/////////////////////////////////////////////////////////////////////////////

TypedVarPtr StackFrameImpl::getLocalVar(const std::wstring& paramName)
{
    SymbolPtrList  vars = getLocalVars();

    SymbolPtrList::iterator it = vars.begin();
    for ( ; it != vars.end(); ++it )
    {
        if ( (*it)->getName() == paramName )
        {
            SymbolPtr  sym = *it;

            unsigned long  location = sym->getLocType();

            if (location == LocIsEnregistered)
            {
                unsigned long  regId = sym->getRegisterId();

                return loadTypedVar(loadType(sym), getCacheAccessor(m_cpuContext->getRegisterByIndex(regId), L"@" + m_cpuContext->getRegisterName(regId)));
            }
            else if (location == LocIsRegRel)
            {
                MEMOFFSET_REL relOffset = sym->getOffset();

                RELREG_ID regRel = sym->getRegRealativeId();

                MEMOFFSET_64  offset = getOffset(regRel, relOffset);

                return loadTypedVar(loadType(sym), offset);
            }
            else if (location == LocIsNull)
            {
                return loadTypedVar(loadType(sym), 0);
            }

            throw DbgException("unknown variable storage");
        }
    }

    std::wstringstream  sstr;
    sstr << L'\'' << paramName << L'\'' << L" - local variable not found";
    throw SymbolException(sstr.str());
}

/////////////////////////////////////////////////////////////////////////////

bool StackFrameImpl::findLocalVar(const std::wstring& varName)
{
    SymbolPtrList  vars = getLocalVars();

    SymbolPtrList::iterator it = vars.begin();
    for (; it != vars.end(); ++it)
    {
        if ((*it)->getName() == varName)
            return true;
    }

    return false;
}

/////////////////////////////////////////////////////////////////////////////

unsigned long StackFrameImpl::getStaticVarCount()
{
    try 
    {
        return static_cast<unsigned long>(getStaticVars().size());
    }
    catch (DbgException&)
    {
    }

    return 0UL;
}

/////////////////////////////////////////////////////////////////////////////

TypedVarPtr StackFrameImpl::getStaticVar(unsigned long index)
{
    SymbolPtrList  vars = getStaticVars();

    SymbolPtrList::iterator it = vars.begin();
    if (index >= vars.size())
        throw IndexException(index);

    std::advance(it, index);

    SymbolPtr  sym = *it;

    unsigned long  location = sym->getLocType();

    if (location == LocIsStatic)
    {
        MEMOFFSET_64  offset = sym->getVa();
        return loadTypedVar(loadType(sym), offset);
    }

    throw DbgException("unknown variable storage");
}

/////////////////////////////////////////////////////////////////////////////

TypedVarPtr StackFrameImpl::getStaticVar(const std::wstring& paramName)
{
    SymbolPtrList  vars = getStaticVars();

    SymbolPtrList::iterator it = vars.begin();
    for (; it != vars.end(); ++it)
    {
        if ((*it)->getName() == paramName)
        {
            SymbolPtr  sym = *it;

            unsigned long  location = sym->getLocType();

            if (location == LocIsStatic)
            {
                MEMOFFSET_64  offset = sym->getVa();
                return loadTypedVar(loadType(sym), offset);
            }

            throw DbgException("unknown variable storage");
        }
    }

    std::wstringstream  sstr;
    sstr << L'\'' << paramName << L'\'' << L" - static local variable not found";
    throw SymbolException(sstr.str());
}

/////////////////////////////////////////////////////////////////////////////

std::wstring  StackFrameImpl::getStaticVarName(unsigned long index)
{
    SymbolPtrList  vars = getStaticVars();

    SymbolPtrList::iterator it = vars.begin();
    std::advance(it, index);

    if (it == vars.end())
        throw IndexException(index);

    return (*it)->getName();
}

/////////////////////////////////////////////////////////////////////////////

bool StackFrameImpl::findStaticVar(const std::wstring& varName)
{
    SymbolPtrList  vars = getLocalVars();

    SymbolPtrList::iterator it = vars.begin();
    for (; it != vars.end(); ++it)
    {
        if ((*it)->getName() == varName)
            return true;
    }

    return false;
}

/////////////////////////////////////////////////////////////////////////////

SymbolPtrList  StackFrameImpl::getLocalVars()
{

    SymbolPtrList  lst;

    try {

        ModulePtr mod  = loadModule(m_ip);

        MEMDISPLACEMENT displacemnt;
        SymbolPtr symFunc = mod->getSymbolByVa( m_ip, SymTagFunction, &displacemnt );



        DebugRange  debugRange = getFuncDebugRange(symFunc);

        if ( !inDebugRange(debugRange, m_ip) )
            return lst;

        //find var in current scope
        SymbolPtrList symList = symFunc->findChildrenByRVA(SymTagData, static_cast<MEMOFFSET_32>(m_ip - mod->getBase()));

        SymbolPtrList::iterator it;
        for ( it = symList.begin(); it != symList.end(); it++ )
        {
            if ( (*it)->getDataKind() == DataIsLocal  )
                lst.push_back( *it );
        }

        //find inners scopes
        SymbolPtrList scopeList = symFunc->findChildren(SymTagBlock);
        SymbolPtrList::iterator itScope = scopeList.begin();

        for (; itScope != scopeList.end(); ++itScope)
        {
            SymbolPtrList  innerVars = getBlockLocalVars(*itScope);
 
            lst.insert( lst.end(), innerVars.begin(), innerVars.end() );
        }

    }
    catch(SymbolException&)
    {
    }


    return lst;
}

/////////////////////////////////////////////////////////////////////////////

SymbolPtrList StackFrameImpl::getParams()
{
    SymbolPtrList  lst;

    try 
    {

        ModulePtr mod = loadModule(m_ip);

        MEMDISPLACEMENT displacemnt;
        SymbolPtr symFunc = mod->getSymbolByVa(m_ip, SymTagFunction, &displacemnt);


        // find var in current scope
        SymbolPtrList symList = symFunc->findChildrenByRVA(SymTagData, static_cast<MEMOFFSET_32>(m_ip - mod->getBase()));

        SymbolPtrList::iterator it;
        for (it = symList.begin(); it != symList.end(); it++)
        {
            unsigned long dataKind = (*it)->getDataKind();

            if ( dataKind == DataIsParam || dataKind == DataIsObjectPtr )
                lst.push_back(*it);
        }

    }
    catch(SymbolException&)
    {
    }

    return lst;
}

/////////////////////////////////////////////////////////////////////////////

SymbolPtrList StackFrameImpl::getStaticVars()
{
    ModulePtr mod = loadModule(m_ip);

    MEMDISPLACEMENT displacemnt;
    SymbolPtr symFunc = mod->getSymbolByVa(m_ip, SymTagFunction, &displacemnt);

    SymbolPtrList  lst;

    DebugRange  debugRange = getFuncDebugRange(symFunc);

    if (!inDebugRange(debugRange, m_ip))
        return lst;

    //find var in current scope
    SymbolPtrList symList = symFunc->findChildrenByRVA(SymTagData, static_cast<MEMOFFSET_32>(m_ip - mod->getBase()));

    SymbolPtrList::iterator it;
    for (it = symList.begin(); it != symList.end(); it++)
    {
        if ((*it)->getDataKind() == DataIsStaticLocal)
        {
            std::wstring  name = (*it)->getName();
            if (!name.empty())
                lst.push_back(*it);
        }
    }

    //find inners scopes
    SymbolPtrList scopeList = symFunc->findChildren(SymTagBlock);
    SymbolPtrList::iterator itScope = scopeList.begin();

    for (; itScope != scopeList.end(); ++itScope)
    {
        SymbolPtrList  innerVars = getBlockStaticVars(*itScope);

        lst.insert(lst.end(), innerVars.begin(), innerVars.end());
    }

    return lst;
}

/////////////////////////////////////////////////////////////////////////////

SymbolPtrList  StackFrameImpl::getBlockLocalVars(SymbolPtr& sym)
{
    SymbolPtrList  lst;

    DebugRange  debugRange = getBlockRange(sym);

    if ( !inDebugRange(debugRange, m_ip) )
        return lst;

    // find var in current scope
    SymbolPtrList symList = sym->findChildren(SymTagData);

    SymbolPtrList::iterator it;
    for ( it = symList.begin(); it != symList.end(); it++ )
    {
        if ( (*it)->getDataKind() == DataIsLocal  )
            lst.push_back( *it );
    }

    // find inners scopes
    SymbolPtrList scopeList = sym->findChildren(SymTagBlock);
    SymbolPtrList::iterator itScope = scopeList.begin();

    for (; itScope != scopeList.end(); ++itScope)
    {
        SymbolPtrList  innerVars = getBlockLocalVars(*itScope);

        lst.insert( lst.end(), innerVars.begin(), innerVars.end() );
    }

    return lst;
}

/////////////////////////////////////////////////////////////////////////////

SymbolPtrList  StackFrameImpl::getBlockStaticVars(SymbolPtr& sym)
{
    SymbolPtrList  lst;

    DebugRange  debugRange = getBlockRange(sym);

    if (!inDebugRange(debugRange, m_ip))
        return lst;

    // find var in current scope
    SymbolPtrList symList = sym->findChildren(SymTagData);

    SymbolPtrList::iterator it;
    for (it = symList.begin(); it != symList.end(); it++)
    {
        if ((*it)->getDataKind() == DataIsStaticLocal)
        {
            std::wstring  name = (*it)->getName();
            if (!name.empty())
                lst.push_back(*it);
        }
    }

    // find inners scopes
    SymbolPtrList scopeList = sym->findChildren(SymTagBlock);
    SymbolPtrList::iterator itScope = scopeList.begin();

    for (; itScope != scopeList.end(); ++itScope)
    {
        SymbolPtrList  innerVars = getBlockStaticVars(*itScope);

        lst.insert(lst.end(), innerVars.begin(), innerVars.end());
    }

    return lst;
}

/////////////////////////////////////////////////////////////////////////////

MEMOFFSET_64 StackFrameImpl::getOffset(unsigned long regRel, MEMOFFSET_REL relOffset)
{
    switch( regRel )
    {
    case rriInstructionPointer:
        return (MEMOFFSET_64)( m_ip + relOffset );

    case rriStackFrame:
        return (MEMOFFSET_64)( m_fp + relOffset );

    case rriStackPointer:
        return (MEMOFFSET_64)( m_sp + relOffset );
    }

    throw DbgException( "unknown relative offset" );
}

/////////////////////////////////////////////////////////////////////////////


}; // kdlib namespace end
