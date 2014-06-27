#include "stdafx.h"

#include "StringBuilder.h"
#include "StringSplit.h"

#include <Windows.h>
#include <WinBase.h>
#include <codecvt>

#include <strsafe.h>
#include <Shlwapi.h>

#include "InfoCollector.h"

#pragma comment(lib, "Version.lib")
#pragma comment(lib, "Shlwapi.lib")

#import <winhttp.dll> no_namespace, named_guids  
#include <msxml.h>

void FireRestartCommand()
{
    if (g_modulePath.size() == 0)
    {
        OutputError(L"No modulePath");
        return;
    }

    STARTUPINFO si;
    PROCESS_INFORMATION pi;

    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

#if defined(_WIN64)
    int platformId = 64;
#else
    int platformId = 32;
#endif

    wchar_t filePath[_MAX_PATH];
    HRESULT hr = StringCchPrintf(filePath, _MAX_PATH, L"%sic%d.exe", g_modulePath.c_str(), platformId);
    if (FAILED(hr) == TRUE)
    {
        OutputError(L"Error in composing file path");
        return;
    }

    wchar_t restartCmd[] = L"-restart";
    if (::CreateProcess(filePath, restartCmd, NULL, NULL, FALSE,
        0, NULL, NULL, &si, &pi) == 0)
    {
        OutputError(L"FAIL: %s %s", filePath, restartCmd);
        return;
    }
}

void RestartService()
{
    wchar_t cmdPath[_MAX_PATH];
    if (ExpandEnvironmentStrings(L"%windir%\\system32\\cmd.exe", cmdPath, _MAX_PATH) == 0)
    {
        return;
    }

    STARTUPINFO si;
    PROCESS_INFORMATION pi;

    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    wchar_t stopCmd[] = L"/C net stop StrawAgent";
    ::CreateProcess(cmdPath, stopCmd, NULL, NULL, FALSE,
        0, NULL, NULL, &si, &pi);

    ::WaitForSingleObject(pi.hProcess, 1000 * 10);

    wchar_t startCmd[] = L"/C net start StrawAgent";
    ::CreateProcess(cmdPath, startCmd, NULL, NULL, FALSE,
        0, NULL, NULL, &si, &pi);
}

