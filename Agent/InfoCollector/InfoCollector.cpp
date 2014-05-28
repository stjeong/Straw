// InfoCollector.cpp : Defines the entry point for the console application.

#include "stdafx.h"
#include "StringBuilder.h"

#include <Windows.h>
#include <WinBase.h>

wstring GetApiKey();
wstring GetUID();
wstring GetComputerName();
wstring GetEnvInfo();
int GetIntervalTime();

typedef LONG(WINAPI *PROCNTQSI)(UINT, PVOID, ULONG, PULONG);
PROCNTQSI NtQuerySystemInformation;

bool GetMemoryInfo(__int64 *maxMemory, __int64 *currentUsage);
bool RetrieveCpuInfo(StringBuilder &sb);
void SendToServer(StringBuilder &sb);

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

int _tmain(int argc, _TCHAR* argv[])
{
    if (NtQuerySystemInformation == nullptr)
    {
        NtQuerySystemInformation = (PROCNTQSI)GetProcAddress(GetModuleHandle(L"ntdll"), "NtQuerySystemInformation");
    }

    StringBuilder sb;

    // getting env info
    wstring apiKey;
    wstring envInfo;
    int intervalTime = 0;

    while (true)
    {
        apiKey = GetApiKey();
        envInfo = GetEnvInfo();
        intervalTime = GetIntervalTime();

        if (apiKey.length() != 0 && envInfo.length() != 0)
        {
            break;
        }

        ::OutputDebugString(L"No Information [apiKey, envInfo]");
        Sleep(1000);
    }

    while (true)
    {
        sb.clear();

        sb.push_back(L"{");
        {
            sb.push_back(L"\"" + SystemInfo::Members::CpuUsage + L"\":");
            sb.push_back(L"{");
            {
                sb.push_back(L"\"" + CpuInfo::Members::Unit + L"\":[");
                if (RetrieveCpuInfo(sb) == false)
                {
                    Sleep(1000);
                    continue;
                }

                sb.push_back(L"]");
            }
            sb.push_back(L"},");

            __int64 maxMemory;
            __int64 currentUsage;
            GetMemoryInfo(&maxMemory, &currentUsage);

            sb.push_back(L"\"" + SystemInfo::Members::MemoryUsage + L"\":");
            {
                sb.push_back(L"{\"" + MemoryInfo::Members::MaxMB + L"\":");
                sb.push_back(maxMemory);

                sb.push_back(L", \"" + MemoryInfo::Members::CurrentMB + L"\":");
                sb.push_back(currentUsage);
                sb.push_back(L"},");
            }

            sb.push_back(L"\"" + PacketBase::Members::ApiKey + L"\":");
            sb.push_back(L"\"");
            sb.push_back(apiKey);
            sb.push_back(L"\",");

            sb.push_back(L"\"EnvInfo\":");
            sb.push_back(GetEnvInfo());
        }

        sb.push_back(L"}");

        SendToServer(sb);
        Sleep(1000);
    }

    return 0;
}

void SendToServer(StringBuilder &sb)
{
    
}

bool GetMemoryInfo(__int64 *maxMemory, __int64 *currentUsage)
{
    *maxMemory = 0;
    *currentUsage = 0;

    {
        MEMORYSTATUSEX memStatus;
        memStatus.dwLength = sizeof(memStatus);

        if (GlobalMemoryStatusEx(&memStatus) == FALSE)
        {
            return false;
        }

        *maxMemory = (memStatus.ullTotalPhys / 1024 / 1024);
        *currentUsage = ((memStatus.ullTotalPhys - memStatus.ullAvailPhys) / 1024 / 1024);
    }

    return true;
}

int old_numberOfCpu;
vector<LONGLONG> old_cpuTime;

