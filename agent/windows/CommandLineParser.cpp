
#include "stdafx.h"
#include "CommandLineParser.h"

wchar_t* getCmdOption(wchar_t ** begin, wchar_t ** end, const std::wstring & option)
{
    wchar_t ** itr = std::find(begin, end, option);
    if (itr != end && ++itr != end)
    {
        return *itr;
    }
    return 0;
}

bool cmdOptionExists(wchar_t** begin, wchar_t** end, const std::wstring& option)
{
    return std::find(begin, end, option) != end;
}
