#include "windows.h"
#include "PscHero12.h"

#define WinMainA main

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
LPCWSTR g_wszWcName = L"MyWindow";
bool g_bActive = true;
bool g_bReact = false;

_Use_decl_annotations_
int WINAPI WinMainA(HINSTANCE hThis, HINSTANCE hPrev, LPSTR pszArgs, int iWinMode)
{
	HWND hWnd;
	MSG Msg;
	WNDCLASS Wnd;
	PscHero12* pHero = nullptr;

	Wnd.hInstance = hThis;
	Wnd.lpszClassName = g_wszWcName;
	Wnd.lpfnWndProc = WndProc;
	Wnd.style = 0;
	Wnd.hIcon = LoadIcon(NULL, MAKEINTRESOURCE(IDI_APPLICATION));
	Wnd.hCursor = LoadCursor(NULL, IDC_ARROW);
	Wnd.lpszMenuName = NULL;
	Wnd.cbClsExtra = 0;
	Wnd.cbWndExtra = 0;
	Wnd.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);

	if (!RegisterClass(&Wnd))
		return 0;

	DWORD dwWindowStyle = WS_POPUP | WS_BORDER | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_VISIBLE;
	DWORD dwWidth = 1280;
	DWORD dwHeight = 720;
	RECT rc;
	SetRect(&rc, 0, 0, dwWidth, dwHeight);
	AdjustWindowRect(&rc, dwWindowStyle, FALSE);

	hWnd = CreateWindow(g_wszWcName,
		L"New Hero Class Direct3D12 Test",
		dwWindowStyle,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		rc.right - rc.left,
		rc.bottom - rc.top,
		NULL,
		NULL,
		hThis,
		NULL);

	BOOL bGotMsg;
	pHero = new PscHero12();
	if (SUCCEEDED(pHero->Init(hWnd, hThis)))
	{
		ShowWindow(hWnd, iWinMode);
		if (SUCCEEDED(pHero->Load()))
		{
			do {
				if (g_bActive)
					bGotMsg = PeekMessage(&Msg, NULL, 0U, 0U, PM_REMOVE);
				else
					bGotMsg = GetMessage(&Msg, NULL, 0U, 0U);
				if (bGotMsg)
				{
					TranslateMessage(&Msg);
					DispatchMessage(&Msg);
				}
				else if (g_bActive)
				{
					if (g_bReact)
					{
						pHero->KbAcquire();
						g_bReact = false;
					}

					pHero->Update();
					if (FAILED(pHero->Render()))
					{
						delete pHero;
						MessageBox(hWnd, L"Render´íÎó", L"±­¾ß°¡£¡", MB_OK);
						return 0;
					}
				}
			} while (Msg.message != WM_QUIT);
		}
		else
		{
			delete pHero;
			MessageBox(hWnd, L"Load´íÎó", L"±­¾ß°¡£¡", MB_OK);
			return 0;
		}
	}
	else
	{
		delete pHero;
		MessageBox(hWnd, L"Init´íÎó", L"±­¾ß°¡£¡", MB_OK);
		return 0;
	}

	if (FAILED(pHero->Clear()))
	{
		delete pHero;
		MessageBox(hWnd, L"Clear´íÎó", L"±­¾ß°¡£¡", MB_OK);
		return 0;
	}

	if (FAILED(pHero->Destroy()))
	{
		delete pHero;
		MessageBox(hWnd, L"Destroy´íÎó", L"±­¾ß°¡£¡", MB_OK);
		return 0;
	}

	delete pHero;
	return Msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMessage, WPARAM wParam, LPARAM lParam)
{
	switch (uMessage)
	{
	case WM_DESTROY:
		PostQuitMessage(WM_QUIT);
		break;
	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_ESCAPE:
			PostQuitMessage(WM_QUIT);
			break;
		case VK_F1:
			g_bActive = false;
			MessageBox(hWnd, L"help", L"help", MB_OK);
			g_bActive = true;
		}
		break;
	case WM_ACTIVATE:
		g_bReact = true;
		break;
	}
	return DefWindowProc(hWnd, uMessage, wParam, lParam);
}