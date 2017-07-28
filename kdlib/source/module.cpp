#include "stdafx.h"

#include <set>
#include <boost/regex.hpp>

#include "kdlib/memaccess.h"
#include "kdlib/exceptions.h"

#include "moduleimp.h"
#include "processmon.h"
#include "typeinfoimp.h"

namespace kdlib {


///////////////////////////////////////////////////////////////////////////////

ModulePtr loadModule( const std::wstring &name )
{
    return ModuleImp::getModule(name);
}

///////////////////////////////////////////////////////////////////////////////

ModulePtr loadModule( MEMOFFSET_64 offset )
{
    return ModuleImp::getModule(offset);
}

///////////////////////////////////////////////////////////////////////////////

ModulePtr ModuleImp::getModule( const std::wstring &name )
{
    return getModule( findModuleBase(name) );
}

///////////////////////////////////////////////////////////////////////////////

ModulePtr ModuleImp::getModule( MEMOFFSET_64 offset )
{
   
    ModulePtr  module = ProcessMonitor::getModule(offset);

    if ( !module )
    {
        module = ModulePtr( new ModuleImp(offset) );
        ProcessMonitor::insertModule(module);
    }

    return module;
}

///////////////////////////////////////////////////////////////////////////////

ModuleImp::ModuleImp(const std::wstring &moduleName )
{
    m_base = findModuleBase( moduleName );
    m_name = moduleName;
    m_noSymbols = true;
    fillFields();
}

///////////////////////////////////////////////////////////////////////////////

ModuleImp::ModuleImp(MEMOFFSET_64 offset )
{
    m_base = findModuleBase( addr64(offset) );
    m_name = getModuleName( m_base );
    m_noSymbols = true;
    fillFields();
}

///////////////////////////////////////////////////////////////////////////////

void ModuleImp::fillFields()
{
    m_imageName = getModuleImageName( m_base );
    m_timeDataStamp = getModuleTimeStamp( m_base );
    m_checkSum = getModuleCheckSum( m_base );
    m_size = getModuleSize( m_base );
    m_isUnloaded = isModuleUnloaded( m_base );
    m_isUserMode = isModuleUserMode( m_base );
}

///////////////////////////////////////////////////////////////////////////////

MEMOFFSET_64 ModuleImp::getSymbolVa( const std::wstring& symbolName )
{
    return m_base + getSymbolRva(symbolName);
}

///////////////////////////////////////////////////////////////////////////////

MEMOFFSET_32 ModuleImp::getSymbolRva( const std::wstring& symbolName )
{
    try
    {
        return getSymbolScope()->getChildByName(symbolName)->getRva();
    }
    catch(const SymbolException &)
    {
    }

    std::vector< SyntheticSymbol > ids;
    try
    {
        getSyntheticSymbols(getName() + L"!" + symbolName, ids);
    }
    catch(const DbgException &)
    {
    }
    for (const auto &id : ids)
    {
        try
        {
            MEMOFFSET_64 offset;
            getSyntheticSymbolInformation(id, nullptr, &offset);

            if (inRange(offset))
                return static_cast<MEMOFFSET_32>(offset - m_base);
        }
        catch(const DbgException &)
        {
        }
    }

    throw SymbolException( symbolName + L" symbol is not found");
}

///////////////////////////////////////////////////////////////////////////////

SymbolPtr ModuleImp::getSymbolScope()
{
    return getSymSession()->getSymbolScope();
}


///////////////////////////////////////////////////////////////////////////////

SymbolSessionPtr& ModuleImp::getSymSession()
{
    if (m_symSession)
        return m_symSession;

    m_exportSymbols = false;
    m_noSymbols = false;


    try
    {
        m_symSession = loadSymbolFile( m_base, m_imageName);
        if (m_symSession)
        {
            return m_symSession;
        }
    }
    catch(const SymbolException &)
    {}

    try
    {
        std::wstring symfile = getModuleSymbolFileName(m_base);
        if (!symfile.empty() )
        {
            m_symSession = loadSymbolFile(symfile, m_base);
        }

        if (m_symSession)
        {
            return m_symSession;
        }
    }
    catch(const DbgException&)
    {}

    try
    {
        m_symSession = loadSymbolFromExports(m_base);
        if (m_symSession)
        {
            m_exportSymbols = true;
            return m_symSession;
        }
    }
    catch(const DbgException&)
    {}

    m_noSymbols = true;
    m_symSession = loadNoSymbolSession();

    return m_symSession;
}

///////////////////////////////////////////////////////////////////////////////

std::wstring ModuleImp::getSymFile()
{
    try {
       
        return getSymSession()->getSymbolFileName();

    } 
    catch( const SymbolException& )
    {}

    return std::wstring();
}

///////////////////////////////////////////////////////////////////////////////

void ModuleImp::reloadSymbols()
{
    m_symSession.reset();
    getSymSession();
}

///////////////////////////////////////////////////////////////////////////////

SymbolPtr ModuleImp::getSymbolByVa( MEMOFFSET_64 offset,  MEMDISPLACEMENT* displacemnt )
{
    return getSymbolByVa( offset, SymTagNull, displacemnt );
}

///////////////////////////////////////////////////////////////////////////////

SymbolPtr ModuleImp::getSymbolByVa( MEMOFFSET_64 offset, unsigned long  symTag, MEMDISPLACEMENT* displacement )
{
    offset = addr64(offset);

    if ( offset < m_base || offset > getEnd() )
        throw DbgException( "address is out of the module space" );

    return getSymbolByRva( (MEMOFFSET_32)(offset - m_base ), symTag, displacement );
}

///////////////////////////////////////////////////////////////////////////////

SymbolPtr ModuleImp::getSymbolByRva( MEMOFFSET_32 rva, MEMDISPLACEMENT* displacemnt  )
{
   return getSymbolByRva( rva, SymTagNull, displacemnt );
}

///////////////////////////////////////////////////////////////////////////////

SymbolPtr ModuleImp::getSymbolByRva( MEMOFFSET_32 rva, unsigned long symTag, MEMDISPLACEMENT* displacement )
{
   return getSymSession()->findByRva( rva, symTag, displacement );
} 

///////////////////////////////////////////////////////////////////////////////

SymbolPtr ModuleImp::getSymbolByName( const std::wstring &symbolName )
{ 
    return getSymbolScope()->getChildByName( symbolName );
}

///////////////////////////////////////////////////////////////////////////////

size_t ModuleImp::getSymbolSize( const std::wstring &symName )
{
    TypeInfoPtr typeInfo = getTypeByName( symName );

    return typeInfo->getSize();
}

///////////////////////////////////////////////////////////////////////////////

SymbolOffsetList ModuleImp::enumSymbols( const std::wstring  &mask )
{
    SymbolOffsetList offsetLst;

    std::set< std::wstring > nameSet;

    SymbolPtrList  symlst = getSymbolScope()->findChildren( SymTagData, mask );

    for ( SymbolPtrList::iterator it = symlst.begin(); it != symlst.end(); ++it )
    {
        if ( (*it)->getDataKind() != DataIsConstant && (*it)->getLocType() != LocIsTLS )
        {
            offsetLst.push_back( SymbolOffset( (*it)->getName(), (*it)->getVa() ) );
            nameSet.insert((*it)->getName());
        }
    }

    symlst = getSymbolScope()->findChildren( SymTagFunction, mask );

    for ( SymbolPtrList::iterator it = symlst.begin(); it != symlst.end(); ++it )
    {
        offsetLst.push_back( SymbolOffset( (*it)->getName(), (*it)->getVa() ) );
        nameSet.insert((*it)->getName());
    }

    // add only "unique" public symbols
    symlst = getSymbolScope()->findChildren( SymTagPublicSymbol, mask );
    for ( SymbolPtrList::iterator it = symlst.begin(); it != symlst.end(); ++it )
    {
        std::set< std::wstring >::const_iterator searchRes = nameSet.find( (*it)->getName() );
        if (searchRes == nameSet.end() )
            offsetLst.push_back( SymbolOffset( (*it)->getName(), (*it)->getVa() ) );
    }

    return offsetLst;
}

///////////////////////////////////////////////////////////////////////////////

TypeNameList ModuleImp::enumTypes(const std::wstring& mask)
{
	TypeNameList  lst;

	SymbolPtrList  symlst = getSymbolScope()->findChildren(SymTagUDT, mask);

	for (SymbolPtrList::iterator it = symlst.begin(); it != symlst.end(); ++it)
	{
		lst.push_back((*it)->getName());
	}

	symlst = getSymbolScope()->findChildren(SymTagEnum, mask);

	for (SymbolPtrList::iterator it = symlst.begin(); it != symlst.end(); ++it)
	{
		lst.push_back((*it)->getName());
	}

    symlst = getSymbolScope()->findChildren(SymTagFunctionType, mask);

	for (SymbolPtrList::iterator it = symlst.begin(); it != symlst.end(); ++it)
	{
		lst.push_back((*it)->getName());
	}

	lst.sort();
	lst.unique();

	return lst;
}

///////////////////////////////////////////////////////////////////////////////

std::wstring ModuleImp::findSymbol( MEMOFFSET_64 offset, MEMDISPLACEMENT &displacement )
{
    if ( !inRange(offset) )
        throw SymbolException(L"offset dont has to module");

    struct
    {
        std::wstring name;
        MEMDISPLACEMENT displacement;
    } symSessionSymbol;
    findSymSessionSymbol(offset, symSessionSymbol.name, symSessionSymbol.displacement);

    std::vector< SyntheticSymbol > ids;
    try
    {
        getNearSyntheticSymbols(offset, ids);
    }
    catch(const DbgException &)
    {
    }

    if (!ids.empty())
    {
        for (const auto &id : ids)
        {
            struct
            {
                std::wstring name;
                MEMOFFSET_64 offset;
                unsigned long size;
            } dbgEngineSymbol;
            try
            {
                getSyntheticSymbolInformation(
                    id,
                    &dbgEngineSymbol.name,
                    &dbgEngineSymbol.offset,
                    &dbgEngineSymbol.size);
            }
            catch(const DbgException &)
            {
            }

            if ( !dbgEngineSymbol.name.empty() && 
                 inRange(dbgEngineSymbol.offset) && 
                 inRange(dbgEngineSymbol.offset + dbgEngineSymbol.size) )
            {
                // select "best" symbol (minimal displacement)
                displacement = (MEMDISPLACEMENT)((long long)offset - (long long)dbgEngineSymbol.offset);
                if ( !symSessionSymbol.name.empty() && 
                    (abs(symSessionSymbol.displacement) <= abs(displacement)) )
                {
                    displacement = symSessionSymbol.displacement;
                    return symSessionSymbol.name;
                }
                return dbgEngineSymbol.name;
            }
        }
    }

    if ( !symSessionSymbol.name.empty() )
    {
        displacement = symSessionSymbol.displacement;
        return symSessionSymbol.name;
    }

    throw SymbolException(L"symbol not found");
}

///////////////////////////////////////////////////////////////////////////////

TypedVarPtr ModuleImp::getTypedVarByAddr( MEMOFFSET_64 offset )
{
    offset = addr64(offset);

    if ( !inRange(offset) )
        throw SymbolException(L"offset dont has to module");

    SymbolPtr symVar = getSymSession()->findByRva( (MEMOFFSET_32)(offset - m_base ) );
    return loadTypedVar( symVar );
}

///////////////////////////////////////////////////////////////////////////////

TypedVarPtr ModuleImp::getTypedVarByName( const std::wstring &symName )
{
    SymbolPtr  symVar = getSymbolScope()->getChildByName( symName );

    return loadTypedVar( symVar );
}

///////////////////////////////////////////////////////////////////////////////

TypedVarPtr ModuleImp::getTypedVarWithPrototype( const std::wstring &symName, const std::wstring &prototype)
{

    SymbolPtrList  functions = getSymbolScope()->findChildren(SymTagFunction);

    for ( SymbolPtrList::iterator  it = functions.begin(); it != functions.end(); ++it )
    {
        SymbolPtr  sym = *it;
        if ( sym->getName() == symName  )
        {
            TypeInfoPtr  funcType = loadType(sym->getType());
            if ( isPrototypeMatch( funcType, prototype ) )
                return loadTypedVar(sym);
        }
    }

    throw TypeException(L"failed to find symbol");
}

///////////////////////////////////////////////////////////////////////////////

TypedVarPtr ModuleImp::getTypedVarByTypeName( const std::wstring &typeName, MEMOFFSET_64 offset )
{
    TypeInfoPtr typeInfo = getTypeByName( typeName );

    return loadTypedVar( typeInfo, offset );
}

///////////////////////////////////////////////////////////////////////////////

TypedVarPtr ModuleImp::containingRecord( MEMOFFSET_64 offset, const std::wstring &typeName,  const std::wstring &fieldName )
{
    offset = addr64(offset);

    TypeInfoPtr typeInfo = getTypeByName( typeName );

    return kdlib::containingRecord( offset, typeInfo, fieldName );
}

///////////////////////////////////////////////////////////////////////////////

TypedVarPtr ModuleImp::getFunctionByAddr( MEMOFFSET_64 offset )
{
    offset = addr64(offset);

    MEMDISPLACEMENT  displacement;
    SymbolPtr  symVar = getSymSession()->findByRva( (MEMDISPLACEMENT)(offset - m_base ), SymTagFunction, &displacement );

    if ( !symVar || displacement < 0 )
         throw SymbolException(L"failed to find function symbols by address");

    size_t  funcLength = symVar->getSize();

    if ( static_cast<size_t>(displacement) > funcLength )
         throw SymbolException(L"failed to find function symbols by address");

    return  loadTypedVar( symVar );
}

///////////////////////////////////////////////////////////////////////////////

TypedVarList ModuleImp::loadTypedVarList( MEMOFFSET_64 offset, const std::wstring &typeName, const std::wstring &fieldName )
{
    offset = addr64(offset);

    TypeInfoPtr typeInfo = getTypeByName( typeName );

    return kdlib::loadTypedVarList( offset, typeInfo, fieldName );
}

///////////////////////////////////////////////////////////////////////////////

TypedVarList ModuleImp::loadTypedVarArray( MEMOFFSET_64 offset, const std::wstring &typeName, size_t count )
{
    offset = addr64(offset);

    TypeInfoPtr typeInfo = getTypeByName( typeName );

    return kdlib::loadTypedVarArray( offset, typeInfo, count );
}

///////////////////////////////////////////////////////////////////////////////

std::wstring ModuleImp::getSourceFile( MEMOFFSET_64 offset )
{
    std::wstring  fileName;
    unsigned long  lineno;
    long  displacement;

    offset = addr64(offset);

    getSourceLine( offset, fileName, lineno, displacement );

    return fileName;
}

///////////////////////////////////////////////////////////////////////////////

std::wstring  ModuleImp::getSourceFileFromSrcSrv(MEMOFFSET_64 offset)
{
    std::wstring  fileName;
    std::wstring  loadedFileName;
    unsigned long  lineno;
    long  displacement;

    offset = addr64(offset);

    getSourceLine(offset, fileName, lineno, displacement);

    loadedFileName = loadSourceFileFromSrcSrv(m_base, fileName);

    return loadedFileName;
}

///////////////////////////////////////////////////////////////////////////////

void ModuleImp::getSourceLine( MEMOFFSET_64 offset, std::wstring &fileName, unsigned long &lineno, long &displacement )
{
    offset = addr64(offset);

    if ( !inRange(offset) )
        throw SymbolException(L"offset dont has to module");

    getSymSession()->getSourceLine( offset, fileName, lineno, displacement );
}

///////////////////////////////////////////////////////////////////////////////

std::string ModuleImp::getVersionInfo( const std::string &value )
{
    return getModuleVersionInfo( m_base, value );
}

///////////////////////////////////////////////////////////////////////////////

void ModuleImp::getFixedFileInfo( FixedFileInfo &fixedFileInfo )
{
    return getModuleFixedFileInfo( m_base, fixedFileInfo );
}

///////////////////////////////////////////////////////////////////////////////

void ModuleImp::getFileVersion(unsigned long& majorVersion, unsigned long& minorVerion, unsigned long& revision, unsigned long& build)
{
    FixedFileInfo  fileInfo = {};

    getModuleFixedFileInfo( m_base, fileInfo);

    majorVersion = (fileInfo.FileVersionMS >> 16) & 0xFFFF;
    minorVerion = (fileInfo.FileVersionMS) & 0xFFFF;
    revision = (fileInfo.FileVersionLS >> 16) & 0xFFFF;
    build = (fileInfo.FileVersionLS) & 0xFFFF;
}

///////////////////////////////////////////////////////////////////////////////

void ModuleImp::findSymSessionSymbol(MEMOFFSET_64 offset, std::wstring &name, MEMDISPLACEMENT &displacement)
{
    displacement = 0;
    offset = addr64(offset);
    try
    {
        while ( name.empty() )
        {
            SymbolPtr sym = getSymSession()->findByRva( (MEMDISPLACEMENT)(offset - m_base), SymTagNull, &displacement );
            name = sym->getName();
            if ( !name.empty() )
                break;

            offset = offset - displacement - 1;
        }
    }
    catch (const DbgException &)
    {
        name.clear();
    }
}

///////////////////////////////////////////////////////////////////////////////

MEMOFFSET_64 findModuleBySymbol( const std::wstring &symbolName )
{
    std::vector<MEMOFFSET_64>   moduleList = getModuleBasesList();

    std::vector<MEMOFFSET_64>::const_iterator it;
    for ( it = moduleList.begin(); it != moduleList.end(); ++it )
    {
        ModulePtr  module = loadModule( *it );

        try {

            SymbolPtr typeInfo = module->getSymbolByName( symbolName );
            return *it;
        } 
        catch( SymbolException& )
        {}

        try {

            TypeInfoPtr typeInfo = module->getTypeByName( symbolName );
            return *it;
        } 
        catch( SymbolException& )
        {}
    }

    std::wstringstream   sstr;
    sstr << L"failed to find module for symbol: " << symbolName;
    throw SymbolException( sstr.str() );
}

///////////////////////////////////////////////////////////////////////////////

std::wstring getSourceFile( MEMOFFSET_64 offset )
{
    offset = addr64(offset);

    if ( offset == 0 )
        offset = getInstructionOffset();

    ModulePtr module = loadModule( offset );
    
    return module->getSourceFile( offset );
}

///////////////////////////////////////////////////////////////////////////////

std::wstring getSourceFileFromSrcSrv(MEMOFFSET_64 offset)
{
    offset = addr64(offset);

    if ( offset == 0 )
        offset = getInstructionOffset();

    ModulePtr module = loadModule(offset);

    return module->getSourceFileFromSrcSrv(offset);
}

///////////////////////////////////////////////////////////////////////////////

void getSourceLine( std::wstring &fileName, unsigned long &lineno, long &displacement, MEMOFFSET_64 offset )
{
    offset = addr64(offset);

    if ( offset == 0 )
        offset = getInstructionOffset();

    ModulePtr module = loadModule( offset );
    
    module->getSourceLine( offset, fileName, lineno, displacement );
}

///////////////////////////////////////////////////////////////////////////////

static const boost::wregex moduleSymMatch(L"^(?:([^!]*)!)?([^!]+)$"); 

void splitSymName( const std::wstring &fullName, std::wstring &moduleName, std::wstring &symbolName )
{
    boost::wsmatch   matchResult;

    if ( !boost::regex_match( fullName, matchResult, moduleSymMatch ) )
    {
        std::wstringstream   sstr;
        sstr << L"invalid symbol name: " << fullName;
        throw kdlib::SymbolException( sstr.str() );
    }

    symbolName = std::wstring( matchResult[2].first, matchResult[2].second );

    if ( matchResult[1].matched )
    {
        moduleName = std::wstring( matchResult[1].first, matchResult[1].second );
    }
    else
    {
        moduleName = L"";
    }
}

///////////////////////////////////////////////////////////////////////////////

}; // kdlib namespace end