bool RetrieveCpuInfo(StringBuilder &sb)
{
    if (NtQuerySystemInformation == nullptr)
    {
        return false;
    }

    ULONG result = 0;
    ULONG sizeLength = 0;
    SYSTEM_PROCESSOR_PERFORMANCE_INFORMATION *pProcInfo = nullptr;
    int status = NtQuerySystemInformation(SystemProcessorPerformanceInformation, NULL, 0, &result);
    if (status != STATUS_INFO_LENGTH_MISMATCH)
    {
        return false;
    }

    BYTE *pByte = nullptr;

    do
    {
        pByte = new BYTE[result];
        if (pByte == nullptr)
        {
            return false;
        }

        sizeLength = result;
        result = 0;
        pProcInfo = (SYSTEM_PROCESSOR_PERFORMANCE_INFORMATION *)pByte;

        int status = NtQuerySystemInformation(SystemProcessorPerformanceInformation, pProcInfo, sizeLength, &result);
        if (status != NO_ERROR)
        {
            return false;
        }

        int numberOfCpu = result / 48;
        bool firstGet = false;
        const int INFO_PER_CPU = 3;

        if (numberOfCpu != old_numberOfCpu)
        {
            old_cpuTime.clear();

            for (int i = 0; i < numberOfCpu; i++)
            {
                LONGLONG userTime = pProcInfo[i].UserTime.QuadPart;
                LONGLONG idleTime = pProcInfo[i].IdleTime.QuadPart;
                LONGLONG kernelTime = pProcInfo[i].KernelTime.QuadPart;

                LONGLONG totalTime = userTime + kernelTime;

                // KernelTime == Kernel + Idle
                kernelTime = kernelTime - idleTime;
                if (kernelTime < 0)
                {
                    kernelTime = 0;
                }

                old_cpuTime.push_back(userTime);
                old_cpuTime.push_back(idleTime);
                old_cpuTime.push_back(kernelTime);
            }

            old_numberOfCpu = numberOfCpu;
            return false;
        }

        LONGLONG totalUsedGap = 0;
        LONGLONG totalCpuGap = 0;

        for (int i = 0; i < (numberOfCpu * INFO_PER_CPU); i += INFO_PER_CPU)
        {
            int j = i / INFO_PER_CPU;

            LONGLONG currentUser = pProcInfo[j].UserTime.QuadPart;
            LONGLONG currentIdle = pProcInfo[j].IdleTime.QuadPart;
            LONGLONG currentKernel = pProcInfo[j].KernelTime.QuadPart;
            currentKernel = currentKernel - currentIdle;
            if (currentKernel < 0)
            {
                currentKernel = 0;
            }

            LONGLONG oldUserTime = old_cpuTime[i + 0];
            LONGLONG oldIdleTime = old_cpuTime[i + 1];
            LONGLONG oldKernelTime = old_cpuTime[i + 2];

            LONGLONG kernelGap = currentKernel - oldKernelTime;
            LONGLONG userGap = currentUser - oldUserTime;
            LONGLONG idleGap = currentIdle - oldIdleTime;

            LONGLONG totalGap = kernelGap + userGap + idleGap;

            totalUsedGap += kernelGap + userGap;
            totalCpuGap += totalGap;

            float kernel = (kernelGap * 100.0f) / totalGap;
            float user = (userGap * 100.0f) / totalGap;
            float idle = (idleGap * 100.0f) / totalGap;

            old_cpuTime[i + 0] = currentUser;
            old_cpuTime[i + 1] = currentIdle;
            old_cpuTime[i + 2] = currentKernel;

            wchar_t buf[25];
            swprintf(buf, L"%.2f", (kernel + user));
            sb.push_back(buf);
            sb.push_back(L",");
        }

        wchar_t buf[25];
        swprintf(buf, L"%.2f", ((totalUsedGap * 100.0f) / totalCpuGap));
        sb.push_back(buf);

    } while (false);

    if (pByte != nullptr)
    {
        delete [] pByte;
    }

    return true;
}

wstring GetApiKey()
{
    return L"f31fc79434df8e4b7f9fd1f5bebe5b111baf8571";
}

wstring GetEnvInfo()
{
    // try to get UID from registry
    wstring uid = GetUID();
    if (uid.length() == 0)
    {
        // if none, return ComputerName
        return GetComputerName();
    }

    return uid;
}

wstring GetUID()
{
    return L"";
}

wstring GetComputerName()
{
    DWORD dwSize = 0;
    ::GetComputerNameEx(ComputerNameDnsFullyQualified, NULL, &dwSize);

    if (GetLastError() != ERROR_MORE_DATA)
    {
        return L"";
    }

    wstring txt;

    wchar_t *pBuf = new wchar_t[dwSize + 1];
    if (::GetComputerNameEx(ComputerNameDnsFullyQualified, pBuf, &dwSize) == TRUE)
    {
        txt = pBuf;
    }
    delete[] pBuf;

    return txt;
}

int GetIntervalTime()
{
    return 2;
}