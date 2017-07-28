#pragma once

#include <string>

#include "gtest/gtest.h"
#include "kdlib/dbgengine.h"
#include "kdlib/module.h"
#include "kdlib/exceptions.h"


class ProcessFixture : public ::testing::Test 
{
public:

    ProcessFixture( const std::wstring &cmdline = std::wstring() ) :
        m_cmdLine( cmdline )
        {}

protected:

    const std::wstring m_processName;

    virtual void SetUp() {
        if ( m_cmdLine.empty() )
            m_processId = kdlib::startProcess(L"targetapp.exe");
        else
            m_processId = kdlib::startProcess(L"targetapp.exe " + m_cmdLine );

        kdlib::targetGo(); // go to work break point

        m_targetModule = kdlib::loadModule( L"targetapp" );
    }

    virtual void TearDown() {
        try {
            kdlib::terminateProcess( m_processId );
        } catch(kdlib::DbgException&)
        {}
    }

    kdlib::PROCESS_DEBUG_ID m_processId;

    kdlib::ModulePtr m_targetModule;

    std::wstring  m_cmdLine;
};

#ifndef FIELD_OFFSET
#define FIELD_OFFSET(type, field)    ((long)&(((type *)0)->field))
#endif

#ifndef ARRAYSIZE
#define ARRAYSIZE(A) (sizeof(A)/sizeof((A)[0]))
#endif
