#pragma once

#include <malloc.h>
#include <tchar.h>
#include <vector>
#include <string>

class StringSplit
{
public:
    void SplitString(wstring data, wstring delimeter = L"\r\n");
    void Empty();

    wstring GetValue(size_t index);

    size_t GetCount() { return m_split.size(); }

    StringSplit() {}
    virtual ~StringSplit() {}

protected:
    vector<wstring> m_split;
};
