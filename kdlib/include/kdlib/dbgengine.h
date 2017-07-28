#pragma once

#include <string>
#include <vector>

#include "kdlib/dbgtypedef.h"
#include "kdlib/variant.h"

namespace kdlib {

//////////////////////////////////////////////////////////////////////////////

// Initialization/Uninitialization
bool initialize();
bool remote_initialize( const std::wstring& remoteOptions );
bool isInintilized();
void uninitialize();

// manage debug target

PROCESS_DEBUG_ID startProcess(const std::wstring  &processName, const ProcessDebugFlags& flags = ProcessDebugDefault);
PROCESS_DEBUG_ID attachProcess(PROCESS_ID pid, const ProcessDebugFlags& flags = ProcessDebugDefault);
void detachProcess( PROCESS_DEBUG_ID processId = -1);
void terminateProcess( PROCESS_DEBUG_ID processId = -1);
void detachAllProcesses();
void terminateAllProcesses();

PROCESS_DEBUG_ID loadDump( const std::wstring &fileName );
void closeDump( PROCESS_DEBUG_ID processId = -1);
void writeDump( const std::wstring &fileNamem, bool smallDump );

bool isLocalKernelDebuggerEnabled();
PROCESS_DEBUG_ID attachKernel( const std::wstring &connectOptions = L"" );

bool isDumpAnalyzing();
bool isKernelDebugging();

std::wstring debugCommand(const std::wstring &command, bool suppressOutput=false);
NumVariant evaluate( const std::wstring  &expression, bool cplusplus=false );

// system properties
size_t ptrSize();
bool is64bitSystem();
size_t getPageSize();
unsigned long getSystemUptime();
unsigned long getCurrentTime();
void getSystemInfo( SystemInfo &systemInfo );
void getSystemCrashInfo( SystemCrashInfo &crashInfo );

//manage target modules
unsigned long getNumberModules();
MEMOFFSET_64 getModuleOffsetByIndex(unsigned long index);
std::vector<MEMOFFSET_64> getModuleBasesList();
MEMOFFSET_64 findModuleBase( const std::wstring &moduleName );
MEMOFFSET_64 findModuleBase( MEMOFFSET_64 offset );
MEMOFFSET_64 findModuleBySymbol( const std::wstring &symbolName );
MEMOFFSET_32 getModuleSize( MEMOFFSET_64 baseOffset );
std::wstring getModuleName( MEMOFFSET_64 baseOffset );
std::wstring getModuleImageName( MEMOFFSET_64 baseOffset ); 
unsigned long getModuleTimeStamp( MEMOFFSET_64 baseOffset );
unsigned long getModuleCheckSum( MEMOFFSET_64 baseOffset );
bool isModuleUnloaded( MEMOFFSET_64 baseOffset );
bool isModuleUserMode( MEMOFFSET_64 baseOffset );
std::wstring getModuleSymbolFileName( MEMOFFSET_64 baseOffset );
std::string getModuleVersionInfo( MEMOFFSET_64 baseOffset, const std::string &value );
void getModuleFixedFileInfo( MEMOFFSET_64 baseOffset, FixedFileInfo &fixedFileInfo );

// Symbol path
std::wstring getSymbolPath();
void setSymbolPath(const std::wstring &symPath);
void appendSymbolPath(const std::wstring &symPath);

std::wstring getSrcPath();
void setSrcPath(const std::wstring &srcPath);
void appendSrcPath(const std::wstring &srcPath);

std::wstring getSourceFile( MEMOFFSET_64 offset = 0);
std::wstring getSourceFileFromSrcSrv( MEMOFFSET_64 offset = 0);
void getSourceLine( std::wstring &fileName, unsigned long &lineno, long &displacement, MEMOFFSET_64 offset = 0 );

// processes end threads

#define CURRENT_THREAD_ID  (-1)
#define CURRENT_PROCESS_ID  (-1)
#define CURRENT_SYSTEM_ID  (-1)

THREAD_DEBUG_ID getCurrentThreadId();
unsigned long getNumberThreads();
void checkThreadById(THREAD_DEBUG_ID id);
THREAD_DEBUG_ID getThreadIdByOffset(MEMOFFSET_64 offset);
THREAD_DEBUG_ID getThreadIdBySystemId(THREAD_ID tid);
THREAD_DEBUG_ID getThreadIdByIndex(unsigned long index);

THREAD_ID getThreadSystemId(THREAD_DEBUG_ID id = CURRENT_THREAD_ID);
MEMOFFSET_64 getThreadOffset(THREAD_DEBUG_ID id = CURRENT_THREAD_ID);

void setCurrentThreadById(THREAD_DEBUG_ID id);
void setCurrentThreadByOffset(MEMOFFSET_64 offset);

void setImplicitThread(MEMOFFSET_64 offset); 
MEMOFFSET_64 getImplicitThreadOffset();

MEMOFFSET_64 getCurrentThread();
void setCurrentThread(MEMOFFSET_64 offset);

unsigned long getNumberProcesses();
PROCESS_DEBUG_ID getCurrentProcessId();
void checkProcessById(PROCESS_DEBUG_ID id);
PROCESS_DEBUG_ID getProcessIdByOffset( MEMOFFSET_64 offset );
PROCESS_DEBUG_ID getProcessIdBySystemId( PROCESS_ID pid );
PROCESS_DEBUG_ID getProcessIdByIndex(unsigned long index);

PROCESS_ID  getProcessSystemId(PROCESS_DEBUG_ID id = CURRENT_PROCESS_ID);
MEMOFFSET_64 getProcessOffset(PROCESS_DEBUG_ID id  = CURRENT_PROCESS_ID);
std::wstring getProcessExecutableName(PROCESS_DEBUG_ID id = CURRENT_PROCESS_ID);

void setCurrentProcessById( PROCESS_DEBUG_ID id );
void setCurrentProcessByOffset( MEMOFFSET_64 offset );

void setImplicitProcess( MEMOFFSET_64 offset ); 
MEMOFFSET_64 getImplicitProcessOffset();

MEMOFFSET_64 getCurrentProcess();
void setCurrentProcess( MEMOFFSET_64  offset );

unsigned long  getNumberSystems();
SYSTEM_DEBUG_ID getCurrentSystemId();
void checkSystemById(SYSTEM_DEBUG_ID id);
SYSTEM_DEBUG_ID getSystemIdByIndex(unsigned long index);
std::wstring getSystemDesc(SYSTEM_DEBUG_ID id = CURRENT_SYSTEM_ID);
void setCurrentSystemById(SYSTEM_DEBUG_ID id);

ExecutionStatus targetExecutionStatus();
ExecutionStatus targetGo();
ExecutionStatus targetStep();
ExecutionStatus targetStepIn();
ExecutionStatus targetStepOut();
void targetBreak();

ExecutionStatus sourceStepIn();
ExecutionStatus sourceStepOver();

// events
EventType getLastEventType();
PROCESS_DEBUG_ID getLastEventProcessId();
THREAD_DEBUG_ID getLastEventThreadId(); 
ExceptionInfo  getLastException();

// registers
MEMOFFSET_64 getInstructionOffset();
MEMOFFSET_64 getStackOffset();
MEMOFFSET_64 getFrameOffset();

void setInstructionOffset(MEMOFFSET_64 offset);
void setStackOffset(MEMOFFSET_64 offset);
void setFrameOffset(MEMOFFSET_64 offset);

unsigned long getRegisterNumber();
unsigned long getRegisterIndex(const std::wstring &name );
CPURegType getRegisterType(unsigned long index );
std::wstring getRegisterName(unsigned long index );
size_t getRegisterSize(unsigned long index);
void getRegisterValue(unsigned long index, void* buffer, size_t bufferSize );
void setRegisterValue(unsigned long index, void* buffer, size_t bufferSize );
CPUType getCPUType();
CPUType getCPUMode();
void setCPUMode(CPUType mode );
void switchCPUMode();

MEMOFFSET_64 getReturnReg();

unsigned long long loadMSR(unsigned long msrIndex );
void setMSR(unsigned long msrIndex, unsigned long long value );
void getStackTrace(std::vector<FrameDesc> &stackTrace);

//
// Extensions
std::wstring getExtensionSearchPath();
EXTENSION_ID loadExtension(const std::wstring &extPath );
void removeExtension( EXTENSION_ID extHandle );
void removeExtension( const std::wstring &extPath );
EXTENSION_ID getExtension(const std::wstring &extPath );
std::wstring callExtension( EXTENSION_ID extHandle, const std::wstring command, const std::wstring  &params  );

// synthetic symbols
void getNearSyntheticSymbols( MEMOFFSET_64 offset, std::vector< SyntheticSymbol > &syntheticSymbols );
void getSyntheticSymbols( const std::wstring &name, std::vector< SyntheticSymbol > &syntheticSymbols );
void getSyntheticSymbolInformation(
    const SyntheticSymbol& syntheticSymbol,
    std::wstring *name = nullptr,
    MEMOFFSET_64 *offset = nullptr,
    unsigned long *size = nullptr
);

SyntheticSymbol addSyntheticSymbol( MEMOFFSET_64 offset, unsigned long size, const std::wstring &name );
void removeSyntheticSymbol(const SyntheticSymbol& syntheticSymbol);

std::wstring loadSourceFileFromSrcSrv(MEMOFFSET_64 offset, const std::wstring& fileName);

// aux function
unsigned long getNumberLiveProcesses();
void getLiveProcessesList(std::vector<LiveProcessInfo> &processInfo);

///////////////////////////////////////////////////////////////////////////////

} // kdlib namespace end

