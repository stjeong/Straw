// InfoCollector.cpp : Defines the entry point for the console application.

#include "stdafx.h"

#include "StringBuilder.h"
#include "StringSplit.h"

#include <Windows.h>
#include <WinBase.h>
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
wstring g_modulePath = L"";

int _tmain(int argc, _TCHAR* argv[])
{
    bool isConsoleApp = IsConsoleApp();

    g_argc = argc;
    g_argv = argv;

    if (isConsoleApp == true)
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
        // Register the service with the Service Control Manager
        success = StartServiceCtrlDispatcher(serviceTable);
        if (!success)
        {
            // This error message would not show up unless we had the "allow service to
            // interact with desktop" option checked, which is unlikely for a console mode
            // app, but we'll print an error message anyway just in case there's someone
            // around to see it.
            OutputError(L"StartServiceCtrlDispatcher fails! (%d)", GetLastError());
        }
    }
}

void SetupHostPort(int argc, _TCHAR* argv[], ConnectionInfo &connection)
{
    wstring txt;

    if (IsConsoleApp() == false)
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

    if (IsConsoleApp() == false)
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

#if defined(_DEBUG)
    ::OutputDebugString((data + L"\n").c_str());
#endif

    std::wstring_convert<std::codecvt_utf8<wchar_t>> myconv;
    string utfData = myconv.to_bytes(data);

    sendto(socketHandle, (const char *)utfData.c_str(), (int)utfData.length(), 0,
        (struct sockaddr *) &remoteServAddr,
        sizeof(remoteServAddr));
}

wstring GetApiKey(int argc, _TCHAR* argv[])
{
    if (IsConsoleApp() == false)
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
    if (IsConsoleApp() == false)
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
    wstring txt = L"2,5";

    if (cmdOptionExists(argv, argv + argc, L"-d") == true)
    {
        txt = getCmdOption(argv, argv + argc, L"-d");
    }

    StringSplit split;
    split.SplitString(txt, L",");
    vector<int> intervalTimes;

    for (size_t i = 0; i < split.GetCount(); i ++)
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
    bool isConsoleApp = IsConsoleApp();

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
                    StringCchPrintf(buf, 40, L"}, \"%s\": %.2f", CpuInfo::Members::total.c_str(), totalUsage / 100);
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

            if (isConsoleApp == true)
            {
                printf(".");
            }
        }

        Sleep(sleepTime);
    }
}

void ProcessDiskInfo(wstring apiKey, wstring envKey, SOCKET socketHandle, sockaddr_in remoteServAddr, int interval)
{
    StringBuilder sb;

    int sleepTime = interval * 1000;

    while (true)
    {
        if (g_servicePaused == FALSE)
        {
            sb.clear();
        }

        Sleep(sleepTime);
    }

}

void ShowHelp()
{
#if defined(_WIN64)
    int platformId = 64;
#else
    int platformId = 32;
#endif

    wstring appVersion = GetAppVersion(g_modulePath.c_str(), NULL, NULL, NULL, NULL);

    wprintf(L"ic%d.exe (ver %s)\n", platformId, appVersion.c_str());
    printf("ic%d.exe -h\n", platformId);
    printf("ic%d.exe -key [apikey] -s [hostaddress] -id [agentid] -d [interval-sec,[...]]\n", platformId);
    printf("\n");
    printf("samples:\n");
    printf("    ic%d.exe -key 8CFDDE2478 -s 192.168.0.5\n", platformId);
    printf("        apikey: 8CFDDE2478\n");
    printf("        data server: 192.168.0.5:80\n");
    printf("        (optional: agent id by default - machine name)\n");
    printf("\n");
    printf("    ic%d.exe -key 8CFDDE2478 -s 192.168.0.5 -port 8282\n", platformId);
    printf("        apikey: 8CFDDE2478\n");
    printf("        data server: 192.168.0.5:8282\n");
    printf("        (optional: agent id by default - machine name)\n");
    printf("\n");
    printf("    ic%d.exe -key 8CFDDE2478 -s 192.168.0.5 -id mypc100\n", platformId);
    printf("        apikey: 8CFDDE2478\n");
    printf("        data server: 192.168.0.5:80\n");
    printf("        id: mypc100\n");
    printf("\n");
    printf("    ic%d.exe -key 8CFDDE2478 -s 192.168.0.5 -id mypc100  -regservice\n", platformId);
    printf("        Register as NT Service with this info,\n");
    printf("            apikey: 8CFDDE2478\n");
    printf("            data server: 192.168.0.5:80\n");
    printf("            id: mypc100\n");
    printf("\n");
    printf("    ic%d.exe -unreg\n", platformId);
    printf("        Unregister NT Service\n");
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

#if _DEBUG
    ::OutputDebugString(txt.c_str());
#endif
}

