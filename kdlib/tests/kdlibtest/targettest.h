
#pragma once

#include "basefixture.h"
#include "memdumpfixture.h"

#include "kdlib/process.h"
#include "kdlib/stack.h"

using namespace kdlib;

class TargetTest : public BaseFixture
{
public:
};

class KernelDumpTest : public BaseFixture
{
public:

    std::wstring getKernelDumpName()
    {
        return L"..\\..\\..\\kdlib\\tests\\dumps\\win8_x64_mem.cab";
    }

    bool is64BitSystem() {
        return true;
    }

    void loadDump()
    {
        PROCESS_DEBUG_ID  dumpId;

        ASSERT_NO_THROW(dumpId = kdlib::loadDump(getKernelDumpName()));

        m_dumpIds.push_back(dumpId);
    }

     virtual void TearDown() 
     {
         std::for_each(m_dumpIds.begin(), m_dumpIds.end(), kdlib::closeDump);
     }

private:

    std::list<PROCESS_DEBUG_ID>  m_dumpIds;
    
};

TEST_F(TargetTest, getTargetSystem)
{
    ASSERT_NO_THROW(startProcess(L"targetapp.exe"));
    ASSERT_NO_THROW(startProcess(L"targetapp.exe"));

    TargetSystemPtr  targetSystem;
    ASSERT_NO_THROW( targetSystem = TargetSystem::getCurrent() );
}


TEST_F(TargetTest, getSystemProcess)
{
    ASSERT_NO_THROW(startProcess(L"targetapp.exe"));
    ASSERT_NO_THROW(startProcess(L"targetapp.exe"));

    TargetSystemPtr  targetSystem;
    ASSERT_NO_THROW( targetSystem = TargetSystem::getCurrent() );

    EXPECT_NO_THROW( targetSystem->getCurrentProcess() );
    EXPECT_NO_THROW( targetSystem->getProcessByIndex(0) );

    PROCESS_ID  pid = -1;
    EXPECT_NO_THROW( pid = targetSystem->getProcessByIndex(0)->getSystemId() );
    EXPECT_EQ( pid, targetSystem->getProcessBySystemId(pid)->getSystemId() );

    EXPECT_THROW( targetSystem->getProcessBySystemId(12345), DbgException );
}


TEST_F(TargetTest, getTargetProcess )
{
    ASSERT_NO_THROW(startProcess(L"targetapp.exe") );
    ASSERT_NO_THROW(startProcess(L"targetapp.exe"));

    TargetProcessPtr  targetProcess;
    EXPECT_NO_THROW( targetProcess = TargetProcess::getCurrent());
    EXPECT_TRUE( 0 != targetProcess );
    EXPECT_NO_THROW( targetProcess = TargetProcess::getByIndex(0));
    EXPECT_TRUE( 0 != targetProcess );
    EXPECT_NO_THROW(targetProcess = TargetProcess::getByIndex(1));
    EXPECT_TRUE(0 != targetProcess);
    EXPECT_THROW(TargetProcess::getByIndex(2), IndexException);

    EXPECT_EQ( TargetProcess::getByIndex(0)->getSystemId(), TargetProcess::getBySystemId(TargetProcess::getByIndex(0)->getSystemId())->getSystemId());
    EXPECT_THROW( TargetProcess::getBySystemId(12345), DbgException );
}

TEST_F(TargetTest, getSystemId)
{
    ASSERT_NO_THROW( startProcess(L"targetapp.exe") );
    ASSERT_NO_THROW( startProcess(L"targetapp.exe") );
    TargetProcessPtr  targetProcess;
    ASSERT_NO_THROW( targetProcess = TargetProcess::getCurrent() );
    ASSERT_TRUE( 0 != targetProcess );
    EXPECT_NE( 0, targetProcess->getSystemId() );
    EXPECT_NE( 0, TargetProcess::getByIndex(1)->getSystemId() );
}