void DoRegistration(wstring apiKey, wstring envKey, string remoteServAddr, int port, vector<int> intervalTimes)
{
    OutputConsole(L"Installing Service...\n");

    DoUnregistration();

    SC_HANDLE scm = OpenSCManager(0, 0, SC_MANAGER_CREATE_SERVICE);
    if (!scm)
    {
        OutputError(L"OpenSCManager fails! (%d)\n", GetLastError());
        return;
    }

    SC_HANDLE myService = NULL;
    HKEY hkey = NULL;
    wchar_t *pBuf = nullptr;

    do
    {
        wstring filePath = g_moduleFilePath;
        wchar_t safeFilePath[MAX_PATH];
        StringCchPrintf(safeFilePath, MAX_PATH, L"\"%s\"", filePath.c_str());

        OutputConsole(L"Opened Service Control Manager...\n");
        myService = CreateService(
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
            OutputError(L"CreateService fails! (%d)\n", GetLastError());
            break;
        }

        wchar_t szBuffer[MAX_PATH] = { 0 };

        StringBuilder sb;

        sb.push_back(L"asNTservice=1");
        sb.push_back(L"apiKey=" + apiKey);
        sb.push_back(L"envKey=" + envKey);
        sb.push_back("server=" + remoteServAddr);

        StringCchPrintf(szBuffer, MAX_PATH, L"port=%d", port);
        sb.push_back(szBuffer);

        if (g_debugMode == TRUE)
        {
            sb.push_back(L"debug=1");
        }

        if (RegOpenKey(HKEY_LOCAL_MACHINE, REG_SERVICE, &hkey) != ERROR_SUCCESS)
        {
            OutputError(L"RegOpenKey fails! (%d)\n", GetLastError());
            break;
        }

        int written = 0;
        pBuf = sb.ToStringMultiLine(&written);

        if (RegSetValueEx(hkey, L"Environment", 0, REG_MULTI_SZ, (const BYTE *)pBuf, written) != ERROR_SUCCESS)
        {
            OutputError(L"RegSetValueEx fails! (%d)\n", GetLastError());
            break;
        }

        OutputConsole(L"Service successfully installed.\n");

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
    OutputConsole(L"Removing Service...\n");

    SC_HANDLE scm = OpenSCManager(0, 0, SC_MANAGER_CREATE_SERVICE);
    if (!scm)
    {
        OutputError(L"OpenSCManager fails! (%d)\n", GetLastError());
        return;
    }

    SC_HANDLE myService = NULL;
    BOOL success;

    do
    {
        OutputConsole(L"Opened Service Control Manager...\n");
        myService = OpenService(scm, SERVICE_NAME, SERVICE_ALL_ACCESS | DELETE);

        if (!myService)
        {
            DWORD dwResult = GetLastError();
            if (dwResult = ERROR_SERVICE_DOES_NOT_EXIST)
            {
                OutputConsole(L"Service doesn't exist!\n");
                break;
            }

            OutputError(L"OpenService fails! (%d)\n", dwResult);
            break;
        }

        if (DoStopService() == FALSE)
        {
            break;
        }

        success = DeleteService(myService);
        if (success)
        {
            OutputConsole(L"Service successfully removed.\n");
        }
        else
        {
            OutputError(L"DeleteService Fails! (%d)\n", GetLastError());
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
        OutputError(L"OpenSCManager fails! (%d)\n", GetLastError());
        return;
    }

    SC_HANDLE myService = NULL;
    BOOL success;
    SERVICE_STATUS status;

    do
    {
        OutputConsole(L"Opened Service Control Manager...\n");
        myService = OpenService(scm, SERVICE_NAME, SERVICE_ALL_ACCESS);

        if (!myService)
        {
            OutputError(L"OpenService fails! (%d)\n", GetLastError());
            break;
        }

        success = QueryServiceStatus(myService, &status);
        if (!success)
        {
            OutputError(L"QueryServiceStatus fails! (%d)\n", GetLastError());
            break;
        }

        if (status.dwCurrentState != SERVICE_STOPPED && status.dwCurrentState != SERVICE_STOP_PENDING)
        {
            OutputError(L"Cannot start the service because it is already running\n");
            break;
        }

        success = StartService(myService, 0, NULL);
        if (!success)
        {
            OutputError(L"StartService fails! (%d)\n", GetLastError());
            break;
        }

        OutputConsole(L"Service started successfully!\n");

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

    SC_HANDLE schSCManager = OpenSCManager(
        NULL,                    // local computer
        NULL,                    // ServicesActive database 
        SC_MANAGER_ALL_ACCESS);  // full access rights 

    if (NULL == schSCManager)
    {
        OutputError(L"OpenSCManager failed (%d)\n", GetLastError());
        return FALSE;
    }

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
            OutputError(L"OpenService failed (%d)\n", GetLastError());
            break;
        }

        if (!QueryServiceStatusEx(
            schService,
            SC_STATUS_PROCESS_INFO,
            (LPBYTE)&ssp,
            sizeof(SERVICE_STATUS_PROCESS),
            &dwBytesNeeded))
        {
            OutputError(L"QueryServiceStatusEx failed (%d)\n", GetLastError());
            break;
        }

        if (ssp.dwCurrentState == SERVICE_STOPPED)
        {
            OutputConsole(L"Service is already stopped.\n");
            stopped = TRUE;
            break;
        }

        bool exitLoop = false;

        while (ssp.dwCurrentState == SERVICE_STOP_PENDING)
        {
            exitLoop = true;
            OutputConsole(L"Service stop pending...\n");

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
                OutputError(L"QueryServiceStatusEx failed (%d)\n", GetLastError());
                break;
            }

            if (ssp.dwCurrentState == SERVICE_STOPPED)
            {
                OutputConsole(L"Service stopped successfully.\n");
                stopped = TRUE;
                break;
            }

            if (GetTickCount() - dwStartTime > dwTimeout)
            {
                OutputError(L"Service stop timed out.\n");
                break;
            }
        }

        if (exitLoop == true)
        {
            break;
        }

        if (!ControlService(schService, SERVICE_CONTROL_STOP, (LPSERVICE_STATUS)&ssp))
        {
            OutputError(L"ControlService failed (%d)\n", GetLastError());
            break;
        }

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
                OutputError(L"QueryServiceStatusEx failed (%d)\n", GetLastError());
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
                OutputError(L"Wait timed out\n");
                exitLoop = true;
                break;
            }
        }

        if (exitLoop == true)
        {
            break;
        }

        stopped = TRUE;
        OutputConsole(L"Service stopped successfully\n");

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

