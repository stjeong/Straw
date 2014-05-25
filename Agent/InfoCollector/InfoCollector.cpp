// InfoCollector.cpp : Defines the entry point for the console application.

#include "stdafx.h"
#include "StringBuilder.h"

#include <Windows.h>
#include <WinBase.h>

wstring GetApiKey();
wstring GetEnvInfo();

bool GetMemoryInfo(__int64 *maxMemory, __int64 *currentUsage);

class Type
{
    public:
};

int _tmain(int argc, _TCHAR* argv[])
{
    StringBuilder sb;

    sb.push_back(L"{");
    {
        sb.push_back(L"\"" + SystemInfo::Members::CpuUsage + L"\":");
        sb.push_back(L"[");
        {
            
        }
        sb.push_back(L"],");

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
        sb.push_back(GetApiKey());
        sb.push_back(L"\",");

        sb.push_back(L"\"EnvInfo\":");
        sb.push_back(GetEnvInfo());
    }

    sb.push_back(L"}");

    ::OutputDebugString(sb.ToString().c_str());

	return 0;
}

bool GetMemoryInfo(__int64 *maxMemory, __int64 *currentUsage)
{
    *maxMemory = 0;
    *currentUsage = 0;

    {
        *maxMemory = 8388608; // 8 * 1024 * 1024; // 8GB
        *currentUsage = 6291456; // 6 * 1024 * 1024; // 6GB
    }

    return true;
}

wstring GetApiKey()
{
    return L"f31fc79434df8e4b7f9fd1f5bebe5b111baf8571";
}

wstring GetEnvInfo()
{
    return L"null";
}