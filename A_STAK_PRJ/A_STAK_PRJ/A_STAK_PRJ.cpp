// A_STAK_PRJ.cpp : 애플리케이션에 대한 진입점을 정의합니다.
//

#include "framework.h"
#include "A_STAK_PRJ.h"
#include <stdio.h>
#include "CList.h"
#include "Profile.h"

#define GRID_WIDTH 61
#define GRID_HEIGHT 36
#define GRID_SIZE 20
#define ADD 1.5

HINSTANCE g_Inst;
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);


stNode* g_StartPoint;
stNode* g_EndPoint;

//타일 선택은 어떻게 할지..?
char t_grid[GRID_WIDTH][GRID_HEIGHT] = {0, };

HBRUSH g_OldBrush;
HBRUSH g_ObsBrush; //장애물 Brush
HBRUSH g_EndBrush; //EndPoint Brush;
HBRUSH g_StartBrush; // StartPoint Brush;
HBRUSH g_OpenBrush;
HBRUSH g_CloseBrush;

CList g_OpenList;
CList g_CloseList;

BOOL g_Draw = false;
BOOL g_StPoint = false; //시작점 설정 키 누르면 TRUE;

void PaintTile(HDC hdc);
void StartPointPaint(HDC hdc);
void EndPointPaint(HDC hdc);
void ObstaclePaint(HDC hdc);
void OpenListPaint(HDC hdc);
void CloseListPaint(HDC hdc);
void PathPaint(HDC hdc);
void FindWay(HDC hdc);
void Create8WayNode(int X, int Y, stNode* parent);
void FileOut();

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);
    // TODO: 여기에 코드를 입력합니다.

    g_Inst = hInstance;

    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ASTAKPRJ));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = MAKEINTRESOURCEW(IDI_ASTAKPRJ);
    wcex.lpszClassName = L"BasicWindowClass";
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    RegisterClassExW(&wcex);

    g_ObsBrush = (HBRUSH)GetStockObject(LTGRAY_BRUSH);
    g_EndBrush = CreateSolidBrush(RGB(255, 0, 0));
    g_StartBrush = CreateSolidBrush(RGB(0, 255, 0));
    g_OpenBrush = CreateSolidBrush(RGB(0, 0, 255));
    g_CloseBrush = CreateSolidBrush(RGB(255, 255, 0));

    g_EndPoint = new stNode;
    g_StartPoint = new stNode;

    HWND hWnd = CreateWindowW(L"BasicWindowClass", L"BASE", WS_OVERLAPPED |
        WS_CAPTION |
        WS_SYSMENU |
        WS_THICKFRAME |
        WS_MINIMIZEBOX |
        WS_MAXIMIZEBOX,
        CW_USEDEFAULT, 0, 1500, 1500, nullptr, nullptr, hInstance, nullptr);
    if (!hWnd)
    {
        return FALSE;
    }

    ShowWindow(hWnd, nCmdShow);

    UpdateWindow(hWnd);

    MSG msg;
    // 기본 메시지 루프입니다:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    delete g_EndPoint;
    delete g_StartPoint;
    ProfileDataOutText(L"Test.txt");
    
    return (int)msg.wParam;
}