//   ServiceMain -
//   ServiceMain is called when the Service Control Manager wants to
// launch the service.  It should not return until the service has
// stopped. To accomplish this, for this example, it waits blocking
// on an event just before the end of the function.  That event gets
// set by the function which terminates the service above.  Also, if
// there is an error starting the service, ServiceMain returns immediately
// without launching the main service thread, terminating the service.
VOID ServiceMain(DWORD argc, LPTSTR *argv)
{
#if _DEBUG
    ::OutputDebugString(L"ServiceMain started.\n");
#endif

    BOOL success;
    // First we must call the Registration function
    g_serviceStatusHandle = RegisterServiceCtrlHandler(SERVICE_NAME,
        (LPHANDLER_FUNCTION)ServiceCtrlHandler);

    if (!g_serviceStatusHandle)
    {
        OutputError(L"RegisterServiceCtrlHandler fails! (%d)", GetLastError());
        return;
    }

    do
    {
        // Next Notify the Service Control Manager of progress
        success = UpdateSCMStatus(SERVICE_START_PENDING, NO_ERROR, 0, 1, 5000);
        if (!success)
        {
            OutputError(L"1. UpdateSCMStatus fails! (%d)", GetLastError());
            break;
        }

        // Now create the our service termination event to block on
        g_killServiceEvent = CreateEvent(0, TRUE, FALSE, 0);
        if (!g_killServiceEvent)
        {
            OutputError(L"CreateEvent fails! (%d)", GetLastError());
            break;
        }

        // Notify the SCM of progress again
        success = UpdateSCMStatus(SERVICE_START_PENDING, NO_ERROR, 0, 2, 1000);
        if (!success)
        {
            OutputError(L"2. UpdateSCMStatus fails! (%d)", GetLastError());
            break;
        }

        // Notify the SCM of progress again...
        success = UpdateSCMStatus(SERVICE_START_PENDING, NO_ERROR, 0, 3, 5000);
        if (!success)
        {
            OutputError(L"3. UpdateSCMStatus fails! (%d)", GetLastError());
            break;
        }

        // Start the service execution thread by calling our StartServiceThread function...
        success = StartServiceThread();
        if (!success)
        {
            OutputError(L"StartServiceThread fails! (%d)", GetLastError());
            break;
        }

        // The service is now running.  Notify the SCM of this fact.
        success = UpdateSCMStatus(SERVICE_RUNNING, NO_ERROR, 0, 0, 0);
        if (!success)
        {
            OutputError(L"4. UpdateSCMStatus fails! (%d)", GetLastError());
            break;
        }

#if _DEBUG
        ::OutputDebugString(L"WaitForSingleObject - started.\n");
#endif

        // Now just wait for our killed service signal, and then exit, which
        // terminates the service!
        WaitForSingleObject(g_killServiceEvent, INFINITE);

#if _DEBUG
        ::OutputDebugString(L"WaitForSingleObject - end.\n");
#endif
    } while (false);
}

//   Handles the events dispatched by the Service Control Manager.
VOID ServiceCtrlHandler(DWORD controlCode)
{
    BOOL success;
    DWORD serviceCurrentStatus = 0;

    switch (controlCode)
    {
        // There is no START option because
        // ServiceMain gets called on a start
        // Pause the service
    case SERVICE_CONTROL_PAUSE:
        if (g_serviceRunning && !g_servicePaused)
        {
            // Tell the SCM we're about to Pause.
            success = UpdateSCMStatus(SERVICE_PAUSE_PENDING, NO_ERROR, 0, 1, 1000);
            g_servicePaused = TRUE;
            serviceCurrentStatus = SERVICE_PAUSED;
        }
        break;

        // Resume from a pause
    case SERVICE_CONTROL_CONTINUE:
        if (g_serviceRunning && g_servicePaused)
        {
            // Tell the SCM we're about to Resume.
            success = UpdateSCMStatus(SERVICE_CONTINUE_PENDING, NO_ERROR, 0, 1, 1000);
            g_servicePaused = FALSE;
            serviceCurrentStatus = SERVICE_RUNNING;
        }
        break;
        // Update the current status for the SCM.

    case SERVICE_CONTROL_INTERROGATE:
        // This does nothing, here we will just fall through to the end
        // and send our current status.
        break;

        // For a shutdown, we can do cleanup but it must take place quickly
        // because the system will go down out from under us.
        // For this app we have time to stop here, which I do by just falling
        // through to the stop message.
    case SERVICE_CONTROL_SHUTDOWN:
        // Stop the service
    case SERVICE_CONTROL_STOP:
        // Tell the SCM we're about to Stop.
        serviceCurrentStatus = SERVICE_STOP_PENDING;
        success = UpdateSCMStatus(SERVICE_STOP_PENDING, NO_ERROR, 0, 1, 5000);
        KillService();
        return;

    default:
        break;
    }

    UpdateSCMStatus(serviceCurrentStatus, NO_ERROR, 0, 0, 0);
}

