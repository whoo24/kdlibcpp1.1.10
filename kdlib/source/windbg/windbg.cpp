#include "stdafx.h"

#include <vector>

#include <DbgEng.h>

#include <boost/tokenizer.hpp>

#include "kdlib/dbgengine.h"
#include "kdlib/windbg.h"
#include "kdlib/dbgio.h"
#include "win/dbgmgr.h"

static volatile LONG g_initCount = 0;

//////////////////////////////////////////////////////////////////////////////

namespace kdlib {
namespace windbg {

///////////////////////////////////////////////////////////////////////////////

void WindbgOut::write( const std::wstring& str )
{
    std::wstringstream  sstr(str);
    std::wstring  line;

    while (std::getline(sstr, line)) 
    {
        if (!sstr.eof())
            line += L'\n';

        g_dbgMgr->control->ControlledOutputWide(  
          // DEBUG_OUTCTL_AMBIENT_TEXT,
           DEBUG_OUTCTL_ALL_OTHER_CLIENTS,
           DEBUG_OUTPUT_NORMAL, 
           L"%ws",
           line.c_str()
           );
    }
}

void WindbgOut::writedml( const std::wstring& str )
{
    g_dbgMgr->control->ControlledOutputWide(  
        //DEBUG_OUTCTL_AMBIENT_DML,
        DEBUG_OUTCTL_ALL_OTHER_CLIENTS | DEBUG_OUTCTL_DML,
        DEBUG_OUTPUT_NORMAL, 
        L"%ws",
        str.c_str()
        );
}


WindbgOut  windbgOut;

///////////////////////////////////////////////////////////////////////////////

void WindbgErr::write( const std::wstring& str )
{
    g_dbgMgr->control->ControlledOutputWide(
        DEBUG_OUTCTL_ALL_OTHER_CLIENTS,
        DEBUG_OUTPUT_ERROR, 
        L"%ws",
        str.c_str()
        );
}

void WindbgErr::writedml( const std::wstring& str )
{
    g_dbgMgr->control->ControlledOutputWide(  
        DEBUG_OUTCTL_ALL_OTHER_CLIENTS,
        DEBUG_OUTPUT_ERROR | DEBUG_OUTCTL_DML,
        L"%ws",
        str.c_str()
        );
}

WindbgOut  windbgErr;

///////////////////////////////////////////////////////////////////////////////

std::wstring WindbgIn::readline()
{
    std::vector<wchar_t>  inputBuffer(0x10000);

    ULONG  read = 0;
    g_dbgMgr->control->InputWide( &inputBuffer[0], static_cast<ULONG>(inputBuffer.size()), &read );

        std::wstring  inputstr = std::wstring( &inputBuffer[0] );

        return inputstr.empty() ? L"\n" : inputstr;
}

WindbgIn  windbgIn;

///////////////////////////////////////////////////////////////////////////////

InterruptWatch::InterruptWatch()
{
    m_stopEvent = CreateEvent( NULL, TRUE, FALSE, NULL );
    m_thread = CreateThread( NULL, 0, threadRoutine, this, 0, NULL );
}

///////////////////////////////////////////////////////////////////////////////

InterruptWatch::~InterruptWatch()
{
    SetEvent( m_stopEvent );
    WaitForSingleObject( m_thread, INFINITE );
    CloseHandle( m_stopEvent );
    CloseHandle( m_thread );
}

///////////////////////////////////////////////////////////////////////////////

DWORD InterruptWatch::interruptWatchRoutine()
{
    while( WAIT_TIMEOUT == WaitForSingleObject( m_stopEvent, 250 ) )
    {
        HRESULT  hres =  g_dbgMgr->control->GetInterrupt();
        if ( hres == S_OK && onInterrupt() )
            break;
    }

    return 0;
}

///////////////////////////////////////////////////////////////////////////////

void WindbgExtension::parseArgs(const char* args)
{
    typedef  boost::escaped_list_separator<char>    char_separator_t;
    typedef  boost::tokenizer< char_separator_t >   char_tokenizer_t;  

    std::string                 argsStr( args );

    char_tokenizer_t            token( argsStr , char_separator_t( "", " \t", "\"" ) );
    ArgsList    argsList;

    for ( char_tokenizer_t::iterator   it = token.begin(); it != token.end(); ++it )
    {
        if ( *it != "" )
            argsList.push_back( *it );
    }

    m_argsList = argsList;
}

///////////////////////////////////////////////////////////////////////////////

void WindbgExtension::registerStructFormatter( WindbgStructFormatter* fmt )
{
    m_formatterList.push_back(fmt);
}

///////////////////////////////////////////////////////////////////////////////

std::string WindbgExtension::getFormatterNames()
{
    std::stringstream  sstr;

    for ( FormatterList::iterator it = m_formatterList.begin(); it != m_formatterList.end(); ++it )
    {
        sstr <<  (*it)->getName();
        sstr << '\0';
    }
    sstr << '\0';

    return sstr.str();
}

///////////////////////////////////////////////////////////////////////////////

std::string WindbgExtension::getFormatterValue( const std::string& structName, MEMOFFSET_64 offset)
{
    for (FormatterList::iterator it = m_formatterList.begin();  it != m_formatterList.end(); ++it )
    {
        if ( (*it)->getName() == structName )
        {
            std::string resultStr;

            try {
                resultStr = (*it)->printValue(offset);
            } 
            catch(kdlib::MemoryException& )
            {
                resultStr = "memory access error";
            }
            catch(kdlib::DbgException&)
            {
                resultStr = "unknown error";
            }

            resultStr += '\0';

            return resultStr;
        }
    }

    return "";
}

///////////////////////////////////////////////////////////////////////////////

} } // kdlib::windbg namespace end


