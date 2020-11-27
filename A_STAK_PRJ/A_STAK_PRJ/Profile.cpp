#include <Windows.h>
#include <stdio.h>
#include <wchar.h>
#include "Profile.h"

PROFILE_SAMPLE profile[MAX_PROFILE];

int IsCheckName(const WCHAR* szName)
{
    for (int iCnt = 0; iCnt < MAX_PROFILE; iCnt++)
    {
        if (profile[iCnt].lFlag == 1)
        {
            if (wcscmp(profile[iCnt].szName, szName) == 0)
            {
                return iCnt;
            }
        }
    }
    return -1;
}


void AddProfile(int index, const WCHAR* szName)
{
    if (*szName == L'\0')
        return;
    profile[index].lFlag = 1;
    wcscpy_s(profile[index].szName, szName);

    QueryPerformanceCounter(&profile[index].lStartTime);

    profile[index].iTotalTime = 0;
    profile[index].iCall = 1;
    memset(profile[index].iMin, 0, sizeof(profile[index].iMin) / sizeof(_int64));
    memset(profile[index].iMax, 0, sizeof(profile[index].iMax) / sizeof(_int64));

    return;
}

void StartProfile(int index)
{
    QueryPerformanceCounter(&profile[index].lStartTime);
    profile[index].iCall += 1;
}

void ProfileBegin(const WCHAR* szName)
{
    int index = -1;
    index = IsCheckName(szName);

    if (index == -1) // 프로파일 이름이 동일한 것이 없다
    {
        for (int iCnt = 0; iCnt < MAX_PROFILE; iCnt++)
        {
            if (profile[iCnt].lFlag == 0)
            {
                AddProfile(iCnt, szName);
                return;
            }
        }
    }
    else
    {
        StartProfile(index);
    }
}

void CheckMinMax(int index, _int64 time)
{
    _int64 tmp = 0;
    if (profile[index].iMin[0] == 0)
    {
        profile[index].iMin[0] = time;
        return;
    }
    for (int iCnt = 0; iCnt < 2; iCnt++)
    {
        if (profile[index].iMin[iCnt] > time)
        {
            tmp = time;
            time = profile[index].iMin[iCnt];
            profile[index].iMin[iCnt] = tmp;
        }
    }

    for (int iCnt = 0; iCnt < 2; iCnt++)
    {
        if (profile[index].iMax[iCnt] < time)
        {
            tmp = time;
            time = profile[index].iMax[iCnt];
            profile[index].iMax[iCnt] = tmp;
        }
    }
}

void ProfileEnd(const WCHAR* szName)
{
    LARGE_INTEGER End;
    LARGE_INTEGER Freq;
    LONGLONG timeDiff;
    double microFreq = 0.0;
    _int64 usingTime;
    int index;

    if (!QueryPerformanceFrequency(&Freq))
        return;

    QueryPerformanceCounter(&End);

    index = IsCheckName(szName); // 체크하는 이름의 인덱스 가져오기

    timeDiff = End.QuadPart - profile[index].lStartTime.QuadPart;//함수 사용시간

    usingTime = (timeDiff) / (Freq.QuadPart / 1000000.0); //함수 사용시간, 마이크로 세컨드 100만단위로 구함

    profile[index].iTotalTime += usingTime; //총 사용시간 집계를 위해 더하기

    CheckMinMax(index, usingTime); //Min과 Max
}

void ProfileDataOutText(const WCHAR* szFileName)
{
    FILE* fp;
    //const char* columns = "Name    |     Average    |       Min        |        Max        |      Call    |\n";
    //char outBuffer[1024] = { 0 };
    //
    //fopen_s(&fp, szFileName, "wb");

    //strncat_s(outBuffer, columns, strlen(columns));

    //for (int iCnt = 0; iCnt < MAX_PROFILE; iCnt++)
    //{
    //    if (profile[iCnt].lFlag == 1)
    //    {
    //        char tmpbuf[192];
    //        sprintf_s(tmpbuf, "%10s |%10.4lf  ㎲ | %10.4lf  ㎲ |  %10.4lf ㎲  |  %10lld |\n", profile[iCnt].szName, (profile[iCnt].iTotalTime / profile[iCnt].iCall) / 1.0,
    //            profile[iCnt].iMin[0] / 1.0, profile[iCnt].iMax[0] / 1.0, profile[iCnt].iCall);
    //        strncat_s(outBuffer, tmpbuf, strlen(tmpbuf));
    //    }
    //}
    //fwrite(outBuffer, 1, strlen(outBuffer), fp);.
    LPCTSTR columns = L"Name  |     Average      |      Min      |      Ma x      |     Call   |\n";
    WCHAR outbuffer[1024];
    char te[1024];
    _wfopen_s(&fp, szFileName, L"w, ccs = UTF-8");
    wmemset(outbuffer, 0, 1024);

    wcsncat_s(outbuffer, columns, wcslen(columns));

    for (int iCnt = 0; iCnt < MAX_PROFILE; iCnt++)
    {
        if (profile[iCnt].lFlag == 1)
        {
            WCHAR tmpbuf[250];

            swprintf_s(tmpbuf, L"%s    |     %.4lf ㎲ | %.4lf ㎲ |  %.4lf ㎲ |  %lld\n", profile[iCnt].szName, (profile[iCnt].iTotalTime / profile[iCnt].iCall) / 1.0,
                profile[iCnt].iMin[0] / 1.0, profile[iCnt].iMax[0] / 1.0, profile[iCnt].iCall);
            wcsncat_s(outbuffer, tmpbuf, wcslen(tmpbuf));
        }
    }
    fwrite(outbuffer, 1, wcslen(outbuffer) * 2, fp);

    fclose(fp);
}