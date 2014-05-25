#include "StdAfx.h"
#include "StringBuilder.h"

#include <Windows.h>
#include <Winuser.h>

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
    wsprintf(buf, L"%ld", value);

    m_vect.push_back(buf);
}

void StringBuilder::push_back( wstring strText )
{
    m_vect.push_back( strText );
}

void StringBuilder::clear()
{
    m_vect.clear();
}

wstring StringBuilder::ToString()
{
    size_t nTotalSize = 0;
    for ( unsigned i = 0; i < m_vect.size(); i ++ ) {
        nTotalSize += m_vect.at( i ).size();
    }

    wchar_t *tempBuf = new wchar_t[ nTotalSize + 1 ];
    memset(tempBuf, 0, (nTotalSize + 1) * sizeof(wchar_t));

    size_t nPos = 0;
    for ( size_t i = 0; i < m_vect.size(); i ++ ) {
        memcpy( tempBuf + nPos, m_vect.at( i ).c_str(), ( m_vect.at( i ).length() * sizeof( wchar_t ) ) );
        nPos += m_vect.at( i ).length();
    }

    wstring strResult = tempBuf;
    delete [] tempBuf;

    return strResult;
}
