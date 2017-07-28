#pragma once

#include <string>

#include "gtest/gtest.h"

#include "kdlib/dbgengine.h"
#include "kdlib/exceptions.h"

class MemDumpFixture : public ::testing::Test 
{
public:

    MemDumpFixture( const std::wstring &dumpName ) :
        m_dumpName(dumpName),
        m_dumpId(-1)
        {}

    virtual void SetUp() 
    {
        m_dumpId = kdlib::loadDump(m_dumpName);
    }

    virtual void TearDown()
    {
        try {
            if (m_dumpId != -1)
                kdlib::closeDump(m_dumpId);
        } 
        catch(kdlib::DbgException&)
        {}
    }

private:

    std::wstring  m_dumpName;
    kdlib::PROCESS_DEBUG_ID  m_dumpId;

};
