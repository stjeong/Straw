#pragma once

#include "stdafx.h"

wstring GetApiKey(int argc, _TCHAR* argv[]);
wstring GetUID(int argc, _TCHAR* argv[]);
wstring GetEnvInfo(int argc, _TCHAR* argv[]);
string GetHostAddress(int argc, _TCHAR* argv[], ConnectionInfo &connection);
void SetupHostPort(int argc, _TCHAR* argv[], ConnectionInfo &connection);

wstring GetComputerName();
vector<int> GetIntervalTime(int argc, _TCHAR* argv[]);

void ShowHelp();
void SendToServer(SOCKET socketHandle, sockaddr_in remoteServAddr, StringBuilder &sb);

void ProcessCpuMemInfo(wstring apiKey, wstring envKey, SOCKET socketHandle, sockaddr_in remoteServAddr, int interval);
void ProcessDiskInfo(wstring apiKey, wstring envKey, SOCKET socketHandle, sockaddr_in remoteServAddr, int interval);

void DoRegistration(wstring apiKey, wstring envKey, string remoteServAddr, int port, vector<int> intervalTimes);
void DoUnregistration();
void DoStartService();
BOOL DoStopService();
void ProcessLatestUpdate();
wstring GetAppVersion(const wchar_t *pModuleFileName, WORD *MajorVersion, WORD *MinorVersion, WORD *BuildNumber, WORD *RevisionNumber);
wstring GetNewVersion(wstring txt);
wstring GetUpdateLocation(wstring txt, bool is32bit);
BOOL IsNewVersion(WORD majorVersion, WORD minorVersion, WORD buildNumber, WORD revisionNumber, wstring newUpdateVersion);

VOID ServiceMain(DWORD argc, LPTSTR *argv);
VOID ServiceCtrlHandler(DWORD controlCode);
BOOL UpdateSCMStatus(DWORD dwCurrentState,
    DWORD dwWin32ExitCode,
    DWORD dwServiceSpecificExitCode,
    DWORD dwCheckPoint,
    DWORD dwWaitHint);
void KillService();

DWORD ServiceExecutionThread(LPDWORD param);
BOOL StartServiceThread();

extern wstring g_modulePath;
