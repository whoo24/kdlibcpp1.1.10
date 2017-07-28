#pragma once

#include <vector>

#include "kdlib/dbgtypedef.h"

#include "win/dbgmgr.h"
#include "win/threadctx.h"

namespace kdlib {

///////////////////////////////////////////////////////////////////////////////

class ContextAutoRestoreIf {

public:

    ContextAutoRestoreIf(bool skipContextRestore = false);

    virtual ~ContextAutoRestoreIf();

protected:

    bool  m_skipRestore;

    bool  m_quietState;

    THREAD_DEBUG_ID    m_currentThread;
    PROCESS_DEBUG_ID   m_currentProcess;
    SYSTEM_DEBUG_ID    m_currentSystem;

    bool  m_savedRegCtx;
    std::vector<DEBUG_VALUE>  m_regValues;

    bool m_savedLocalContext;
    CONTEXT_STORAGE  m_localContext;

    bool m_savedCurrentFrame;
    MEMOFFSET_64  m_instructionOffset;
    DEBUG_STACK_FRAME  m_currentFrame;
};


class  ContextAutoRestore : public ContextAutoRestoreIf
{
public:

    ContextAutoRestore() : ContextAutoRestoreIf(false)
    {}
};

///////////////////////////////////////////////////////////////////////////////

} // namesapce kdlib

