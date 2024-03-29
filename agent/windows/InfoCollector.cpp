// InfoCollector.cpp : Defines the entry point for the console application.

#include "stdafx.h"

#include "StringBuilder.h"
#include "StringSplit.h"

#include <codecvt>

#include <thread>
#include <ppl.h>

#include <strsafe.h>

#include "SystemInfo.h"
#include "InfoCollector.h"
#include "CommandLineParser.h"

#pragma comment(lib, "ws2_32.lib")

// ./ic32.exe -key ce3bd840-f0a7-11e3-ac10-0800200c9a66 -s straw.imyoyo.net -port 8282 -regservice

int g_argc;
_TCHAR** g_argv;

SERVICE_STATUS_HANDLE g_serviceStatusHandle = NULL;
BOOL g_serviceRunning = FALSE;
BOOL g_servicePaused = FALSE;
HANDLE g_killServiceEvent = NULL;
HANDLE g_killSafeExitEvent = NULL;

wstring g_moduleFilePath = L"";
wstring g_modulePath = L"";
// wstring g_

int _tmain(int argc, _TCHAR* argv[])
{
    g_isConsoleApp = IsConsoleApp();

    g_argc = argc;
    g_argv = argv;

    if (g_isConsoleApp == TRUE)
    {
        return ServiceExecutionThread(NULL);
    }
    else
    {
        SERVICE_TABLE_ENTRY serviceTable[] =
        {
            { SERVICE_NAME, (LPSERVICE_MAIN_FUNCTION)ServiceMain },
            { NULL, NULL }
        };

        BOOL success;
        success = StartServiceCtrlDispatcher(serviceTable);
        if (!success)
        {
            OutputError(L"StartServiceCtrlDispatcher fails! (%d)", GetLastError());
        }
    }
}

void SetupHostPort(int argc, _TCHAR* argv[], ConnectionInfo &connection)
{
    wstring txt;

    if (g_isConsoleApp == false)
    {
        txt = GetEnvVar(L"port");
    }
    else
    {
        if (cmdOptionExists(argv, argv + argc, L"-port") == true)
        {
            txt = getCmdOption(argv, argv + argc, L"-port");
        }
    }

    if (txt.length() == 0)
    {
        return;
    }

    connection.Setport(_wtoi(txt.c_str()));
}