TEST_F(TargetTest, getPebOffset)
{
    ASSERT_NO_THROW(startProcess(L"targetapp.exe"));
    ASSERT_NO_THROW(startProcess(L"targetapp.exe"));
    EXPECT_NE(0, TargetProcess::getCurrent()->getPebOffset());
    EXPECT_NE(0, TargetProcess::getByIndex(0)->getPebOffset());
    EXPECT_NE(0, TargetProcess::getByIndex(1)->getPebOffset());
}

TEST_F(TargetTest, getExecutableName)
{
    ASSERT_NO_THROW(startProcess(L"targetapp.exe"));
    ASSERT_NO_THROW(startProcess(L"targetapp.exe"));
    EXPECT_NE(L"", TargetProcess::getCurrent()->getExecutableName());
    EXPECT_NE(L"", TargetProcess::getByIndex(0)->getExecutableName());
    EXPECT_NE(L"", TargetProcess::getByIndex(1)->getExecutableName());
}

TEST_F(TargetTest, getNumberThreads)
{
    PROCESS_DEBUG_ID  id;
    ASSERT_NO_THROW(id = startProcess(L"targetapp.exe multithread")); 
    ASSERT_NO_THROW(targetGo());
    EXPECT_LE(5UL, TargetProcess::getCurrent()->getNumberThreads());
}

TEST_F(TargetTest, enumThreads)
{
    PROCESS_DEBUG_ID  procId;
    ASSERT_NO_THROW(procId = startProcess(L"targetapp.exe multithread"));
    ASSERT_NO_THROW(targetGo());

    TargetProcessPtr  process;
    ASSERT_NO_THROW(process = TargetProcess::getCurrent());

    unsigned long numberThreads = 0;
    ASSERT_NO_THROW(numberThreads = process->getNumberThreads());

    for (unsigned long i = 0; i < numberThreads; ++i)
    {
        TargetThreadPtr  thread;
        ASSERT_NO_THROW(thread = process->getThreadByIndex(i));
    }
}

TEST_F(TargetTest, getThread)
{
    ASSERT_NO_THROW(startProcess(L"targetapp.exe"));

    TargetProcessPtr  process;
    ASSERT_NO_THROW(process = TargetProcess::getCurrent());


    TargetThreadPtr  thread;
    ASSERT_NO_THROW( thread = process->getThreadByIndex(0) );
    EXPECT_EQ( thread->getId(), process->getThreadById(thread->getId())->getId());
    EXPECT_EQ( thread->getId(), process->getThreadBySystemId(thread->getSystemId())->getId());
}

TEST_F(TargetTest, EnumModules)
{
    PROCESS_DEBUG_ID  procId;
    ASSERT_NO_THROW(procId = startProcess(L"targetapp.exe multithread"));
    ASSERT_NO_THROW(targetGo());

    TargetProcessPtr  process;
    ASSERT_NO_THROW(process = TargetProcess::getCurrent());

    unsigned long numberModules = 0;
    ASSERT_NO_THROW(numberModules = process->getNumberModules());
    ASSERT_LT(0UL, numberModules);

    for (unsigned long i = 0; i < numberModules; ++i)
    {
        ModulePtr  module;
        ASSERT_NO_THROW(module = process->getModuleByIndex(i));
    }
}

TEST_F(TargetTest, GetModule)
{
    PROCESS_DEBUG_ID  procId;
    ASSERT_NO_THROW(procId = startProcess(L"targetapp.exe multithread"));
    ASSERT_NO_THROW(targetGo());

    TargetProcessPtr  process;
    ASSERT_NO_THROW(process = TargetProcess::getCurrent());

    ModulePtr  module;

    ASSERT_NO_THROW( module = process->getModuleByIndex(0) );
    EXPECT_EQ( module->getBase(), process->getModuleByOffset(module->getBase())->getBase());
    EXPECT_NO_THROW(process->getModuleByName(L"targetapp"));
}

TEST_F(TargetTest, currentThread)
{
    ASSERT_NO_THROW(startProcess(L"targetapp.exe"));

    TargetProcessPtr  targetProcess;
    ASSERT_NO_THROW(targetProcess = TargetProcess::getCurrent());

    TargetThreadPtr  targetThread;
    ASSERT_NO_THROW(targetThread = targetProcess->getCurrentThread());
    
    EXPECT_TRUE(targetThread->isCurrent());
    EXPECT_NE(0, targetThread->getSystemId());
    EXPECT_NE(0, targetThread->getTebOffset());
}

