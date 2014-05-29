// InfoCollector.cpp : Defines the entry point for the console application.

#include "stdafx.h"
#include "StringBuilder.h"

#include <Windows.h>
#include <WinBase.h>
#include <codecvt>

#include "SystemInfo.h"
#include "InfoCollector.h"
#include "CommandLineParser.h"

#pragma comment(lib, "ws2_32.lib")

int _tmain(int argc, _TCHAR* argv[])
{
    IntializeSystemInfo();

    WORD wVersionRequested = MAKEWORD(1, 1);
    WSADATA wsaData;
    WSAStartup(wVersionRequested, &wsaData);
    SOCKET udpSocket = INVALID_SOCKET;

    int result = IC_NOERROR;
    bool showHelp = false;

    do
    {
        wstring apiKey;
        wstring envInfo;
        int intervalTime = 0;
        ConnectionInfo connection;

        if (cmdOptionExists(argv, argv + argc, L"-h") == true
            || cmdOptionExists(argv, argv + argc, L"/h") == true)
        {
            showHelp = true;
            break;
        }

        apiKey = GetApiKey(argc, argv);
        envInfo = GetEnvInfo(argc, argv);
        intervalTime = GetIntervalTime(argc, argv);

        if (apiKey.length() == 0)
        {
            OutputError(L"NO ApiKey\n");
            result = IC_NO_APIKEY;
            showHelp = true;
            break;
        }

        if (envInfo.length() == 0)
        {
            OutputError(L"NO AgentID Info\n");
            result = IC_NO_AGENTIDINFO;
            showHelp = true;
            break;
        }

        string address = GetHostAddress(argc, argv, connection);
        struct hostent *host = gethostbyname(address.c_str());
        if (host == nullptr)
        {
            OutputError(L"Can't resolve host address: %s\n", address.c_str());
            result = IC_NO_RESOLVE_HOSTADDR;
            break;
        }

        struct sockaddr_in cliAddr, remoteServAddr;

        remoteServAddr.sin_family = host->h_addrtype;
        memcpy((char *)&remoteServAddr.sin_addr.s_addr, host->h_addr_list[0], host->h_length);
        remoteServAddr.sin_port = htons((u_short)connection.Getport());

        /* socket creation */
        SOCKET udpSocket = socket(AF_INET, SOCK_DGRAM, 0);
        cliAddr.sin_family = AF_INET;
        cliAddr.sin_addr.s_addr = htonl(INADDR_ANY);
        cliAddr.sin_port = htons(0);

        if (bind(udpSocket, (struct sockaddr *) &cliAddr, sizeof(cliAddr)) < 0)
        {
            OutputError(L"%d: cannot bind port\n", connection.Getport());
            result = IC_ERROR_SOCKETBIND;
            break;
        }

        ProcessInfo(apiKey, envInfo, udpSocket, remoteServAddr);

    } while (false);

    if (udpSocket != INVALID_SOCKET)
    {
        closesocket(udpSocket);
    }

    if (showHelp == true)
    {
        ShowHelp();
    }

    WSACleanup();

    return result;
}

string GetHostAddress(int argc, _TCHAR* argv[], ConnectionInfo connection)
{
    wstring txt;
    if (cmdOptionExists(argv, argv + argc, L"-s") == true)
    {
        txt = getCmdOption(argv, argv + argc, L"-s");
    }
    else
    {
        txt = connection.Getaddress();
    }
    
    return ws2s(txt);
}

void SendToServer(SOCKET socketHandle, sockaddr_in remoteServAddr, StringBuilder &sb)
{
    wstring data = sb.ToString();

#if defined(_DEBUG)
    ::OutputDebugString((data + L"\n").c_str());
#endif

    std::wstring_convert<std::codecvt_utf8<wchar_t>> myconv;
    string utfData = myconv.to_bytes(data);;

    sendto(socketHandle, (const char *)utfData.c_str(), utfData.length(), 0,
        (struct sockaddr *) &remoteServAddr,
        sizeof(remoteServAddr));
}

wstring GetApiKey(int argc, _TCHAR* argv[])
{
    if (cmdOptionExists(argv, argv + argc, L"-key") == true)
    {
        return getCmdOption(argv, argv + argc, L"-key");
    }

    return L"";
}

wstring GetEnvInfo(int argc, _TCHAR* argv[])
{
    wstring uid = GetUID(argc, argv);
    if (uid.length() == 0)
    {
        return GetComputerName();
    }

    return uid;
}

wstring GetUID(int argc, _TCHAR* argv[])
{
    if (cmdOptionExists(argv, argv + argc, L"-id") == true)
    {
        return getCmdOption(argv, argv + argc, L"-id");
    }

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

int GetIntervalTime(int argc, _TCHAR* argv[])
{
    wstring txt = L"2";
    if (cmdOptionExists(argv, argv + argc, L"-d") == true)
    {
        txt = getCmdOption(argv, argv + argc, L"-d");
    }

    return ::_wtoi(txt.c_str());
}

void ProcessInfo(wstring apiKey, wstring envKey, SOCKET socketHandle, sockaddr_in remoteServAddr)
{
    StringBuilder sb;

    while (true)
    {
        sb.clear();

        sb.push_back(L"{");
        {
            sb.push_back(L"\"" + SystemInfo::Members::CpuUsage + L"\":");
            sb.push_back(L"{");
            {
                float totalUsage = 0.0f;
                sb.push_back(L"\"" + CpuInfo::Members::Unit + L"\":[");
                if (RetrieveCpuInfo(sb, &totalUsage) == false)
                {
                    Sleep(1000);
                    continue;
                }

                sb.push_back(L"]");

                wchar_t buf[40];
                swprintf(buf, L"}, \"%s\": %.2f", CpuInfo::Members::Total.c_str(), totalUsage);
                sb.push_back(buf);
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
            sb.push_back(L"\"");
            sb.push_back(envKey);
            sb.push_back(L"\"");
        }

        sb.push_back(L"}");

        SendToServer(socketHandle, remoteServAddr, sb);
        Sleep(1000);
    }

}

void ShowHelp()
{
#if defined(_WIN64)
    int platformId = 64;
#else
    int platformId = 32;
#endif
    printf("ic%d.exe -h\n", platformId);
    printf("ic%d.exe -key [apikey] -s [hostaddress] -id [agentid] -d [interval-sec]\n", platformId);
    printf("samples:\n");
    printf("    ic%d.exe -key 8CFDDE2478 -s 192.168.0.5 -id mypc100 -d 5\n", platformId);
    printf("        data server: 192.168.0.5, apikey: 8CFDDE2478\n");
    printf("        id: mypc100, every 5 seconds\n");
}

void OutputError(wstring txt, ...)
{
    va_list args;
    const wchar_t *fmt = txt.c_str();
    va_start(args, fmt);

    wprintf(L"Error: ");
    wprintf(txt.c_str(), args);
    wprintf(L"\n");
    
    va_end(args);
}