//
//  함수: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  용도: 주 창의 메시지를 처리합니다.
//
//  WM_COMMAND  - 애플리케이션 메뉴를 처리합니다.
//  WM_PAINT    - 주 창을 그립니다.
//  WM_DESTROY  - 종료 메시지를 게시하고 반환합니다.
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{

    switch (message)
    {
    case WM_CREATE:
    {

    }
    break;
    case WM_LBUTTONDOWN:
        g_Draw = TRUE;
        InvalidateRect(hWnd, NULL, TRUE);
        break;
    case WM_MOUSEMOVE :
    if(g_Draw)
    {        
        int x = LOWORD(lParam) / GRID_SIZE;
        int y = HIWORD(lParam) / GRID_SIZE;

        if(x < GRID_WIDTH  && y < GRID_HEIGHT  )
            t_grid[x][y] = 1;//1이 장애물.
        InvalidateRect(hWnd, NULL, FALSE);
    }
    break;
    case WM_LBUTTONUP:
        g_Draw = FALSE;
        break;

    case WM_RBUTTONDOWN:
    {
        int y = HIWORD(lParam) / GRID_SIZE; //타일 크기가 20 * 20 이라 20을 나눔
        int x = LOWORD(lParam) / GRID_SIZE;
        
        if (x < GRID_WIDTH  && y < GRID_HEIGHT &&t_grid[x][y] != 1)
        {
            if (g_StPoint)
            {
                g_StartPoint->_iX = x;
                g_StartPoint->_iY = y;
                g_StPoint = false;
            }
            else
            {
                g_EndPoint->_iX = x;
                g_EndPoint->_iY = y;
            }
        }
        InvalidateRect(hWnd, NULL, TRUE);
    }
        break;
    case WM_KEYDOWN:
    {
        switch (wParam)
        {
        case 'A':
            g_StPoint = TRUE;
            break;
        case VK_SPACE: //장애물 초기화
            ZeroMemory(t_grid, GRID_WIDTH * GRID_HEIGHT);
            InvalidateRect(hWnd, NULL, TRUE);
            break;
        case VK_RETURN:        
        {                   
            HDC hdc = GetDC(hWnd);
            g_OpenList.Clear();            
            g_CloseList.Clear();
            FindWay(hdc);
            ReleaseDC(hWnd, hdc);                    
        }
        case 'Y':
            FileOut();
            break;
        case 'Z':
            PostQuitMessage(0);
            break;
        }
    }
    break;

    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        PaintTile(hdc);
        
        ObstaclePaint(hdc);

        StartPointPaint(hdc);
        EndPointPaint(hdc);
        
        TextOut(hdc, 10, (GRID_HEIGHT * GRID_SIZE) + 20, L"Space : 장애물 초기화 A : 시작점 설정 ENTER : 길찾기 시작", 
            wcslen(L"Space : 장애물 초기화 A : 시작점 설정 ENTER : 길찾기 시작"));
        EndPaint(hWnd, &ps);
    }
    break;
    case WM_DESTROY:

        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

void PaintTile(HDC hdc)
{
    for (int i = 0; i <= GRID_WIDTH; i++)
    {
        MoveToEx(hdc, i * GRID_SIZE, 0, NULL);
        LineTo(hdc, i * GRID_SIZE, GRID_HEIGHT * GRID_SIZE);
    }
    for (int i = 0; i <= GRID_HEIGHT ; i++)
    {
        MoveToEx(hdc, 0, i * GRID_SIZE, NULL);
        LineTo(hdc, ((GRID_WIDTH ) * GRID_SIZE), i * GRID_SIZE);
    }
}

void StartPointPaint(HDC hdc)
{
    g_OldBrush = (HBRUSH)SelectObject(hdc, g_StartBrush);
    Rectangle(hdc, g_StartPoint->_iX * GRID_SIZE, g_StartPoint->_iY * GRID_SIZE, g_StartPoint->_iX * GRID_SIZE + 21, g_StartPoint->_iY * GRID_SIZE + 21);
    SelectObject(hdc, g_OldBrush);
}
/*
도착지를 빨간색으로 채우기.
*/
void EndPointPaint(HDC hdc)
{
    g_OldBrush = (HBRUSH)SelectObject(hdc, g_EndBrush);
    Rectangle(hdc, g_EndPoint->_iX * GRID_SIZE, g_EndPoint->_iY * GRID_SIZE, g_EndPoint->_iX * GRID_SIZE + 21, g_EndPoint->_iY * GRID_SIZE + 21); //21을 더해줘야 타일을 정확히 덮었음
    SelectObject(hdc, g_OldBrush);
}

/*
OpenList에 있는 Node들을 파란색으로 Paint
순회를 어떻게 해야할지 감이 안잡힌다..
*/
void OpenListPaint(HDC hdc)
{
    g_OldBrush = (HBRUSH)SelectObject(hdc, g_OpenBrush);
    CList::stListNode* tmp = g_OpenList.GetStartNode();
    CList::stListNode* end = g_OpenList.GetEndNode();
    while (tmp != end)
    {
        Rectangle(hdc, tmp->_data->_iX * GRID_SIZE, tmp->_data->_iY * GRID_SIZE, tmp->_data->_iX * GRID_SIZE + 21, tmp->_data->_iY * GRID_SIZE + 21);
        tmp = tmp->_Next;
    }
    SelectObject(hdc, g_OldBrush);
}

/*
CloseList에 있는 Node들을 노란색으로 Paint
순회를 어떻게 해야할까.. 머리가 안돌아가는구만..
*/
void CloseListPaint(HDC hdc)
{
    g_OldBrush = (HBRUSH)SelectObject(hdc, g_CloseBrush);
    CList::stListNode* tmp = g_CloseList.GetStartNode();
    CList::stListNode* end = g_CloseList.GetEndNode();
    while (tmp != end)
    {
        Rectangle(hdc, tmp->_data->_iX * GRID_SIZE, tmp->_data->_iY * GRID_SIZE, tmp->_data->_iX * GRID_SIZE + 21, tmp->_data->_iY * GRID_SIZE + 21);
        tmp = tmp->_Next;
    }
    SelectObject(hdc, g_OldBrush);
}


/*
타일 값이 1이면 장애물, 이 장애물을 회색으로 Paint
*/
void ObstaclePaint(HDC hdc)
{
    g_OldBrush = (HBRUSH)SelectObject(hdc, g_ObsBrush);
    for (int i = 0; i < GRID_WIDTH; i++)
    {
        for (int j = 0; j < GRID_HEIGHT; j++)
        {
            if (t_grid[i][j] == 1)
            {
                Rectangle(hdc, i * 20, j * 20, i * 20 + 21, j * 20 + 21);
            }
        }
    }
    SelectObject(hdc, g_OldBrush);
}

void FindWay(HDC hdc)
{
    int tmpX = g_StartPoint->_iX - g_EndPoint->_iX;
    int tmpY = g_StartPoint->_iY - g_EndPoint->_iY;
    g_StartPoint->_dValueG = 0;
    g_StartPoint->_iValueH = abs(tmpX) + abs(tmpY);
    g_StartPoint->_dValueF = g_StartPoint->_dValueG + g_StartPoint->_iValueH;

    g_OpenList.Push(g_StartPoint);
    stNode* pTemp;
    
    
    while (1)
    {
        pTemp = g_OpenList.Pop();
        g_CloseList.Push(pTemp);
        if (pTemp == NULL)
            break;
        if (pTemp->_iX == g_EndPoint->_iX &&
            pTemp->_iY == g_EndPoint->_iY)
        {
            g_EndPoint->pParent = pTemp;
            //parent 들어가서 경로 출력은 별개로 한다.
            break;
        }

        Create8WayNode(pTemp->_iX, pTemp->_iY, pTemp);

        OpenListPaint(hdc);
        CloseListPaint(hdc);
        StartPointPaint(hdc);
        EndPointPaint(hdc);
    }
    PathPaint(hdc);
}

void PathPaint(HDC hdc)
{
    stNode* tmp = g_EndPoint;
    HPEN MyPen, OldPen;
    MyPen = CreatePen(PS_SOLID, 5, RGB(255, 0, 0));
    OldPen = (HPEN)SelectObject(hdc, MyPen);
    while (tmp->pParent != NULL)
    {        
        MoveToEx(hdc, (tmp->_iX * 20) + 10, (tmp->_iY * 20) + 10, NULL);
        tmp = tmp->pParent;
        LineTo(hdc, (tmp->_iX * 20) + 10, (tmp->_iY * 20) + 10);
    }
    SelectObject(hdc, OldPen);
    DeleteObject(MyPen);
}

void Create8WayNode(int X, int Y, stNode* parent)
{
    stNode* tmpData = new stNode;
    int cnt = 0;
    //왼쪽 3방향
    PRO_BEGIN(L"Create8Way");
    if (X - 1 >= 0 && Y - 1 >= 0)
    {
        if (t_grid[X - 1][Y - 1] != 1)
        {
            if (!g_CloseList.Search(X - 1, Y - 1, &tmpData))
            {

            }
            else if (!g_OpenList.Search(X - 1, Y - 1, &tmpData))
            {
                if (tmpData->_dValueG > parent->_dValueG + ADD)
                {
                    tmpData->pParent = parent;
                    tmpData->_dValueG = parent->_dValueG + ADD;
                    tmpData->_dValueF = tmpData->_dValueG + tmpData->_iValueH;
                }
            }
            else {
                stNode* newNode = new stNode;
                newNode->_iX = X - 1;
                newNode->_iY = Y - 1;
                newNode->_dValueG = parent->_dValueG + ADD;
                newNode->_iValueH = abs(g_EndPoint->_iX - (X - 1)) + abs(g_EndPoint->_iY - (Y - 1));
                newNode->_dValueF = newNode->_dValueG + newNode->_iValueH;
                newNode->pParent = parent;
                g_OpenList.Push(newNode);
            }
        }
    }

    if (X - 1 >= 0)
    {
        if (t_grid[X - 1][Y] != 1)
        {
            if (!g_CloseList.Search(X - 1, Y , &tmpData))
            {

            }
            else if (!g_OpenList.Search(X - 1, Y, &tmpData))
            {
                if (tmpData->_dValueG > parent->_dValueG + 1)
                {
                    tmpData->pParent = parent;
                    tmpData->_dValueG = parent->_dValueG + 1;
                    tmpData->_dValueF = tmpData->_dValueG + tmpData->_iValueH;
                }
            }
            else {
                stNode* newNode = new stNode;
                newNode->_iX = X - 1;
                newNode->_iY = Y ;
                newNode->_dValueG = parent->_dValueG + 1;
                newNode->_iValueH = abs(g_EndPoint->_iX - (X - 1)) + abs(g_EndPoint->_iY - (Y));
                newNode->_dValueF = newNode->_dValueG + newNode->_iValueH;
                newNode->pParent = parent;
                g_OpenList.Push(newNode);
            }
        }
    }


    if (X - 1 >= 0 && Y + 1 < GRID_HEIGHT)
    {
        if (t_grid[X - 1][Y + 1] != 1)
        {
            if (!g_CloseList.Search(X - 1, Y + 1, &tmpData))
            {

            }
            else if (!g_OpenList.Search(X - 1, Y + 1, &tmpData))
            {
                if (tmpData->_dValueG > parent->_dValueG + ADD)
                {
                    tmpData->pParent = parent;
                    tmpData->_dValueG = parent->_dValueG + ADD;
                    tmpData->_dValueF = tmpData->_dValueG + tmpData->_iValueH;
                }
            }
            else
            {
                stNode* newNode = new stNode;
                newNode->_iX = X - 1;
                newNode->_iY = Y + 1;
                newNode->_dValueG = parent->_dValueG + ADD;
                newNode->_iValueH = abs(g_EndPoint->_iX - (X - 1)) + abs(g_EndPoint->_iY - (Y + 1));
                newNode->_dValueF = newNode->_dValueG + newNode->_iValueH;
                newNode->pParent = parent;
                g_OpenList.Push(newNode);
            }
        }
    }
    
    //////// 가운데 2개
    if (Y - 1 >= 0)
    {
        if (t_grid[X][Y - 1] != 1)
        {
            if (!g_CloseList.Search(X, Y - 1, &tmpData))
            {

            }
            else if (!g_OpenList.Search(X, Y - 1, &tmpData))
            {
                if (tmpData->_dValueG > parent->_dValueG + 1)
                {
                    tmpData->pParent = parent;
                    tmpData->_dValueG = parent->_dValueG + 1;
                    tmpData->_dValueF = tmpData->_dValueG + tmpData->_iValueH;
                }
            }
            else
            {
                stNode* newNode = new stNode;
                newNode->_iX = X;
                newNode->_iY = Y - 1;
                newNode->_dValueG = parent->_dValueG + 1;
                newNode->_iValueH = abs(g_EndPoint->_iX - (X)) + abs(g_EndPoint->_iY - (Y - 1));
                newNode->_dValueF = newNode->_dValueG + newNode->_iValueH;
                newNode->pParent = parent;
                g_OpenList.Push(newNode);
            }
        }
    }

    if (Y + 1 < GRID_HEIGHT)
    {
        if (t_grid[X][Y + 1] != 1)
        {
            if (!g_CloseList.Search(X , Y + 1, &tmpData))
            {

            }
            else if (!g_OpenList.Search(X , Y + 1, &tmpData))
            {
                if (tmpData->_dValueG > parent->_dValueG + 1)
                {
                    tmpData->pParent = parent;
                    tmpData->_dValueG = parent->_dValueG + 1;
                    tmpData->_dValueF = tmpData->_dValueG + tmpData->_iValueH;
                }
            }
            else
            {
                stNode* newNode = new stNode;
                newNode->_iX = X ;
                newNode->_iY = Y + 1;
                newNode->_dValueG = parent->_dValueG + 1;
                newNode->_iValueH = abs(g_EndPoint->_iX - (X)) + abs(g_EndPoint->_iY - (Y + 1));
                newNode->_dValueF = newNode->_dValueG + newNode->_iValueH;
                newNode->pParent = parent;
                g_OpenList.Push(newNode);
            }
        }
    }

    ///오른쪽 3개

    if (X + 1 < GRID_WIDTH && Y - 1 >= 0)
    {
        if (t_grid[X + 1][Y - 1] != 1)
        {
            if (!g_CloseList.Search(X + 1, Y - 1, &tmpData))
            {

            }
            else if (!g_OpenList.Search(X + 1, Y - 1, &tmpData))
            {
                if (tmpData->_dValueG > parent->_dValueG + ADD)
                {
                    tmpData->pParent = parent;
                    tmpData->_dValueG = parent->_dValueG + ADD;
                    tmpData->_dValueF = tmpData->_dValueG + tmpData->_iValueH;
                }
            }
            else
            {
                stNode* newNode = new stNode;
                newNode->_iX = X + 1;
                newNode->_iY = Y - 1;
                newNode->_dValueG = parent->_dValueG + ADD;
                newNode->_iValueH = abs(g_EndPoint->_iX - (X + 1)) + abs(g_EndPoint->_iY - (Y - 1));
                newNode->_dValueF = newNode->_dValueG + newNode->_iValueH;
                newNode->pParent = parent;
                g_OpenList.Push(newNode);
            }            
        }
    }

    if (X + 1 < GRID_WIDTH )
    {
        if (t_grid[X + 1][Y] != 1)
        {
            if (!g_CloseList.Search(X + 1, Y , &tmpData))
            {

            }
            else if (!g_OpenList.Search(X + 1, Y, &tmpData))
            {
                if (tmpData->_dValueG > parent->_dValueG + 1)
                {
                    tmpData->pParent = parent;
                    tmpData->_dValueG = parent->_dValueG + 1;
                    tmpData->_dValueF = tmpData->_dValueG + tmpData->_iValueH;
                }
            }
            else {
                stNode* newNode = new stNode;
                newNode->_iX = X + 1;
                newNode->_iY = Y;
                newNode->_dValueG = parent->_dValueG + 1;
                newNode->_iValueH = abs(g_EndPoint->_iX - (X + 1)) + abs(g_EndPoint->_iY - (Y));
                newNode->_dValueF = newNode->_dValueG + newNode->_iValueH;
                newNode->pParent = parent;
                g_OpenList.Push(newNode);
            }
        }
    }

    if (X + 1 < GRID_WIDTH && Y + 1 < GRID_HEIGHT)
    {
        if (t_grid[X + 1][Y + 1] != 1)
        {
            if (!g_CloseList.Search(X + 1, Y + 1, &tmpData))
            {

            }
            else if (!g_OpenList.Search(X + 1, Y + 1, &tmpData))
            {
                if (tmpData->_dValueG > parent->_dValueG + ADD)
                {
                    tmpData->pParent = parent;
                    tmpData->_dValueG = parent->_dValueG + ADD;
                    tmpData->_dValueF = tmpData->_dValueG + tmpData->_iValueH;
                }
            }
            else
            {
                stNode* newNode = new stNode;
                newNode->_iX = X + 1;
                newNode->_iY = Y + 1;
                newNode->_dValueG = parent->_dValueG + ADD;
                newNode->_iValueH = abs(g_EndPoint->_iX - (X + 1)) + abs(g_EndPoint->_iY - (Y + 1));
                newNode->_dValueF = newNode->_dValueG + newNode->_iValueH;
                newNode->pParent = parent;
                g_OpenList.Push(newNode);
            }
        }
    }
    PRO_END(L"Create8Way");
    PRO_BEGIN(L"SORT");
    g_OpenList.Sort();
    PRO_END(L"SORT");
    tmpData = NULL;
    delete tmpData;
}

void FileOut()
{
    FILE* fp;

    _wfopen_s(&fp, L"Open List.txt", L"wt");

    CList::stListNode* tmp = g_OpenList.GetStartNode();
    CList::stListNode* end = g_OpenList.GetEndNode();
    while (tmp != end)
    {
        fwprintf(fp, L"(%d , %d) : F : %lf G : %lf H : %d\n", tmp->_data->_iX, tmp->_data->_iY, tmp->_data->_dValueF, tmp->_data->_dValueG, tmp->_data->_iValueH);
        tmp = tmp->_Next;
    }

    fclose(fp);

    FILE* fp2;

    _wfopen_s(&fp2, L"Close List.txt", L"wt");

    tmp = g_CloseList.GetStartNode();
    end = g_CloseList.GetEndNode();
    while (tmp != end)
    {
        fwprintf(fp2, L"(%d , %d) : F : %lf G : %lf H : %d\n", tmp->_data->_iX, tmp->_data->_iY, tmp->_data->_dValueF, tmp->_data->_dValueG, tmp->_data->_iValueH);
        tmp = tmp->_Next;
    }

    fclose(fp2);
}