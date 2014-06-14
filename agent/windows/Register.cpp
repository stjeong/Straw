#include "stdafx.h"

#include "StringBuilder.h"
#include "StringSplit.h"

#include <Windows.h>
#include <WinBase.h>
#include <codecvt>

#include <strsafe.h>

#include "InfoCollector.h"

#pragma comment(lib, "Version.lib")

void DoRegistration(wstring apiKey, wstring envKey, string remoteServAddr, vector<int> intervalTimes)
{
    printf("Installing Service...\n");

    SC_HANDLE scm = OpenSCManager(0, 0, SC_MANAGER_CREATE_SERVICE);
    if (!scm)
    {
        printf("OpenSCManager fails! (%d)\n", GetLastError());
        return;
    }

    SC_HANDLE myService = NULL;
    HKEY hkey = NULL;
    wchar_t *pBuf = nullptr;

    do
    {
        wchar_t filePath[MAX_PATH];
        ::GetModuleFileName(NULL, filePath, MAX_PATH);
        wchar_t safeFilePath[MAX_PATH];
        StringCchPrintf(safeFilePath, MAX_PATH, L"\"%s\"", filePath);

        printf("Opened Service Control Manager...\n");
        SC_HANDLE myService = CreateService(
            scm, SERVICE_NAME, // the internal service name used by the SCM
            L"StrawAgent Service",  // the external label seen in the Service Control applet
            SERVICE_ALL_ACCESS,  // We want full access to control the service
            SERVICE_WIN32_OWN_PROCESS,  // The service is a single app and not a driver
            SERVICE_AUTO_START,  // The service will be started by us manually
            SERVICE_ERROR_NORMAL,  // If error during service start, don't misbehave.
            safeFilePath,
            0, 0, 0, 0, 0);

        if (!myService)
        {
            printf("CreateService fails! (%d)\n", GetLastError());
            break;
        }

        char szBuffer[MAX_PATH] = { 0 };

        StringBuilder sb;

        sb.push_back(L"asNTservice=1");
        sb.push_back(L"apiKey=" + apiKey);
        sb.push_back(L"envKey=" + envKey);
        sb.push_back("server=" + remoteServAddr);

        if (RegOpenKey(HKEY_LOCAL_MACHINE, REG_SERVICE, &hkey) != ERROR_SUCCESS)
        {
            printf("RegOpenKey fails! (%d)\n", GetLastError());
            break;
        }

        int written = 0;
        pBuf = sb.ToStringMultiLine(&written);

        if (RegSetValueEx(hkey, L"Environment", 0, REG_MULTI_SZ, (const BYTE *)pBuf, written) != ERROR_SUCCESS)
        {
            printf("RegSetValueEx fails! (%d)\n", GetLastError());
            break;
        }

        printf("%S", szBuffer);
        
        printf("Service successfully installed.\n");

    } while (false);

    if (pBuf != nullptr)
    {
        delete[] pBuf;
    }

    if (hkey != NULL)
    {
        RegCloseKey(hkey);
    }

    if (myService != NULL)
    {
        CloseServiceHandle(myService);
    }

    if (scm != NULL)
    {
        CloseServiceHandle(scm);
    }
}

void DoUnregistration()
{
    printf("Removing Service...\n");

    SC_HANDLE scm = OpenSCManager(0, 0, SC_MANAGER_CREATE_SERVICE);
    if (!scm)
    {
        printf("OpenSCManager fails! (%d)\n", GetLastError());
        return;
    }

    SC_HANDLE myService = NULL;
    BOOL success;

    do
    {
        printf("Opened Service Control Manager...\n");
        SC_HANDLE myService = OpenService(scm, SERVICE_NAME, SERVICE_ALL_ACCESS | DELETE);

        if (!myService)
        {
            printf("OpenService fails! (%d)\n", GetLastError());
            break;
        }

        if (DoStopService() == FALSE)
        {
            break;
        }

        // Remove the service
        success = DeleteService(myService);
        if (success)
        {
            printf("Service successfully removed.\n");
        }
        else
        {
            printf("DeleteService Fails! (%d)\n", GetLastError());
            break;
        }

    } while (false);

    if (myService != NULL)
    {
        CloseServiceHandle(myService);
    }

    if (scm != NULL)
    {
        CloseServiceHandle(scm);
    }
}

