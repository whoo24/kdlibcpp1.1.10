#pragma once

#include <string>

namespace kdlib {

///////////////////////////////////////////////////////////////////////////////

typedef unsigned long           PROCESS_ID;
typedef unsigned long           PROCESS_DEBUG_ID;
typedef unsigned long           THREAD_ID;
typedef unsigned long           THREAD_DEBUG_ID;
typedef unsigned long           SYSTEM_DEBUG_ID;
typedef unsigned long           BREAKPOINT_ID;
typedef unsigned long long      MEMOFFSET_64;
typedef unsigned long           MEMOFFSET_32;
typedef long                    MEMOFFSET_REL;
typedef long                    MEMDISPLACEMENT;
typedef unsigned long           BITOFFSET;
typedef unsigned long           ACCESS_TYPE;
typedef unsigned long long      EXTENSION_ID;
typedef unsigned long           RELREG_ID;
typedef unsigned long long      SYMBOL_ID;

enum DebugCallbackResult {
    DebugCallbackProceed = 0,
    DebugCallbackNoChange = 1,
    DebugCallbackBreak = 2,
    DebugCallbackMax = 3
};

enum ExecutionStatus {
    DebugStatusNoChange,
    DebugStatusGo,
    DebugStatusBreak,
    DebugStatusNoDebuggee
};


enum BreakpointType {

    SoftwareBreakpoint,
    DataAccessBreakpoint
};

enum CPUType {
    CPU_I386 = 0,
    CPU_AMD64 = 1,
    CPU_MAX = 2
};

enum CPURegType {
    RegInt8,
    RegInt16,
    RegInt32,
    RegInt64,
    RegFloat32,
    RegFloat64,
    RegFloat80,
    RegFloat128,
    RegVector64,
    RegVector128
};


enum EventType {
    EventTypeNoEvent,
    EventTypeBreakpoint,
    EventTypeException,
    EventTypeCreateThread,
    EventTypeExitThread,
    EventTypeCreateProcess,
    EventTypeExitProcess,
    EventTypeLoadModule,
    EventTypeUnloadModule,
    EventTypeSystemError,
    EventTypeSessionStatus,
    EventTypeChangeDebuggeeState,
    EventTypeChangeEngineState,
    EventTypeChangeSymbolState
};

struct SystemInfo {
    unsigned long  majorVersion;
    unsigned long  minorVersion;
    unsigned long  servicePackNumber;
    unsigned long  buildNumber;
    std::wstring  buildDescription;
};

struct SystemCrashInfo {
    unsigned long long  code;
    unsigned long paramterCount;
    unsigned long long parameters[0x10];
};

struct ExceptionInfo {
    bool  firstChance;
    unsigned long  exceptionCode;
    unsigned long  exceptionFlags;
    MEMOFFSET_64  exceptionRecord;
    MEMOFFSET_64  exceptionAddress;
    PROCESS_DEBUG_ID  process;
    THREAD_DEBUG_ID  thread;
    unsigned long parameterCount;
    unsigned long long parameters[0x10];
};

struct FixedFileInfo {
    unsigned long Signature;
    unsigned long StrucVersion;
    unsigned long FileVersionMS;
    unsigned long FileVersionLS;
    unsigned long ProductVersionMS;
    unsigned long ProductVersionLS;
    unsigned long FileFlagsMask;
    unsigned long FileFlags;
    unsigned long FileOS;
    unsigned long FileType;
    unsigned long FileSubtype;
    unsigned long FileDateMS;
    unsigned long FileDateLS;
};

enum FileFlag {
    FileFlagDebug           = 0x00000001,
    FileFlagPreRelease      = 0x00000002,
    FileFlagPatched         = 0x00000004,
    FileFlagPrivateBuild    = 0x00000008,
    FileFlagInfoInferred    = 0x00000010,
    FileFlagSpecialBuild    = 0x00000020,
};

enum MemoryProtect {

    PageNoAccess            = 0x00000001,
    PageReadOnly            = 0x00000002,
    PageReadWrite           = 0x00000004,
    PageWriteCopy           = 0x00000008,
    PageExecute             = 0x00000010,
    PageExecuteRead         = 0x00000020,
    PageExecuteReadWrite    = 0x00000040,
    PageExecuteWriteCopy    = 0x00000080
};


enum ProcessExitReason {
    ProcessExit,
    ProcessTerminate,
    ProcessDetach
};

enum ProcessDebugOptions {
    ProcessBreakOnStart    = 0x00000001,
    ProcessBreakOnStop     = 0x00000002,
    ProcessDebugChildren   = 0x00000004,
    ProcessNoDebugHeap     = 0x00000008,

    ProcessDebugDefault = ProcessBreakOnStart
};

typedef unsigned long  ProcessDebugFlags;


struct FrameDesc {
    MEMOFFSET_64            instructionOffset;
    MEMOFFSET_64            returnOffset;
    MEMOFFSET_64            frameOffset;
    MEMOFFSET_64            stackOffset; 
};

struct SyntheticSymbol {
    MEMOFFSET_64    moduleBase;
    SYMBOL_ID       symbolId;
};

enum VarStorage {
    UnknownVar,
    MemoryVar,
    RegisterVar
};


struct LiveProcessInfo {
    PROCESS_ID  pid;
    PROCESS_ID  parentPid;
    std::wstring  name;
    std::wstring  user;
    std::wstring  commandLine;
};

///////////////////////////////////////////////////////////////////////////////

} // kdlib namespace end


