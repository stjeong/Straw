﻿#include "StdAfx.h"
#include "StringBuilder.h"

#include <Windows.h>
#include <Winuser.h>

#include <locale>
#include <strsafe.h>

string ws2s(const std::wstring& wstr)
{
    size_t converted = 0;
    char *pBuf = new char[wstr.length() + 1];
    if (pBuf == nullptr)
    {
        return "";
    }

    wcstombs_s(&converted, pBuf, wstr.length() + 1, wstr.c_str(), _TRUNCATE);

    string txt = pBuf;

    delete [] pBuf;

    return txt;
}

wstring s2ws(const std::string& str)
{
    int len;
    int slength = (int)str.length() + 1;

    len = MultiByteToWideChar(CP_ACP, 0, str.c_str(), slength, 0, 0);
    wchar_t* buf = new wchar_t[len];

    MultiByteToWideChar(CP_ACP, 0, str.c_str(), slength, buf, len);
    std::wstring r(buf);
    delete[] buf;

    return r;
}

void StringBuilder::push_back( wchar_t ch )
{
    wstring text;
    
    text += ch;
    m_vect.push_back( text );
}

void StringBuilder::push_back(__int64 value)
{
    // 9223372036854775807

    // 1048576 MB
    // 1073741824 GB
    // 1099511627776 TB (tera)
    // 1125899906842624 PB (peta)
    wchar_t buf[25] = { 0 };
    StringCchPrintf(buf, 25, L"%I64d", value);

    m_vect.push_back(buf);
}

void StringBuilder::push_back( wstring strText )
{
    m_vect.push_back( strText );
}

void StringBuilder::push_back(string strText)
{
    wstring txt = s2ws(strText);
    m_vect.push_back(txt);
}

void StringBuilder::clear()
{
    m_vect.clear();
}

wstring StringBuilder::ToString()
{
    size_t totalSize = 0;
    for ( unsigned i = 0; i < m_vect.size(); i ++ ) {
        totalSize += m_vect.at(i).size();
    }

    wchar_t *tempBuf = new wchar_t[totalSize + 1];
    memset(tempBuf, 0, (totalSize + 1) * sizeof(wchar_t));

    size_t nPos = 0;
    for ( size_t i = 0; i < m_vect.size(); i ++ ) {
        memcpy( tempBuf + nPos, m_vect.at( i ).c_str(), ( m_vect.at( i ).length() * sizeof( wchar_t ) ) );
        nPos += m_vect.at( i ).length();
    }

    wstring strResult = tempBuf;
    delete [] tempBuf;

    return strResult;
}

wchar_t *StringBuilder::ToStringMultiLine(int *allocatedSize)
{
    int totalSize = 0;
    for (unsigned i = 0; i < m_vect.size(); i++) {
        totalSize += (int)m_vect.at(i).size() + 1; // +1 == '\0'
    }

    *allocatedSize = (totalSize + 1) * sizeof(wchar_t);

    wchar_t *tempBuf = new wchar_t[totalSize + 1]; // +1 == '\0'
    memset(tempBuf, 0, (totalSize + 1) * sizeof(wchar_t));

    size_t nPos = 0;
    for (size_t i = 0; i < m_vect.size(); i++) {
        memcpy(tempBuf + nPos, m_vect.at(i).c_str(), (m_vect.at(i).length() * sizeof(wchar_t)));
        nPos += m_vect.at(i).length();

        nPos ++; // '\0';
    }

    return tempBuf;
}