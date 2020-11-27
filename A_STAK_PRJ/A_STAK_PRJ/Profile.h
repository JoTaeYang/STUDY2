#pragma once

typedef struct
{
    long lFlag;  //프로파일의 사용 여부. (배열 시에만)
    WCHAR szName[64]; //프로파일 샘플 이름

    LARGE_INTEGER lStartTime; // 프로파일 샘플 실행 시간

    _int64 iTotalTime;  // 전체 사용 시간 카운터
    _int64 iMin[2];  //최소 사용 시간 카운터
    _int64 iMax[2];  //최대 사용 시간 카운터

    _int64 iCall;  //누적 호출
}PROFILE_SAMPLE;

#define MAX_PROFILE 100

int IsCheckName(const WCHAR* szName);
void AddProfile(int index, const WCHAR* szName);
void StartProfile(int index);
void ProfileBegin(const WCHAR* szName);
void CheckMinMax(int index, _int64 time);
void ProfileEnd(const WCHAR* szName);
void ProfileDataOutText(const WCHAR* szFileName);

#define PRO_BEGIN(TagName) ProfileBegin(TagName)
#define PRO_END(TagName) ProfileEnd(TagName)