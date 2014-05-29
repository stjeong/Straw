// InfoCollector.cpp : Defines the entry point for the console application.

#include "stdafx.h"
#include "StringBuilder.h"

#include <Windows.h>
#include <WinBase.h>
#include <codecvt>

#include "SystemInfo.h"

#pragma comment(lib, "ws2_32.lib")

wstring GetApiKey();
wstring GetUID();
wstring GetComputerName();
wstring GetEnvInfo();
int GetIntervalTime();
void ProcessInfo(wstring apiKey, SOCKET socketHandle, sockaddr_in remoteServAddr);

void SendToServer(SOCKET socketHandle, sockaddr_in remoteServAddr, StringBuilder &sb);

int _tmain(int argc, _TCHAR* argv[])
{
    IntializeSystemInfo();

    WORD wVersionRequested = MAKEWORD(1, 1);
    WSADATA wsaData;
    WSAStartup(wVersionRequested, &wsaData);
    SOCKET udpSocket = INVALID_SOCKET;

    int result = IC_NOERROR;

    do
    {
        // getting env info
        wstring apiKey;
        wstring envInfo;
        int intervalTime = 0;
        ConnectionInfo connection;

        apiKey = GetApiKey();
        envInfo = GetEnvInfo();
        intervalTime = GetIntervalTime();

        if (apiKey.length() == 0)
        {
            printf("NO ApiKey");
            result = IC_NO_APIKEY;
            break;
        }

        if (envInfo.length() == 0)
        {
            printf("NO AgentID Info");
            result = IC_NO_AGENTIDINFO;
            break;
        }

        string address = ws2s(connection.Getaddress());
        struct hostent *host = gethostbyname(address.c_str());
        if (host == nullptr)
        {
            printf("Can't resolve host address: %s", address.c_str());
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
            printf("%d: cannot bind port", connection.Getport());
            result = IC_ERROR_SOCKETBIND;
            break;
        }

        ProcessInfo(apiKey, udpSocket, remoteServAddr);

    } while (false);

    if (udpSocket != INVALID_SOCKET)
    {
        closesocket(udpSocket);
    }

    WSACleanup();

    return result;
}

void SendToServer(SOCKET socketHandle, sockaddr_in remoteServAddr, StringBuilder &sb)
{
    wstring data = sb.ToString();

    std::wstring_convert<std::codecvt_utf8<wchar_t>> myconv;
    string utfData = myconv.to_bytes(data);;

    sendto(socketHandle, (const char *)utfData.c_str(), utfData.length(), 0,
        (struct sockaddr *) &remoteServAddr,
        sizeof(remoteServAddr));
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

void ProcessInfo(wstring apiKey, SOCKET socketHandle, sockaddr_in remoteServAddr)
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
            sb.push_back(L"\"");
            sb.push_back(GetEnvInfo());
            sb.push_back(L"\"");
        }

        sb.push_back(L"}");

        SendToServer(socketHandle, remoteServAddr, sb);
        Sleep(1000);
    }

}