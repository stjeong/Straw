#pragma once
#include <string.h>
#include <malloc.h>
#include <tchar.h>
#include <vector>

class StringBuilder
{
public:
    StringBuilder() {}
    ~StringBuilder() {}

    void push_back( wchar_t ch );
    void push_back(__int64 value);
    void push_back( wstring strText );

    wstring ToString();
    void clear();

protected:
    vector<wstring> m_vect;
};