void ProcessLatestUpdate()
{
    WORD majorVersion = 0;
    WORD minorVersion = 0;
    WORD buildNumber = 0;
    WORD revisionNumber = 0;

    wchar_t szTempFileName[MAX_PATH] { 0 };
    wchar_t lpTempPathBuffer[MAX_PATH] = { 0 };

    wstring thisFileName = g_moduleFilePath;
    wstring currentVersion;

    currentVersion = GetAppVersion(thisFileName.c_str(), &majorVersion, &minorVersion, &buildNumber, &revisionNumber);

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

        if (newUpdateVersion == currentVersion)
        {
            if (g_isConsoleApp == TRUE)
            {
                OutputConsole(L"This is the latest version (%s)\n", currentVersion.c_str());
            }
            break;
        }

        if (IsNewVersion(majorVersion, minorVersion, buildNumber, revisionNumber, newUpdateVersion) == FALSE)
        {
            if (g_isConsoleApp == TRUE)
            {
                OutputConsole(L"This is the latest version (%s)\n", L"1.0.0.4"); // currentVersion.c_str());
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
            OutputError(L"file not found: %s", location.c_str());
            break;
        }

        IStream*    pStream = NULL;
        BYTE        bBuffer[8192];
        DWORD       cb, cbRead, cbWritten;

        if (VT_UNKNOWN == V_VT(&varResponse) || VT_STREAM == V_VT(&varResponse))
        {
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

        DWORD dwRetVal = GetTempPath(MAX_PATH,          // length of the buffer
            lpTempPathBuffer); // buffer for path 
        if (dwRetVal > MAX_PATH || (dwRetVal == 0))
        {
            OutputError(L"GetTempPath failed (%d)", GetLastError());
            break;
        }

        dwRetVal = GetTempFileName(lpTempPathBuffer, // directory for tmp files
            L"straw",     // temp file name prefix 
            0,                // create unique name 
            szTempFileName);  // buffer for name 
        if (dwRetVal == 0)
        {
            OutputError(L"GetTempFileName failed (%d)", GetLastError());
            break;
        }

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
            OutputError(L"Can't open a file: %s", szTempFileName);
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
                    OutputError(L"WriteFile fails with 0x%08lx\n", HRESULT_FROM_WIN32(GetLastError()));
                    succeed = false;
                    break;
                }

                hr = pStream->Read(bBuffer, cb, &cbRead);
            }
        }

        CloseHandle(hFile);
        pStream->Release();
        VariantClear(&varResponse);

        if (succeed == true)
        {
            wstring oldFileName = thisFileName;
            oldFileName += currentVersion + L".bak";

            ::DeleteFile(oldFileName.c_str());

            if (MoveFile(thisFileName.c_str(), oldFileName.c_str()) == FALSE)
            {
                OutputError(L"Backup fails (%d)", GetLastError());
                break;
            }

            if (MoveFile(szTempFileName, thisFileName.c_str()) == FALSE)
            {
                OutputError(L"Update fails (%d)", GetLastError());
                break;
            }

            FireRestartCommand();
        }

    } while (false);

    if (::PathFileExists(szTempFileName) == TRUE)
    {
        ::DeleteFile(szTempFileName);
    }

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