extern kdlib::windbg::WindbgExtension* WinDbgExt;

HRESULT
CALLBACK
DebugExtensionInitialize(
    PULONG  Version,
    PULONG  Flags )
{
    *Version = DEBUG_EXTENSION_VERSION( 1, 0 );
    *Flags = 0;

    if ( 1 == InterlockedIncrement( &g_initCount ) )
    {
        kdlib::initialize();

        kdlib::dbgout =&kdlib::windbg::windbgOut;
        kdlib::dbgerr = &kdlib::windbg::windbgErr;
        kdlib::dbgin = &kdlib::windbg::windbgIn;

        WinDbgExt->setUp();

        WinDbgExt->setInit(true);
    }

    return S_OK;
}

//////////////////////////////////////////////////////////////////////////////

VOID
CALLBACK
DebugExtensionUninitialize()
{
    if ( 0 == InterlockedDecrement( &g_initCount ) )
    {
        WinDbgExt->setInit(false);
        WinDbgExt->tearDown();

        kdlib::uninitialize();
    }
}

///////////////////////////////////////////////////////////////////////////////

HRESULT
CALLBACK 
KnownStructOutput(
    ULONG Flag,
    ULONG64 Address,
    PSTR StructName,
    PSTR Buffer,
    PULONG BufferSize
    )
{
    switch(Flag)
    {
    case DEBUG_KNOWN_STRUCT_GET_NAMES:
        {
            std::string  resultStr = WinDbgExt->getFormatterNames();

            if ( *BufferSize  < resultStr.size() )
            {
                *BufferSize = resultStr.size();
                return S_FALSE;
            }

            memcpy_s( Buffer, *BufferSize, resultStr.c_str(), resultStr.size() );

            *BufferSize = resultStr.size();
            return S_OK;
        }


    case DEBUG_KNOWN_STRUCT_SUPPRESS_TYPE_NAME:
        return S_OK;

    case DEBUG_KNOWN_STRUCT_GET_SINGLE_LINE_OUTPUT:

            std::string  resultStr = WinDbgExt->getFormatterValue(StructName, Address);

            if ( *BufferSize  < resultStr.size() )
            {
                *BufferSize = resultStr.size();
                return S_FALSE;
            }

            memcpy_s( Buffer, *BufferSize, resultStr.c_str(), resultStr.size() );

            *BufferSize = resultStr.size();
            return S_OK;
    }

    return S_FALSE;
}

///////////////////////////////////////////////////////////////////////////////
