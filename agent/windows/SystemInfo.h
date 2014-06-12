#pragma once

#include "stdafx.h"

typedef LONG(WINAPI *PROCNTQSI)(UINT, PVOID, ULONG, PULONG);

typedef struct
{
    LARGE_INTEGER IdleTime;
    LARGE_INTEGER KernelTime;
    LARGE_INTEGER UserTime;
    LARGE_INTEGER Reserved1[2];
    ULONG Reserved2;
} SYSTEM_PROCESSOR_PERFORMANCE_INFORMATION;

#define STATUS_INFO_LENGTH_MISMATCH 0xC0000004

typedef enum _SYSTEMINFOCLASS
{
    SystemBasicInformation,             // 0x002C
    SystemProcessorInformation,         // 0x000C
    SystemPerformanceInformation,       // 0x0138
    SystemTimeInformation,              // 0x0020
    SystemPathInformation,              // not implemented
    SystemProcessInformation,           // 0x00C8+ per process
    SystemCallInformation,              // 0x0018 + (n * 0x0004)
    SystemConfigurationInformation,     // 0x0018

    SystemProcessorPerformanceInformation = 8,
    SystemProcessorCounters = 8,            // 0x0030 per cpu

    SystemGlobalFlag,                   // 0x0004 (fails if size != 4)
    SystemCallTimeInformation,          // not implemented
    SystemModuleInformation,            // 0x0004 + (n * 0x011C)
    SystemLockInformation,              // 0x0004 + (n * 0x0024)
    SystemStackTraceInformation,        // not implemented
    SystemPagedPoolInformation,         // checked build only
    SystemNonPagedPoolInformation,      // checked build only
    SystemHandleInformation,            // 0x0004  + (n * 0x0010)
    SystemObjectTypeInformation,        // 0x0038+ + (n * 0x0030+)
    SystemPageFileInformation,          // 0x0018+ per page file
    SystemVdmInstemulInformation,       // 0x0088
    SystemVdmBopInformation,            // invalid info class
    SystemCacheInformation,             // 0x0024
    SystemPoolTagInformation,           // 0x0004 + (n * 0x001C)
    SystemInterruptInformation,         // 0x0000, or 0x0018 per cpu
    SystemDpcInformation,               // 0x0014
    SystemFullMemoryInformation,        // checked build only
    SystemLoadDriver,                   // 0x0018, set mode only
    SystemUnloadDriver,                 // 0x0004, set mode only
    SystemTimeAdjustmentInformation,    // 0x000C, 0x0008 writeable
    SystemSummaryMemoryInformation,     // checked build only
    SystemNextEventIdInformation,       // checked build only
    SystemEventIdsInformation,          // checked build only
    SystemCrashDumpInformation,         // 0x0004
    SystemExceptionInformation,         // 0x0010
    SystemCrashDumpStateInformation,    // 0x0004
    SystemDebuggerInformation,          // 0x0002
    SystemContextSwitchInformation,     // 0x0030
    SystemRegistryQuotaInformation,     // 0x000C
    SystemAddDriver,                    // 0x0008, set mode only
    SystemPrioritySeparationInformation,// 0x0004, set mode only
    SystemPlugPlayBusInformation,       // not implemented
    SystemDockInformation,              // not implemented
    SystemPowerInfo,             // 0x0060 (XP only!)
    SystemProcessorSpeedInformation,    // 0x000C (XP only!)
    SystemTimeZoneInformation,          // 0x00AC
    SystemLookasideInformation,         // n * 0x0020
    SystemSetTimeSlipEvent,
    SystemCreateSession,				// set mode only
    SystemDeleteSession,				// set mode only
    SystemInvalidInfoClass1,			// invalid info class
    SystemRangeStartInformation,		// 0x0004 (fails if size != 4)
    SystemVerifierInformation,
    SystemAddVerifier,
    SystemSessionProcessesInformation,	// checked build only
    MaxSystemInfoClass
} SYSTEMINFOCLASS, *PSYSTEMINFOCLASS;

bool IsConsoleApp();
wstring GetEnvVar(wstring key);
bool GetMemoryInfo(__int64 *maxMemory, __int64 *currentUsage);
bool RetrieveCpuInfo(StringBuilder &sb, float *totalUsage);
void IntializeSystemInfo();