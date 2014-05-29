#pragma once

#include "stdafx.h"

wstring GetApiKey(int argc, _TCHAR* argv[]);
wstring GetUID(int argc, _TCHAR* argv[]);
wstring GetEnvInfo(int argc, _TCHAR* argv[]);
string GetHostAddress(int argc, _TCHAR* argv[], ConnectionInfo connection);

void OutputError(wstring txt, ...);

wstring GetComputerName();
int GetIntervalTime(int argc, _TCHAR* argv[]);
void ProcessInfo(wstring apiKey, wstring envKey, SOCKET socketHandle, sockaddr_in remoteServAddr);
void ShowHelp();
void SendToServer(SOCKET socketHandle, sockaddr_in remoteServAddr, StringBuilder &sb);
