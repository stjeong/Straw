// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

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
};

#define SERVICE_NAME L"StrawAgent"
#define REG_SERVICE L"SYSTEM\\CurrentControlSet\\Services\\" ##SERVICE_NAME
#define UPDATE_CHECK_URL L"https://raw.githubusercontent.com/stjeong/Straw/master/agent/windows/version.xml"