wstring GetAppVersion(const wchar_t *pModuleFileName, WORD *MajorVersion, WORD *MinorVersion, WORD *BuildNumber, WORD *RevisionNumber)
{
    DWORD dwHandle, dwLen;
    UINT BufLen;
    wchar_t *lpData;
    VS_FIXEDFILEINFO *pFileInfo;

    dwLen = GetFileVersionInfoSize(pModuleFileName, &dwHandle);

    if (!dwLen)
    {
        return L"";
    }

    lpData = new wchar_t[dwLen];

    if (lpData == NULL)
    {
        return L"";
    }

    wchar_t oldVersion[60];

    do
    {
        if (!GetFileVersionInfo(pModuleFileName, dwHandle, dwLen, lpData))
        {
            break;
        }

        if (VerQueryValue(lpData, L"\\", (LPVOID *)&pFileInfo, (PUINT)&BufLen))
        {
            if (MajorVersion != NULL)
            {
                *MajorVersion = HIWORD(pFileInfo->dwFileVersionMS);
            }
            if (MinorVersion != NULL)
            {
                *MinorVersion = LOWORD(pFileInfo->dwFileVersionMS);
            }

            if (BuildNumber != NULL)
            {
                *BuildNumber = HIWORD(pFileInfo->dwFileVersionLS);
            }

            if (RevisionNumber != NULL)
            {
                *RevisionNumber = LOWORD(pFileInfo->dwFileVersionLS);
            }

            StringCchPrintf(oldVersion, 60, L"%d.%d.%d.%d", HIWORD(pFileInfo->dwFileVersionMS),
                                                            LOWORD(pFileInfo->dwFileVersionMS), 
                                                            HIWORD(pFileInfo->dwFileVersionLS),
                                                            LOWORD(pFileInfo->dwFileVersionLS));
        }

    } while (false);

    delete[] lpData;

    return oldVersion;
}

wstring GetNewVersion(wstring txt)
{
    wstring startTag = L"<version>";

    size_t spos = txt.find(startTag);
    if (spos == wstring::npos)
    {
        return L"";
    }

    size_t epos = txt.find(L"</version>", spos);
    if (epos == wstring::npos)
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

    size_t spos = txt.find(startTag);
    if (spos == wstring::npos)
    {
        return L"";
    }

    size_t epos = txt.find(L"</location>", spos);
    if (epos == wstring::npos)
    {
        return L"";
    }

    size_t len = wcslen(startTag);
    spos += len;
    return txt.substr(spos, epos - spos);
}

BOOL IsNewVersion(WORD majorVersion, WORD minorVersion, WORD buildNumber, WORD revisionNumber, wstring newUpdateVersion)
{
    StringSplit text;
    text.SplitString(newUpdateVersion, L".");

    if (text.GetCount() != 4)
    {
        return FALSE;
    }

    int newMajor = ::_wtoi(text.GetValue(0).c_str());
    int newMinor = ::_wtoi(text.GetValue(1).c_str());
    int newBuild = ::_wtoi(text.GetValue(2).c_str());
    int newRevision = ::_wtoi(text.GetValue(3).c_str());

    if (newMajor > majorVersion)
    {
        return TRUE;
    }
    else if (newMajor == majorVersion)
    {
        if (newMinor > minorVersion)
        {
            return TRUE;
        }
        else if (newMinor == minorVersion)
        {
            if (newBuild > buildNumber)
            {
                return TRUE;
            }
            else if (newBuild == buildNumber)
            {
                if (newRevision > revisionNumber)
                {
                    return TRUE;
                }
            }
        }
    }

    return FALSE;
}