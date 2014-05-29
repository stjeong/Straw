#pragma once

// http://stackoverflow.com/questions/865668/parse-command-line-arguments

#include "stdafx.h"

#include <algorithm>

wchar_t* getCmdOption(wchar_t ** begin, wchar_t ** end, const std::wstring & option);
bool cmdOptionExists(wchar_t** begin, wchar_t** end, const std::wstring& option);