void DoStartService()
{
    SC_HANDLE scm = OpenSCManager(0, 0, SC_MANAGER_ALL_ACCESS | GENERIC_WRITE);
    if (!scm)
    {
        printf("OpenSCManager fails! (%d)\n", GetLastError());
        return;
    }

    SC_HANDLE myService = NULL;
    BOOL success;
    SERVICE_STATUS status;

    do
    {
        printf("Opened Service Control Manager...\n");
        SC_HANDLE myService = OpenService(scm, SERVICE_NAME, SERVICE_ALL_ACCESS);

        if (!myService)
        {
            printf("OpenService fails! (%d)\n", GetLastError());
            break;
        }

        success = QueryServiceStatus(myService, &status);
        if (!success)
        {
            printf("QueryServiceStatus fails! (%d)\n", GetLastError());
            break;
        }

        if (status.dwCurrentState != SERVICE_STOPPED && status.dwCurrentState != SERVICE_STOP_PENDING)
        {
            printf("Cannot start the service because it is already running\n");
            break;
        }

        success = StartService(myService, 0, NULL);
        if (!success)
        {
            printf("StartService fails! (%d)\n", GetLastError());
            break;
        }

        printf("Service started successfully!\n");

    } while (false);

    if (myService != NULL)
    {
        CloseServiceHandle(myService);
    }

    if (scm != NULL)
    {
        CloseServiceHandle(scm);
    }
}

BOOL DoStopService()
{
    SERVICE_STATUS_PROCESS ssp;
    DWORD dwStartTime = GetTickCount();
    DWORD dwBytesNeeded;
    DWORD dwTimeout = 30000; // 30-second time-out
    DWORD dwWaitTime;
    BOOL stopped = FALSE;

    // Get a handle to the SCM database. 

    SC_HANDLE schSCManager = OpenSCManager(
        NULL,                    // local computer
        NULL,                    // ServicesActive database 
        SC_MANAGER_ALL_ACCESS);  // full access rights 

    if (NULL == schSCManager)
    {
        printf("OpenSCManager failed (%d)\n", GetLastError());
        return FALSE;
    }

    // Get a handle to the service.
    SC_HANDLE schService = OpenService(
        schSCManager,         // SCM database 
        SERVICE_NAME,            // name of service 
        SERVICE_STOP |
        SERVICE_QUERY_STATUS |
        SERVICE_ENUMERATE_DEPENDENTS);

    do
    {
        if (schService == NULL)
        {
            printf("OpenService failed (%d)\n", GetLastError());
            break;
        }

        // Make sure the service is not already stopped.
        if (!QueryServiceStatusEx(
            schService,
            SC_STATUS_PROCESS_INFO,
            (LPBYTE)&ssp,
            sizeof(SERVICE_STATUS_PROCESS),
            &dwBytesNeeded))
        {
            printf("QueryServiceStatusEx failed (%d)\n", GetLastError());
            break;
        }

        if (ssp.dwCurrentState == SERVICE_STOPPED)
        {
            printf("Service is already stopped.\n");
            stopped = TRUE;
            break;
        }

        // If a stop is pending, wait for it.
        bool exitLoop = false;

        while (ssp.dwCurrentState == SERVICE_STOP_PENDING)
        {
            exitLoop = true;
            printf("Service stop pending...\n");

            // Do not wait longer than the wait hint. A good interval is 
            // one-tenth of the wait hint but not less than 1 second  
            // and not more than 10 seconds. 

            dwWaitTime = ssp.dwWaitHint / 10;

            if (dwWaitTime < 1000)
                dwWaitTime = 1000;
            else if (dwWaitTime > 10000)
                dwWaitTime = 10000;

            Sleep(dwWaitTime);

            if (!QueryServiceStatusEx(
                schService,
                SC_STATUS_PROCESS_INFO,
                (LPBYTE)&ssp,
                sizeof(SERVICE_STATUS_PROCESS),
                &dwBytesNeeded))
            {
                printf("QueryServiceStatusEx failed (%d)\n", GetLastError());
                break;
            }

            if (ssp.dwCurrentState == SERVICE_STOPPED)
            {
                printf("Service stopped successfully.\n");
                stopped = TRUE;
                break;
            }

            if (GetTickCount() - dwStartTime > dwTimeout)
            {
                printf("Service stop timed out.\n");
                break;
            }
        }

        if (exitLoop == true)
        {
            break;
        }

        // Send a stop code to the service.
        if (!ControlService(
            schService,
            SERVICE_CONTROL_STOP,
            (LPSERVICE_STATUS)&ssp))
        {
            printf("ControlService failed (%d)\n", GetLastError());
            break;
        }

        // Wait for the service to stop.
        while (ssp.dwCurrentState != SERVICE_STOPPED)
        {
            Sleep(ssp.dwWaitHint);
            if (!QueryServiceStatusEx(
                schService,
                SC_STATUS_PROCESS_INFO,
                (LPBYTE)&ssp,
                sizeof(SERVICE_STATUS_PROCESS),
                &dwBytesNeeded))
            {
                printf("QueryServiceStatusEx failed (%d)\n", GetLastError());
                exitLoop = true;
                break;
            }

            if (ssp.dwCurrentState == SERVICE_STOPPED)
            {
                stopped = TRUE;
                break;
            }

            if (GetTickCount() - dwStartTime > dwTimeout)
            {
                printf("Wait timed out\n");
                exitLoop = true;
                break;
            }
        }

        if (exitLoop == true)
        {
            break;
        }

        stopped = TRUE;
        printf("Service stopped successfully\n");

    } while (false);

    if (schService != NULL)
    {
        CloseServiceHandle(schService);
    }

    if (schSCManager != NULL)
    {
        CloseServiceHandle(schSCManager);
    }

    return stopped;
}

