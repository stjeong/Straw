#include "stdafx.h"
#include "StringSplit.h"

// data 문자열을 delimeter 문자열로 조각내어 내부 Split 배열에 저장합니다.
void StringSplit::SplitString(wstring data, wstring delimeter)
{
    m_split.clear();

    size_t sindex = 0, eindex = 0;
    wstring temp;
    bool bLoop = true;

    if (data.length()) {
        while (bLoop) {

            // 대소문자를 구별할 것인지에 따라서
            eindex = data.find(delimeter, sindex);

            if (eindex == -1) {
                eindex = data.length();
                bLoop = false;
            }

            temp = data.substr(sindex, eindex - sindex);

            m_split.push_back(temp);
            sindex = eindex + delimeter.length();
        }
    }
}

// Split 메모리를 비웁니다.
void StringSplit::Empty()
{
    m_split.clear();
}

wstring StringSplit::GetValue(size_t index)
{
    wstring retVal = L"";
    if (index >= m_split.size())
    {
        throw "IndexOutOfRangeException";
    }
    else {
        retVal = m_split.at(index);
    }

    return retVal;
}
