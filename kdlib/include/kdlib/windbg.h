#pragma once

#include <vector>
#include <string>
#include <list>

#include <atlcomcli.h>
#include <DbgEng.h>

#include "kdlib/dbgtypedef.h"
#include "kdlib/exceptions.h"
#include "kdlib/dbgio.h"

namespace kdlib {
namespace windbg {

///////////////////////////////////////////////////////////////////////////////

struct WindbgStructFormatter 
{
    virtual std::string  getName() = 0;
    virtual std::string  printValue( MEMOFFSET_64 offset ) = 0;
};

template<typename T>
class WindbgStructFormatterRegistrator 
{
public:

    WindbgStructFormatterRegistrator()
    {
        m_impl.reset( new T );
        WinDbgExt->registerStructFormatter(m_impl.get());
    }

private:

    std::auto_ptr<T>  m_impl;
};

///////////////////////////////////////////////////////////////////////////////

class WindbgExtension {

public:

    WindbgExtension() : m_init(false) {}
    virtual ~WindbgExtension() {}

    virtual void setUp() {}
    virtual void tearDown() {}
    virtual void parseArgs(const char* args);

    bool isInit() {
        return m_init;
    }

    void setInit( bool state ) {
        m_init = state;
    }

    
    void registerStructFormatter( WindbgStructFormatter* fmt );
    std::string getFormatterNames();
    std::string getFormatterValue( const std::string& structName, MEMOFFSET_64 offset);

protected:

    typedef std::vector< std::string > ArgsList;

    const ArgsList& getArgs() const {
        return  m_argsList;
    }
    ArgsList  m_argsList;

    bool m_init;

    typedef std::list<WindbgStructFormatter*> FormatterList;
    FormatterList  m_formatterList;
};

///////////////////////////////////////////////////////////////////////////////

class InterruptWatch
{
public:

    InterruptWatch();
    virtual ~InterruptWatch();

    virtual bool onInterrupt() { return true; }

private:

    DWORD interruptWatchRoutine();

    static DWORD WINAPI threadRoutine(LPVOID lpParameter) {
        return  static_cast<InterruptWatch*>(lpParameter)->interruptWatchRoutine();
    }

    HANDLE  m_thread;

    HANDLE  m_stopEvent;
};


///////////////////////////////////////////////////////////////////////////////

class WindbgOut : public DbgOut
{
public:

    virtual void write( const std::wstring& str );

    virtual void writedml( const std::wstring& str );
    
};

///////////////////////////////////////////////////////////////////////////////

class WindbgErr : public DbgOut
{
public:

    virtual void write( const std::wstring& str );

    virtual void writedml( const std::wstring& str );
};
///////////////////////////////////////////////////////////////////////////////

class WindbgIn : public DbgIn
{
public:

    virtual std::wstring readline();
};

///////////////////////////////////////////////////////////////////////////////

} } // end kdlib::windbg namespace

///////////////////////////////////////////////////////////////////////////////

extern kdlib::windbg::WindbgExtension  *WinDbgExt;

#define KDLIB_WINDBG_EXTENSION_INIT(X)                                       \
X* KdlibExtImpl = new X();                                                   \
kdlib::windbg::WindbgExtension  *WinDbgExt = KdlibExtImpl

#define KDLIB_EXT_COMMAND_METHOD(METHOD_NAME)  void METHOD_NAME(void)

#define KDLIB_EXT_COMMAND_METHOD_IMPL(CLASS_NAME, METHOD_NAME)               \
extern "C"                                                                   \
HRESULT                                                                      \
CALLBACK                                                                     \
METHOD_NAME ( __in PDEBUG_CLIENT client,  __in_opt PCSTR args)               \
{                                                                            \
    ULONG  mask;                                                             \
    client->GetOutputMask( &mask );                                          \
    client->SetOutputMask( mask & ~DEBUG_OUTPUT_PROMPT );                    \
    CComQIPtr<IDebugControl4> ctrl = client;                                 \
    try  {                                                                   \
        KdlibExtImpl->parseArgs(args);                                       \
        KdlibExtImpl->METHOD_NAME();                                         \
    }                                                                        \
    catch( kdlib::DbgException& de )                                         \
    {                                                                        \
        ctrl->Output( DEBUG_OUTPUT_ERROR,  "Kdlib exception: %s", de.what() ); \
    }                                                                        \
    catch(...)                                                               \
    {                                                                        \
        ctrl->Output( DEBUG_OUTPUT_ERROR,  "Unknown kdlib exception" );  \
    }                                                                        \
    client->SetOutputMask( mask );                                           \
    return S_OK;                                                             \
}                                                                            \
void CLASS_NAME::METHOD_NAME()

///////////////////////////////////////////////////////////////////////////////

#define KDLIB_WINDBG_REGISTER_FORMATER(X)                                     \
namespace {                                                                   \
    kdlib::windbg::WindbgStructFormatterRegistrator<X>  fmt##X  ;             \
} 

///////////////////////////////////////////////////////////////////////////////
