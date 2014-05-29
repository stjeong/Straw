#include "stdafx.h"
#include "StringBuilder.h"

#include <Windows.h>
#include <WinBase.h>
#include <codecvt>

#include "SystemInfo.h"

int old_numberOfCpu;
vector<LONGLONG> old_cpuTime;
PROCNTQSI NtQuerySystemInformation;

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

void IntializeSystemInfo()
{
    if (NtQuerySystemInformation == nullptr)
    {
        NtQuerySystemInformation = (PROCNTQSI)GetProcAddress(GetModuleHandle(L"ntdll"), "NtQuerySystemInformation");
    }
}

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
        delete[] pByte;
    }

    return true;
}