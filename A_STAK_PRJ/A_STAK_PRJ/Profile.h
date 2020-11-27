#pragma once

typedef struct
{
    long lFlag;  //���������� ��� ����. (�迭 �ÿ���)
    WCHAR szName[64]; //�������� ���� �̸�

    LARGE_INTEGER lStartTime; // �������� ���� ���� �ð�

    _int64 iTotalTime;  // ��ü ��� �ð� ī����
    _int64 iMin[2];  //�ּ� ��� �ð� ī����
    _int64 iMax[2];  //�ִ� ��� �ð� ī����

    _int64 iCall;  //���� ȣ��
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