#import <winhttp.dll> no_namespace, named_guids  
#include <msxml.h>

void ProcessLatestUpdate(bool isConsoleApp)
{
    WORD MajorVersion = 0;
    WORD MinorVersion = 0;
    WORD BuildNumber = 0;
    WORD RevisionNumber = 0;

    wchar_t thisFileName[4096];
    wchar_t oldVersion[60];
    GetModuleFileName(NULL, thisFileName, 4096);

    GetAppVersion(thisFileName,
        &MajorVersion,
        &MinorVersion,
        &BuildNumber,
        &RevisionNumber);

    StringCchPrintf(oldVersion, 60, L"%d.%d.%d.%d", MajorVersion, MinorVersion, BuildNumber, RevisionNumber);

    IWinHttpRequest *  pIWinHttpRequest = NULL;
    BSTR            bstrResponse = NULL;

    HRESULT hr = CoInitialize(NULL);

    do
    {
        VARIANT         varFalse;
        VARIANT         varEmpty;

        CLSID           clsid;

        VariantInit(&varFalse);
        V_VT(&varFalse) = VT_BOOL;
        V_BOOL(&varFalse) = VARIANT_FALSE;

        VariantInit(&varEmpty);
        V_VT(&varEmpty) = VT_ERROR;

        hr = CLSIDFromProgID(L"WinHttp.WinHttpRequest.5.1", &clsid);

        if (SUCCEEDED(hr) == FALSE)
        {
            break;
        }
        
        hr = CoCreateInstance(clsid, NULL, CLSCTX_INPROC_SERVER,
            IID_IWinHttpRequest, (void **)&pIWinHttpRequest);
        
        if (SUCCEEDED(hr) == FALSE)
        {
            break;
        }

        BSTR bstrMethod = SysAllocString(L"GET");
        BSTR bstrUrl = SysAllocString(UPDATE_CHECK_URL);
        hr = pIWinHttpRequest->Open(bstrMethod, bstrUrl, varFalse);
        SysFreeString(bstrMethod);
        SysFreeString(bstrUrl);

        if (SUCCEEDED(hr) == FALSE)
        {
            break;
        }

        hr = pIWinHttpRequest->Send(varEmpty);
        if (SUCCEEDED(hr) == FALSE)
        {
            break;
        }

        hr = pIWinHttpRequest->get_ResponseText(&bstrResponse);

        wstring txt = bstrResponse;
        wstring newUpdateVersion = GetNewVersion(txt);
        if (newUpdateVersion.length() == 0)
        {
            break;
        }

        if (newUpdateVersion == oldVersion)
        {
            if (isConsoleApp == true)
            {
                printf("This is the latest version\n");
            }
            break;
        }

        // if new, update latest version.
        // if old, roll-back last stable version.

        bool is32bit = sizeof(char *) == 4;
        wstring location = GetUpdateLocation(txt, is32bit);

        VariantInit(&varEmpty);
        V_VT(&varEmpty) = VT_ERROR;

        bstrMethod = SysAllocString(L"GET");
        bstrUrl = SysAllocString(location.c_str());
        pIWinHttpRequest->Open(bstrMethod, bstrUrl, varFalse);
        SysFreeString(bstrMethod);
        SysFreeString(bstrUrl);

        if (SUCCEEDED(hr) == FALSE)
        {
            break;
        }

        hr = pIWinHttpRequest->Send(varEmpty);
        if (SUCCEEDED(hr) == FALSE)
        {
            break;
        }

        VARIANT varResponse;
        VariantInit(&varResponse);
        hr = pIWinHttpRequest->get_ResponseStream(&varResponse);
        if (SUCCEEDED(hr) == FALSE)
        {
            break;
        }

        txt = pIWinHttpRequest->GetResponseHeader(L"Content-Type").operator const wchar_t *();
        if (txt.find(L"text/html") != -1)
        {
            wprintf(L"file not found: %s", location.c_str());
            break;
        }

        IStream*    pStream = NULL;
        BYTE        bBuffer[8192];
        DWORD       cb, cbRead, cbWritten;
        // Check that an IStream was received.
        if (VT_UNKNOWN == V_VT(&varResponse) || VT_STREAM == V_VT(&varResponse))
        {
            // Get IStream interface pStream.
            hr = V_UNKNOWN(&varResponse)->QueryInterface(IID_IStream,
                reinterpret_cast<void**>(&pStream));
        }
        else
        {
            break;
        }

        if (SUCCEEDED(hr) == FALSE)
        {
            break;
        }

        wchar_t szTempFileName[MAX_PATH];
        bool succeed = true;

        HANDLE hFile = CreateFile(szTempFileName,
            GENERIC_WRITE,                // Open for writing. 
            0,                            // Do not share. 
            NULL,                         // No security. 
            CREATE_ALWAYS,                // Overwrite existing.
            FILE_ATTRIBUTE_NORMAL,        // Normal file.
            NULL);                        // No attribute template.
        if (hFile == INVALID_HANDLE_VALUE)
        {
            wprintf(L"Can't open a file: %s", szTempFileName);
            break;
        }
        else
        {
            cb = sizeof(bBuffer);
            hr = pStream->Read(bBuffer, cb, &cbRead);
            while (SUCCEEDED(hr) && 0 != cbRead)
            {
                if (!WriteFile(hFile, bBuffer,
                    cbRead, &cbWritten, NULL))
                {
                    printf("WriteFile fails with 0x%08lx\n", HRESULT_FROM_WIN32(GetLastError()));
                    succeed = false;
                    break;
                }

                hr = pStream->Read(bBuffer, cb, &cbRead);
            }
        }

        if (succeed == true)
        {
            wstring oldFileName = thisFileName;
            oldFileName += oldVersion;
            MoveFile(thisFileName, oldFileName.c_str());

            MoveFile(szTempFileName, thisFileName);
        }

        CloseHandle(hFile);
        pStream->Release();
        VariantClear(&varResponse);

    } while (false);

    // Release memory.
    if (pIWinHttpRequest)
    {
        pIWinHttpRequest->Release();
    }

    if (bstrResponse)
    {
        SysFreeString(bstrResponse);
    }

    CoUninitialize();
}

