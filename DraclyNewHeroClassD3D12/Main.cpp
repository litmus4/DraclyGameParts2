#include "windows.h"

//LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
LPCSTR g_szWcName = "MyWindow";
bool g_bActive = true;
bool g_bReact = false;

_Use_decl_annotations_
int WINAPI WinMain(HINSTANCE hThis, HINSTANCE hPrev, LPSTR pszArgs, int iWinMode)
{
	HWND hWnd;
	MSG Msg;
	WNDCLASS WndClass;
	//

	//*FLAGJK
	Msg.wParam = 0;
	//*/

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
			g_bActive = FALSE;
			MessageBox(hWnd, L"help", L"help", MB_OK);
			g_bActive = TRUE;
		}
		break;
	case WM_ACTIVATE:
		g_bReact = TRUE;
		break;
	}
	return DefWindowProc(hWnd, uMessage, wParam, lParam);
}