string GetHostAddress(int argc, _TCHAR* argv[], ConnectionInfo &connection)
{
    wstring txt;

    if (g_isConsoleApp == false)
    {
        txt = GetEnvVar(L"server");
        return ws2s(txt);
    }
    else
    {
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
}

void SendToServer(SOCKET socketHandle, sockaddr_in remoteServAddr, StringBuilder &sb)
{
    wstring data = sb.ToString();

    if (g_debugMode == TRUE)
    {
       // OutputConsole(L"%s\n", data.c_str());
    }

    std::wstring_convert<std::codecvt_utf8<wchar_t>> myconv;
    string utfData = myconv.to_bytes(data);

    sendto(socketHandle, (const char *)utfData.c_str(), (int)utfData.length(), 0,
        (struct sockaddr *) &remoteServAddr,
        sizeof(remoteServAddr));
}

BOOL GetDebugMode(int argc, _TCHAR* argv[])
{
    if (g_isConsoleApp == false)
    {
        return GetEnvVar(L"debug") == L"1";
    }

    if (cmdOptionExists(g_argv, g_argv + g_argc, L"-debug") == true)
    {
        return TRUE;
    }

    return FALSE;
}

wstring GetApiKey(int argc, _TCHAR* argv[])
{
    if (g_isConsoleApp == false)
    {
        return GetEnvVar(L"apiKey");
    }

    if (cmdOptionExists(argv, argv + argc, L"-key") == true)
    {
        return getCmdOption(argv, argv + argc, L"-key");
    }

    return L"";
}

wstring GetEnvInfo(int argc, _TCHAR* argv[])
{
    if (g_isConsoleApp == false)
    {
        wstring result = GetEnvVar(L"envKey");
        if (result.length() != 0)
        {
            return result;
        }
    }

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

vector<int> GetIntervalTime(int argc, _TCHAR* argv[])
{
    wstring txt = L"1,1";

    if (cmdOptionExists(argv, argv + argc, L"-d") == true)
    {
        txt = getCmdOption(argv, argv + argc, L"-d");
    }

    StringSplit split;
    split.SplitString(txt, L",");
    vector<int> intervalTimes;

    for (size_t i = 0; i < split.GetCount(); i++)
    {
        int interval = ::_wtoi(txt.c_str());
        intervalTimes.push_back(interval);
    }

    if (intervalTimes.size() < 2)
    {
        intervalTimes.push_back(5);
    }

    return intervalTimes;
}

void ProcessCpuMemInfo(wstring apiKey, wstring envKey, SOCKET socketHandle, sockaddr_in remoteServAddr, int interval)
{
    StringBuilder sb;
    int sleepTime = interval * 1000;

    while (true)
    {
        if (g_servicePaused == FALSE)
        {
            sb.clear();

            sb.push_back(L"{");
            {
                sb.push_back(L"\"" + SystemInfo::Members::cpuUsage + L"\":");
                sb.push_back(L"{");
                {
                    float totalUsage = 0.0f;
                    sb.push_back(L"\"" + CpuInfo::Members::unit + L"\":[");
                    if (RetrieveCpuInfo(sb, &totalUsage) == false)
                    {
                        Sleep(1000);
                        continue;
                    }

                    sb.push_back(L"]");

                    wchar_t buf[40];
                    StringCchPrintf(buf, 40, L", \"%s\": %.2f", CpuInfo::Members::total.c_str(), totalUsage / 100);
                    sb.push_back(buf);
                }
                sb.push_back(L"},");

                __int64 maxMemory;
                __int64 currentUsage;
                GetMemoryInfo(&maxMemory, &currentUsage);

                sb.push_back(L"\"" + SystemInfo::Members::memoryUsage + L"\":");
                {
                    sb.push_back(L"{\"" + MemoryInfo::Members::max + L"\":");
                    sb.push_back(maxMemory);

                    sb.push_back(L", \"" + MemoryInfo::Members::current + L"\":");
                    sb.push_back(currentUsage);
                    sb.push_back(L"},");
                }

                sb.push_back(L"\"" + PacketBase::Members::groupKey + L"\":");
                sb.push_back(L"\"");
                sb.push_back(apiKey);
                sb.push_back(L"\",");

                sb.push_back(L"\"" + PacketBase::Members::machineId + L"\":");
                sb.push_back(L"\"");
                sb.push_back(envKey);
                sb.push_back(L"\"");
            }

            sb.push_back(L"}");

            SendToServer(socketHandle, remoteServAddr, sb);

            if (g_isConsoleApp == TRUE)
            {
                printf(".");
            }
        }

        if (::WaitForSingleObject(g_killServiceEvent, sleepTime) == WAIT_TIMEOUT)
        {
            continue;
        }

#if defined(_DEBUG)
        ::OutputDebugString(L"ProcessCpuMemInfo-thread exited.");
#endif

        break;
    }
}

void ProcessDiskInfo(wstring apiKey, wstring envKey, SOCKET socketHandle, sockaddr_in remoteServAddr, int interval)
{
    StringBuilder sb;

    int sleepTime = interval * 1000;
    wchar_t driveLetter[_MAX_PATH + 1];
    HRESULT hr;
    ULARGE_INTEGER freeBytesAvailable;
    ULARGE_INTEGER totalNumberOfBytes;
    ULARGE_INTEGER totalNumberOfFreeBytes;
    
    while (true)
    {
        if (g_servicePaused == FALSE)
        {
            sb.clear();

            sb.push_back(L"{");

            sb.push_back(L"\"" + StorageInfo::Members::disk + L"\":");
            sb.push_back(L"[");

            DWORD dwNeedLength = ::GetLogicalDriveStrings(0, NULL);

            wchar_t *pWchBuf = nullptr;
            wchar_t *pHead = nullptr;

            do
            {
                if (dwNeedLength != 0)
                {
                    pWchBuf = new wchar_t[dwNeedLength + 1];
                    if (pWchBuf == nullptr)
                    {
                        break;
                    }

                    pHead = pWchBuf;

                    DWORD dwResult = ::GetLogicalDriveStrings(dwNeedLength, pWchBuf);

                    while (dwResult > 0)
                    {
                        hr = StringCchPrintf(driveLetter, _MAX_PATH, L"%s", pWchBuf);
                        if (FAILED(hr) == TRUE)
                        {
                            break;
                        }

                        int length = (int)(wcslen(driveLetter) + 1);
                        if (length < 2)
                        {
                            break;
                        }

                        dwResult -= length;
                        pWchBuf += length;

                        if (::GetDiskFreeSpaceEx(driveLetter, &freeBytesAvailable, &totalNumberOfBytes, &totalNumberOfFreeBytes) == FALSE)
                        {
                            break;
                        }

                        driveLetter[1] = '\0';

                        sb.push_back(L"{ \"" + DiskInfo::Members::name + L"\": \"");
                        sb.push_back(driveLetter);
                        sb.push_back(L"\", ");

                        sb.push_back(L"\"" + DiskInfo::Members::size + L"\": ");
                        sb.push_back((__int64)totalNumberOfBytes.QuadPart);
                        sb.push_back(L", ");

                        sb.push_back(L"\"" + DiskInfo::Members::current + L"\": ");
                        sb.push_back((__int64)(totalNumberOfBytes.QuadPart - totalNumberOfFreeBytes.QuadPart));
                        sb.push_back(L"}");

                        if (dwResult > 0)
                        {
                            sb.push_back(L",");
                        }
                    }
                }
            } while (false);

            if (pHead != nullptr)
            {
                delete[] pHead;
            }

            sb.push_back(L"],");

            sb.push_back(L"\"" + PacketBase::Members::groupKey + L"\":");
            sb.push_back(L"\"");
            sb.push_back(apiKey);
            sb.push_back(L"\",");

            sb.push_back(L"\"" + PacketBase::Members::machineId + L"\":");
            sb.push_back(L"\"");
            sb.push_back(envKey);
            sb.push_back(L"\"");

            sb.push_back(L"}");
        }

#if defined(_DEBUG)
        // ::OutputDebugString(sb.ToString().c_str());
#endif

        SendToServer(socketHandle, remoteServAddr, sb);

        if (g_isConsoleApp == TRUE)
        {
            printf(":");
        }

        DWORD dwEventResult = ::WaitForSingleObject(g_killServiceEvent, sleepTime);
        if (dwEventResult == WAIT_TIMEOUT)
        {
            continue;
        }

#if defined(_DEBUG)
        ::OutputDebugString(L"ProcessDiskInfo-thread exited.");
#endif
        break;
    }
}

void ShowHelp()
{
#if defined(_WIN64)
    int platformId = 64;
#else
    int platformId = 32;
#endif

    wstring appVersion = GetAppVersion(g_moduleFilePath.c_str(), NULL, NULL, NULL, NULL);

    OutputConsole(L"ic%d.exe (ver %s)\n", platformId, appVersion.c_str());
    OutputConsole(L"ic%d.exe -h\n", platformId);
    OutputConsole(L"ic%d.exe -key [apikey] -s [hostaddress] -id [agentid] -d [interval-sec,[...]]\n", platformId);
    OutputConsole(L"\n");
    OutputConsole(L"samples:\n");
    OutputConsole(L"    ic%d.exe -key 8CFDDE2478 -s 192.168.0.5\n", platformId);
    OutputConsole(L"        apikey: 8CFDDE2478\n");
    OutputConsole(L"        data server: 192.168.0.5:80\n");
    OutputConsole(L"        (optional: agent id by default - machine name)\n");
    OutputConsole(L"\n");
    OutputConsole(L"    ic%d.exe -key 8CFDDE2478 -s 192.168.0.5 -port 8282\n", platformId);
    OutputConsole(L"        apikey: 8CFDDE2478\n");
    OutputConsole(L"        data server: 192.168.0.5:8282\n");
    OutputConsole(L"        (optional: agent id by default - machine name)\n");
    OutputConsole(L"\n");
    OutputConsole(L"    ic%d.exe -key 8CFDDE2478 -s 192.168.0.5 -id mypc100\n", platformId);
    OutputConsole(L"        apikey: 8CFDDE2478\n");
    OutputConsole(L"        data server: 192.168.0.5:80\n");
    OutputConsole(L"        id: mypc100\n");
    OutputConsole(L"\n");
    OutputConsole(L"    ic%d.exe -key 8CFDDE2478 -s 192.168.0.5 -id mypc100  -regservice\n", platformId);
    OutputConsole(L"        Register as NT Service with this info,\n");
    OutputConsole(L"            apikey: 8CFDDE2478\n");
    OutputConsole(L"            data server: 192.168.0.5:80\n");
    OutputConsole(L"            id: mypc100\n");
    OutputConsole(L"\n");
    OutputConsole(L"    ic%d.exe -unreg\n", platformId);
    OutputConsole(L"        Unregister NT Service\n");
}

VOID ServiceMain(DWORD argc, LPTSTR *argv)
{
#if _DEBUG
    ::OutputDebugString(L"ServiceMain started.\n");
#endif

    BOOL success;
    g_serviceStatusHandle = RegisterServiceCtrlHandler(SERVICE_NAME,
        (LPHANDLER_FUNCTION)ServiceCtrlHandler);

    if (!g_serviceStatusHandle)
    {
        OutputError(L"RegisterServiceCtrlHandler fails! (%d)", GetLastError());
        return;
    }

    do
    {
        success = UpdateSCMStatus(SERVICE_START_PENDING, NO_ERROR, 0, 1, 5000);
        if (!success)
        {
            OutputError(L"1. UpdateSCMStatus fails! (%d)", GetLastError());
            break;
        }

        g_killSafeExitEvent = CreateEvent(0, TRUE, FALSE, 0);
        if (!g_killSafeExitEvent)
        {
            OutputError(L"CreateEvent-2 fails! (%d)", GetLastError());
            break;
        }

        success = UpdateSCMStatus(SERVICE_START_PENDING, NO_ERROR, 0, 2, 1000);
        if (!success)
        {
            OutputError(L"2. UpdateSCMStatus fails! (%d)", GetLastError());
            break;
        }

        success = UpdateSCMStatus(SERVICE_START_PENDING, NO_ERROR, 0, 3, 5000);
        if (!success)
        {
            OutputError(L"3. UpdateSCMStatus fails! (%d)", GetLastError());
            break;
        }

        success = StartServiceThread();
        if (!success)
        {
            OutputError(L"StartServiceThread fails! (%d)", GetLastError());
            break;
        }

        success = UpdateSCMStatus(SERVICE_RUNNING, NO_ERROR, 0, 0, 0);
        if (!success)
        {
            OutputError(L"4. UpdateSCMStatus fails! (%d)", GetLastError());
            break;
        }

#if _DEBUG
        ::OutputDebugString(L"WaitForSingleObject - started.\n");
#endif

        WaitForSingleObject(g_killSafeExitEvent, INFINITE);

        UpdateSCMStatus(SERVICE_STOPPED, NO_ERROR, 0, 0, 0);

#if _DEBUG
        ::OutputDebugString(L"WaitForSingleObject - end.\n");
#endif
    } while (false);
}

VOID ServiceCtrlHandler(DWORD controlCode)
{
    BOOL success;
    DWORD serviceCurrentStatus = controlCode;

    switch (controlCode)
    {
    case SERVICE_CONTROL_PAUSE:
        if (g_serviceRunning && !g_servicePaused)
        {
            success = UpdateSCMStatus(SERVICE_PAUSE_PENDING, NO_ERROR, 0, 1, 1000);
            g_servicePaused = TRUE;
        }
        break;

    case SERVICE_CONTROL_CONTINUE:
        if (g_serviceRunning && g_servicePaused)
        {
            success = UpdateSCMStatus(SERVICE_CONTINUE_PENDING, NO_ERROR, 0, 1, 1000);
            g_servicePaused = FALSE;
        }
        break;

    case SERVICE_CONTROL_INTERROGATE:
        break;

    case SERVICE_CONTROL_SHUTDOWN:
    case SERVICE_CONTROL_STOP:
        success = UpdateSCMStatus(SERVICE_STOP_PENDING, NO_ERROR, 0, 1, 5000);
        KillService();
        return;

    default:
        break;
    }

    UpdateSCMStatus(serviceCurrentStatus, NO_ERROR, 0, 0, 0);
}

BOOL UpdateSCMStatus(DWORD dwCurrentState,
    DWORD dwWin32ExitCode,
    DWORD dwServiceSpecificExitCode,
    DWORD dwCheckPoint,
    DWORD dwWaitHint)
{
    BOOL success;
    SERVICE_STATUS serviceStatus;
    serviceStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
    serviceStatus.dwCurrentState = dwCurrentState;
    
    if (dwCurrentState == SERVICE_START_PENDING)
    {
        serviceStatus.dwControlsAccepted = 0;
    }
    else
    {
        serviceStatus.dwControlsAccepted =
            SERVICE_ACCEPT_STOP |
            SERVICE_ACCEPT_PAUSE_CONTINUE |
            SERVICE_ACCEPT_SHUTDOWN;
    }

    if (dwServiceSpecificExitCode == 0)
    {
        serviceStatus.dwWin32ExitCode = dwWin32ExitCode;
    }
    else
    {
        serviceStatus.dwWin32ExitCode = ERROR_SERVICE_SPECIFIC_ERROR;
    }
    
    serviceStatus.dwServiceSpecificExitCode = dwServiceSpecificExitCode;
    serviceStatus.dwCheckPoint = dwCheckPoint;
    serviceStatus.dwWaitHint = dwWaitHint;
    
    success = SetServiceStatus(g_serviceStatusHandle, &serviceStatus);
    if (!success)
    {
        KillService();
    }

    return success;
}

void KillService()
{
    g_serviceRunning = FALSE;

#if _DEBUG
    ::OutputDebugString(L"KillService called\n");
#endif

    SetEvent(g_killServiceEvent);
}

DWORD ServiceExecutionThread(LPDWORD param)
{
#if _DEBUG
    ::OutputDebugString(L"ServiceExecutionThread\n");
#endif

    if (IntializeSystemInfo() == FALSE)
    {
        return IC_NO_INITIALIZE;
    }

    WORD wVersionRequested = MAKEWORD(1, 1);
    WSADATA wsaData;
    WSAStartup(wVersionRequested, &wsaData);
    SOCKET udpSocket = INVALID_SOCKET;

    int result = IC_NOERROR;
    bool showHelp = false;

    do
    {
        g_killServiceEvent = CreateEvent(0, TRUE, FALSE, 0);
        if (!g_killServiceEvent)
        {
            OutputError(L"CreateEvent fails! (%d)", GetLastError());
            result = IC_EVENT_CREATE_FAIL;
            break;
        }

        wstring apiKey;
        wstring envInfo;
        vector<int> intervalTimes;
        ConnectionInfo connection;

        g_debugMode = GetDebugMode(g_argc, g_argv);

        if (cmdOptionExists(g_argv, g_argv + g_argc, L"-h") == true
            || cmdOptionExists(g_argv, g_argv + g_argc, L"/h") == true)
        {
            showHelp = true;
            break;
        }

        if (cmdOptionExists(g_argv, g_argv + g_argc, L"-unreg") == true)
        {
            DoUnregistration();
            break;
        }

        if (cmdOptionExists(g_argv, g_argv + g_argc, L"-start") == true)
        {
            DoStartService();
            break;
        }

        if (cmdOptionExists(g_argv, g_argv + g_argc, L"-stop") == true)
        {
            DoStopService();
            break;
        }

        if (cmdOptionExists(g_argv, g_argv + g_argc, L"-update") == true)
        {
            ProcessLatestUpdate();
            break;
        }

        if (cmdOptionExists(g_argv, g_argv + g_argc, L"-restart") == true)
        {
            RestartService();
            break;
        }

        ::OutputDebugString(L"ServiceExecutionThread - GetInfo...\n");

        apiKey = GetApiKey(g_argc, g_argv);
        envInfo = GetEnvInfo(g_argc, g_argv);
        intervalTimes = GetIntervalTime(g_argc, g_argv);

        if (apiKey.length() == 0)
        {
            ::OutputDebugString(L"ServiceExecutionThread - NO ApiKey\n");
            OutputError(L"NO ApiKey\n");
            result = IC_NO_APIKEY;
            showHelp = true;
            break;
        }

        if (envInfo.length() == 0)
        {
            ::OutputDebugString(L"ServiceExecutionThread - NO AgentID Info\n");
            OutputError(L"NO AgentID Info\n");
            result = IC_NO_AGENTIDINFO;
            showHelp = true;
            break;
        }

        string address = GetHostAddress(g_argc, g_argv, connection);
        struct hostent *host = gethostbyname(address.c_str());
        if (host == nullptr)
        {
            ::OutputDebugString(L"ServiceExecutionThread - Can't resolve host address\n");
            OutputError(L"Can't resolve host address: %s\n", address.c_str());
            result = IC_NO_RESOLVE_HOSTADDR;
            break;
        }

        SetupHostPort(g_argc, g_argv, connection);

        if (cmdOptionExists(g_argv, g_argv + g_argc, L"-regservice") == true)
        {
            DoRegistration(apiKey, envInfo, address, connection.Getport(), intervalTimes);
            break;
        }

        struct sockaddr_in remoteServAddr;

        remoteServAddr.sin_family = host->h_addrtype;
        memcpy((char *)&remoteServAddr.sin_addr.s_addr, host->h_addr_list[0], host->h_length);
        remoteServAddr.sin_port = htons((u_short)connection.Getport());

        /* socket creation */
        udpSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
        if (udpSocket == INVALID_SOCKET) {
            OutputError(L"socket failed with error: %ld\n", WSAGetLastError());
            break;
        }

        ::OutputDebugString(L"ServiceExecutionThread - socket...\n");
        wstring appVersion = GetAppVersion(g_moduleFilePath.c_str(), NULL, NULL, NULL, NULL);
        OutputConsole(L"(%s) ServiceExecutionThread - data collect thread - start\n", appVersion.c_str());

        {
            thread processCpuMemThread([apiKey, envInfo, udpSocket, remoteServAddr, intervalTimes]()
            {
                ProcessCpuMemInfo(apiKey, envInfo, udpSocket, remoteServAddr, intervalTimes[0]);
            });

            thread processDiskThread([apiKey, envInfo, udpSocket, remoteServAddr, intervalTimes]()
            {
                ProcessDiskInfo(apiKey, envInfo, udpSocket, remoteServAddr, intervalTimes[1]);
            });

            thread updateThread([]()
            {
                DWORD oneday = 1000 * 60 * 60 * 24;

                while (true)
                {
                    ProcessLatestUpdate();

                    if (::WaitForSingleObject(g_killServiceEvent, oneday) == WAIT_TIMEOUT)
                    {
                        continue;
                    }

                    break;
                }
            });

            if (g_isConsoleApp == TRUE)
            {
                printf("Press any key to exit...\n");
                getchar();
            }
            else
            {
#if _DEBUG
                ::OutputDebugString(L"Service thread - WaitForSingleObject...\n");
#endif
                WaitForSingleObject(g_killServiceEvent, INFINITE);
            }

#if _DEBUG
            ::OutputDebugString(L"Service thread waiting...\n");
            Sleep(1000);
#endif

            if (processCpuMemThread.joinable() == true)
            {
                processCpuMemThread.join();
            }

            if (processDiskThread.joinable() == true)
            {
                processDiskThread.join();
            }

            if (updateThread.joinable() == true)
            {
                updateThread.join();
            }

#if _DEBUG
            ::OutputDebugString(L"All Service-threads exited...\n");
#endif
        }

        ::SetEvent(g_killSafeExitEvent);

#if _DEBUG
        ::OutputDebugString(L"Service thread detached\n");
#endif

    } while (false);

#if _DEBUG
    ::OutputDebugString(L"ServiceExecutionThread - data collect thread - ending\n");
#endif

    if (udpSocket != INVALID_SOCKET)
    {
        closesocket(udpSocket);
    }

    if (showHelp == true)
    {
        ShowHelp();
    }

    WSACleanup();

#if _DEBUG
    ::OutputDebugString(L"ServiceExecutionThread - data collect thread - ended\n");
#endif

    return result;
}

BOOL StartServiceThread()
{
    DWORD id;

    HANDLE threadHandle = CreateThread(0, 0,
        (LPTHREAD_START_ROUTINE)ServiceExecutionThread, 0, 0, &id);

    if (threadHandle == 0)
    {
        return FALSE;
    }
    else
    {
        g_serviceRunning = TRUE;

        ::CloseHandle(threadHandle);
        return TRUE;
    }
}