BOOL GetAppVersion(wchar_t *LibName, WORD *MajorVersion, WORD *MinorVersion, WORD *BuildNumber, WORD *RevisionNumber)
{
    DWORD dwHandle, dwLen;
    UINT BufLen;
    LPTSTR lpData;
    VS_FIXEDFILEINFO *pFileInfo;
    dwLen = GetFileVersionInfoSize(LibName, &dwHandle);
    if (!dwLen)
        return FALSE;

    lpData = (LPTSTR)malloc(dwLen);
    if (!lpData)
        return FALSE;

    if (!GetFileVersionInfo(LibName, dwHandle, dwLen, lpData))
    {
        free(lpData);
        return FALSE;
    }
    if (VerQueryValue(lpData, L"\\", (LPVOID *)&pFileInfo, (PUINT)&BufLen))
    {
        *MajorVersion = HIWORD(pFileInfo->dwFileVersionMS);
        *MinorVersion = LOWORD(pFileInfo->dwFileVersionMS);
        *BuildNumber = HIWORD(pFileInfo->dwFileVersionLS);
        *RevisionNumber = LOWORD(pFileInfo->dwFileVersionLS);
        free(lpData);
        return TRUE;
    }
    free(lpData);
    return FALSE;
}

wstring GetNewVersion(wstring txt)
{
    wstring startTag = L"<version>";

    int spos = txt.find(startTag);
    if (spos == -1)
    {
        return L"";
    }

    int epos = txt.find(L"</version>", spos);
    if (epos == -1)
    {
        return L"";
    }

    spos += startTag.length();
    return txt.substr(spos, epos - spos);
}

wstring GetUpdateLocation(wstring txt, bool is32bit)
{
    wchar_t startTag[MAX_PATH] = { 0 };

    StringCchPrintf(startTag, MAX_PATH, L"<location platform=\"%s\">", is32bit == true ? L"win32" : L"win64");

    int spos = txt.find(startTag);
    if (spos == -1)
    {
        return L"";
    }

    int epos = txt.find(L"</location>", spos);
    if (epos == -1)
    {
        return L"";
    }

    int len = wcslen(startTag);
    spos += len;
    return txt.substr(spos, epos - spos);
}