#include "stdafx.h"

#include "win/autoswitch.h"

namespace kdlib
{

///////////////////////////////////////////////////////////////////////////////

ContextAutoRestoreIf::ContextAutoRestoreIf(bool skipContextRestore)
{
    HRESULT      hres;

    m_skipRestore = skipContextRestore;
    if (m_skipRestore)
        return;

    memset( &m_localContext, 0, sizeof(m_localContext) );
    memset( &m_currentFrame, 0, sizeof(m_currentFrame) );

    m_quietState = g_dbgMgr->setQuietNotiification(true);

    hres = g_dbgMgr->system->GetCurrentThreadId(&m_currentThread);
    if (FAILED(hres))
    {
        m_currentThread = -1;
    }

    hres = g_dbgMgr->system->GetCurrentProcessId(&m_currentProcess);
    if (FAILED(hres))
    {
        m_currentProcess = -1;
    }

    hres = g_dbgMgr->system->GetCurrentSystemId(&m_currentSystem);
    if (FAILED(hres))
    {
        m_currentSystem = -1;
    }

    m_savedRegCtx = false;
    m_savedLocalContext = false;
    m_savedCurrentFrame = false;

    DEBUG_VALUE  regVal;
    hres = g_dbgMgr->registers->GetValues2(DEBUG_REGSRC_EXPLICIT, 1, NULL, 0, &regVal);
    if (S_OK == hres)
    {
        hres = g_dbgMgr->symbols->GetScope(&m_instructionOffset, NULL, &m_localContext, sizeof(m_localContext));
        m_savedLocalContext = (S_OK == hres);
    }

    hres = g_dbgMgr->symbols->GetScope(&m_instructionOffset, &m_currentFrame, NULL, 0);
    m_savedCurrentFrame = (S_OK == hres);
}


///////////////////////////////////////////////////////////////////////////////

ContextAutoRestoreIf::~ContextAutoRestoreIf()
{
    if (m_skipRestore)
        return;

    if (m_currentSystem != -1)
        g_dbgMgr->system->SetCurrentSystemId(m_currentSystem);

    if (m_currentProcess != -1)
        g_dbgMgr->system->SetCurrentProcessId(m_currentProcess);

    if (m_currentThread != -1)
        g_dbgMgr->system->SetCurrentThreadId(m_currentThread);

    if (m_savedRegCtx)
        g_dbgMgr->registers->SetValues2(DEBUG_REGSRC_EXPLICIT, static_cast<ULONG>(m_regValues.size()), NULL, 0, &m_regValues[0]);

    if (m_savedLocalContext)
        g_dbgMgr->symbols->SetScope(m_instructionOffset, NULL, &m_localContext, sizeof(m_localContext));

    if (m_savedCurrentFrame)
        g_dbgMgr->symbols->SetScope(m_instructionOffset, &m_currentFrame, NULL, 0);

    g_dbgMgr->setQuietNotiification(m_quietState);
}

///////////////////////////////////////////////////////////////////////////////

} // namespace kdlib
