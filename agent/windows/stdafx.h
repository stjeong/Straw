// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#include <Windows.h>
#include <WinBase.h>

#include <stdio.h>
#include <tchar.h>

#include <string>
using namespace std;

#include "..\..\Common\DataPacket.generated.h"

enum IC_RSEULT {
    IC_NOERROR,
    IC_NO_APIKEY,
    IC_NO_AGENTIDINFO,
    IC_ERROR_SOCKETBIND,
    IC_NO_RESOLVE_HOSTADDR,
    IC_EVENT_CREATE_FAIL,
    IC_NO_INITIALIZE,
};

#define SERVICE_NAME L"StrawAgent"
#define REG_SERVICE L"SYSTEM\\CurrentControlSet\\Services\\" ##SERVICE_NAME

#define SERVICE_STOP_CMD L"/C net stop " ##SERVICE_NAME;
#define SERVICE_START_CMD L"/C net start " ##SERVICE_NAME;

#define UPDATE_CHECK_URL L"https://raw.githubusercontent.com/stjeong/Straw/master/agent/windows/version.xml"

void OutputError(wchar_t *fmt, ...);
void OutputConsole(wchar_t *fmt, ...);

extern BOOL g_isConsoleApp;
extern BOOL g_debugMode;