// This function updates the service status for the SCM
BOOL UpdateSCMStatus(DWORD dwCurrentState,
    DWORD dwWin32ExitCode,
    DWORD dwServiceSpecificExitCode,
    DWORD dwCheckPoint,
    DWORD dwWaitHint)
{
    BOOL success;
    SERVICE_STATUS serviceStatus;
    // Fill in all of the SERVICE_STATUS fields
    serviceStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
    serviceStatus.dwCurrentState = dwCurrentState;
    // If in the process of something, then accept
    // no control events, else accept anything
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
    // if a specific exit code is defines, set up
    // the Win32 exit code properly
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
    // Pass the status record to the SCM
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
    // Set the event that is blocking ServiceMain
    // so that ServiceMain can return
    SetEvent(g_killServiceEvent);
}

// ServiceExecutionThread -
//   This is the main thread of execution for the
// service while it is running.
DWORD ServiceExecutionThread(LPDWORD param)
{
#if _DEBUG
    ::OutputDebugString(L"ServiceExecutionThread\n");
#endif

    IntializeSystemInfo();

    bool isConsoleApp = IsConsoleApp();

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
        vector<int> intervalTimes;
        ConnectionInfo connection;

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
            ProcessLatestUpdate(isConsoleApp);
            break;
        }

        apiKey = GetApiKey(g_argc, g_argv);
        envInfo = GetEnvInfo(g_argc, g_argv);
        intervalTimes = GetIntervalTime(g_argc, g_argv);

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

        string address = GetHostAddress(g_argc, g_argv, connection);
        struct hostent *host = gethostbyname(address.c_str());
        if (host == nullptr)
        {
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
            wprintf(L"socket failed with error: %ld\n", WSAGetLastError());
            break;
        }

#if _DEBUG
        ::OutputDebugString(L"ServiceExecutionThread - data collect thread - start\n");
#endif

        thread processCpuMemThread([apiKey, envInfo, udpSocket, remoteServAddr, intervalTimes]()
        {
            ProcessCpuMemInfo(apiKey, envInfo, udpSocket, remoteServAddr, intervalTimes[0]);
        });

        thread processDiskThread([apiKey, envInfo, udpSocket, remoteServAddr, intervalTimes]()
        {
            ProcessDiskInfo(apiKey, envInfo, udpSocket, remoteServAddr, intervalTimes[1]);
        });

        thread updateThread([isConsoleApp]()
        {
            DWORD oneday = 1000 * 60 * 60 * 24;

            while (true)
            {
                ProcessLatestUpdate(isConsoleApp);
                Sleep(oneday);
            }
        });

        if (isConsoleApp == true)
        {
            printf("Press any key to exit...\n");
            getchar();
        }
        else
        {
            WaitForSingleObject(g_killServiceEvent, INFINITE);
        }

#if _DEBUG
        ::OutputDebugString(L"Service thread detaching...\n");
#endif
        processCpuMemThread.detach();
        processDiskThread.detach();
        updateThread.detach();
#if _DEBUG
        ::OutputDebugString(L"Service thread detached\n");
#endif

    } while (false);

#if _DEBUG
    ::OutputDebugString(L"ServiceExecutionThread - data collect thread - end\n");
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

    return result;
}

// StartService -
//   This starts the service by creating its execution thread.
BOOL StartServiceThread()
{
    DWORD id;

    // Start the service's thread
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