TEST_F(TargetTest, setCurrentThread)
{
    PROCESS_DEBUG_ID  procId;
    ASSERT_NO_THROW(procId = startProcess(L"targetapp.exe multithread"));
    ASSERT_NO_THROW(targetGo());

    TargetProcessPtr  process;
    ASSERT_NO_THROW(process = TargetProcess::getCurrent());

    std::set<MEMOFFSET_64>  tebList;

    unsigned long numberThreads = 0;
    ASSERT_NO_THROW(numberThreads = process->getNumberThreads());

    for (unsigned long i = 0; i < numberThreads; ++i)
    {
        TargetThreadPtr  thread;
        ASSERT_NO_THROW(thread = process->getThreadByIndex(i));
        ASSERT_NO_THROW(thread->setCurrent());
        EXPECT_TRUE(thread->isCurrent());
        tebList.insert(process->getCurrentThread()->getTebOffset());
    }

    EXPECT_EQ(numberThreads, tebList.size());
}

TEST_F(TargetTest, threadGetProcess)
{
    ASSERT_NO_THROW(startProcess(L"targetapp.exe"));

    TargetProcessPtr  targetProcess;
    ASSERT_NO_THROW(targetProcess = TargetProcess::getCurrent());

    TargetThreadPtr  targetThread;
    ASSERT_NO_THROW(targetThread = targetProcess->getCurrentThread());

    EXPECT_EQ(targetProcess->getSystemId(), targetThread->getProcess()->getSystemId());
}

TEST_F(TargetTest, threadGetStack)
{
    ASSERT_NO_THROW(startProcess(L"targetapp.exe"));

    TargetThreadPtr  targetThread;
    ASSERT_NO_THROW(targetThread = TargetProcess::getCurrent()->getCurrentThread());
    EXPECT_NO_THROW(targetThread->getStack());
}


TEST_F(TargetTest, restoreContext)
{
    ASSERT_NO_THROW(startProcess(L"targetapp.exe"));
    ASSERT_NO_THROW(startProcess(L"targetapp.exe"));

    ASSERT_NO_THROW(targetGo());

    ASSERT_NO_THROW(setCurrentStackFrameByIndex(2));

    unsigned int processNumber = TargetProcess::getNumber();

    for (unsigned int i = 0; i < processNumber; ++i)
        TargetProcess::getByIndex(i)->getExecutableName();

    EXPECT_EQ(2, getCurrentStackFrameNumber());
}

TEST_F(TargetTest, TwoSystem)
{
    PROCESS_DEBUG_ID   id;

    ASSERT_NO_THROW(id = startProcess(L"targetapp.exe  processtest"));
    ASSERT_NO_THROW(writeDump(L"targetapp0.dmp", false));
    ASSERT_NO_THROW(terminateProcess(id));

    ASSERT_NO_THROW(id = startProcess(L"targetapp.exe  processtest"));
    ASSERT_NO_THROW(writeDump(L"targetapp1.dmp", false));
    ASSERT_NO_THROW(terminateProcess(id));

    PROCESS_DEBUG_ID  id1, id2;
    ASSERT_NO_THROW(id1 = loadDump(L"targetapp0.dmp"));
    ASSERT_NO_THROW(id2 = loadDump(L"targetapp1.dmp"));

    EXPECT_LE(2UL, TargetSystem::getNumber());

    for (unsigned long i = 0; i < TargetSystem::getNumber(); ++i)
    {
        TargetSystemPtr  targetSystem;
        EXPECT_NO_THROW(targetSystem = TargetSystem::getByIndex(i));

        std::wstring  targetSystemDesc;
        EXPECT_NO_THROW(targetSystemDesc = targetSystem->getDescription());
    }

    EXPECT_NO_THROW(closeDump(id1));
    EXPECT_NO_THROW(closeDump(id2));
}

