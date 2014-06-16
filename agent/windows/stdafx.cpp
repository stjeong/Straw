// stdafx.cpp : source file that includes just the standard includes
// InfoCollector.pch will be the pre-compiled header
// stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"

#include <stdarg.h>
#include <strsafe.h>

#include "..\..\Common\DataPacket.generated.cpp"

BOOL g_debugMode = FALSE;
BOOL g_isConsoleApp = FALSE;

void OutputError(wchar_t *fmt, ...)
{
    va_list args;
    va_start(args, fmt);

    if (g_isConsoleApp == TRUE)
    {
        wprintf(L"Error: ");
        wprintf(fmt, args);
        wprintf(L"\n");
    }
    else
    {
        if (g_debugMode == TRUE)
        {
            wchar_t buf[4096] = { 0 };
            StringCchVPrintf(buf, 4096, fmt, args);
            ::OutputDebugString(buf);
        }
    }

    va_end(args);
}

void OutputConsole(wchar_t *fmt, ...)
{
    va_list args;
    va_start(args, fmt);

    if (g_isConsoleApp == TRUE)
    {
        vwprintf(fmt, args);
    }
    else
    {
        if (g_debugMode == TRUE)
        {
            wchar_t buf[4096] = { 0 };
            StringCchVPrintf(buf, 4096, fmt, args);
            ::OutputDebugString(buf);
        }
    }
    va_end(args);
}