TEST_F(TargetTest, IpFrameStackOffset)
{
    ASSERT_NO_THROW(startProcess(L"targetapp.exe"));

    TargetProcessPtr  targetProcess;
    ASSERT_NO_THROW(targetProcess = TargetProcess::getCurrent());

    TargetThreadPtr  targetThread;
    ASSERT_NO_THROW(targetThread = targetProcess->getCurrentThread());

    EXPECT_EQ(kdlib::getInstructionOffset(), targetThread->getInstructionOffset());
    EXPECT_EQ(kdlib::getFrameOffset(), targetThread->getFrameOffset());
    EXPECT_EQ(kdlib::getStackOffset(), targetThread->getStackOffset());
}


TEST_F(KernelDumpTest, LoadDump)
{
    loadDump();

    std::wstring  str = TargetSystem::getCurrent()->getDescription();
}
    

TEST_F(KernelDumpTest, KernelTargetDesc)
{
    loadDump();

    TargetSystemPtr  targetSystem;
    ASSERT_NO_THROW(targetSystem = TargetSystem::getCurrent());

    std::wstring  targetSystemDesc;
    EXPECT_NO_THROW(targetSystemDesc = targetSystem->getDescription());

    EXPECT_TRUE(targetSystem->isDumpAnalyzing());
    EXPECT_TRUE(targetSystem->isKernelDebugging());
    EXPECT_EQ( is64BitSystem(), targetSystem->is64bitSystem() );
}

TEST_F(KernelDumpTest, KernelTargetProcess)
{
    loadDump();

    TargetSystemPtr  targetSystem;
    ASSERT_NO_THROW(targetSystem = TargetSystem::getCurrent());

    unsigned long  procNumber = targetSystem->getNumberProcesses();
    EXPECT_EQ(1, procNumber);

    TargetProcessPtr  targetProcess;
    ASSERT_NO_THROW(targetProcess = targetSystem->getCurrentProcess());

    std::wstring  exeName;
    EXPECT_NO_THROW( exeName = targetProcess->getExecutableName() );

    EXPECT_LT( 0UL, targetProcess->getNumberThreads());
    EXPECT_LT( 0UL, targetProcess->getNumberModules());
}

TEST_F(KernelDumpTest, KernelTargetThread)
{
    loadDump();

    TargetProcessPtr  targetProcess;
    ASSERT_NO_THROW(targetProcess = TargetProcess::getCurrent());
    
    unsigned long numberThread;
    ASSERT_NO_THROW(numberThread = targetProcess->getNumberThreads());
    EXPECT_NE( 0UL, numberThread);

    for ( unsigned long i = 0; i < numberThread; ++i)
    {
        TargetThreadPtr  targetThread;
        ASSERT_NO_THROW(targetThread = targetProcess->getThreadByIndex(i));
        EXPECT_NE(0UL, targetThread->getFrameOffset());
        EXPECT_NE(0UL, targetThread->getInstructionOffset());
        EXPECT_NE(0UL, targetThread->getStackOffset());

        EXPECT_THROW(targetThread->getSystemId(), kdlib::DbgException);
        EXPECT_NE(0UL, targetThread->getTebOffset());

        EXPECT_NO_THROW(targetThread->setCurrent());
        EXPECT_TRUE(targetThread->isCurrent());

        //unsigned long  registerNumber;
        //ASSERT_NO_THROW(registerNumber = targetThread->getNumberRegisters());
        //EXPECT_NE(0UL, registerNumber);
    }

    int currentOccured = 0;
    for ( unsigned long i = 0; i < numberThread; ++i)
    {
        if ( targetProcess->getThreadByIndex(i)->isCurrent() )
            currentOccured++;
    }

    EXPECT_EQ(1, currentOccured);
}

TEST_F(KernelDumpTest, DISABLED_TwoDump)
{
    loadDump();
    loadDump();

    for ( unsigned long i = 0; i < TargetSystem::getNumber(); ++i)
    {
        TargetSystemPtr  targetSystem;
        ASSERT_NO_THROW( targetSystem = TargetSystem::getByIndex(i));

        if (targetSystem->isDumpAnalyzing())
            EXPECT_NO_THROW( targetSystem->getProcessById(0)->setCurrent() );
    }
}


