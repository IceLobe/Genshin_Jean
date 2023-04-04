// Genshin_Jean.cpp : 定义应用程序的入口点。
//

#include "framework.h"
#include "Genshin_Jean.h"
#include "MainWnd.h"

#define MAX_LOADSTRING 100

// 全局变量:
WCHAR szTitle[MAX_LOADSTRING];                  // 标题栏文本
WCHAR szWindowClass[MAX_LOADSTRING];            // 主窗口类名

void InitResource(HINSTANCE hInstance) {
	// 初始化UI管理器
	CPaintManagerUI::SetInstance(hInstance);
	//CPaintManagerUI::GetResourceType(UILIB_ZIPRESOURCE);
	//HRSRC hResource = ::FindResource(CPaintManagerUI::GetResourceDll(), MAKEINTRESOURCE(IDR_ZIPRES), _T("ZIPRES"));
	//if (hResource != NULL) {
	//	DWORD dwSize = 0;
	//	HGLOBAL hGlobal = ::LoadResource(CPaintManagerUI::GetResourceDll(), hResource);
	//	if (hGlobal != NULL) {
	//		dwSize = ::SizeofResource(CPaintManagerUI::GetResourceDll(), hResource);
	//		if (dwSize > 0) {
	//			CPaintManagerUI::SetResourceZip((LPBYTE)::LockResource(hGlobal), dwSize);
	//		}
	//	}
	//	::FreeResource(hResource);
	//}
}

BOOL IsMutex(TCHAR* fileName) {
	// 给进程实例加锁
	CreateMutex(NULL, FALSE, fileName);
	if (ERROR_ALREADY_EXISTS == GetLastError()) {
		HWND hWnd = FindWindow(szWindowClass, NULL);
		SetForegroundWindow(hWnd);
		return TRUE;
	}
	return FALSE;
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow) {
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// TODO: 在此处放置代码。

	// 初始化全局字符串
	_tcscpy_s(szTitle, MAX_LOADSTRING, _T("Genshin_Jean"));
	_tcscpy_s(szWindowClass, MAX_LOADSTRING, _T("GENSHIN_JEAN"));

	if (IsMutex(szWindowClass))
		return 0;

	HRESULT Hr = ::CoInitialize(NULL);
	if (FAILED(Hr))
		return 0;
	// OLE
	HRESULT hRes = ::OleInitialize(NULL);
	//std::locale::global(std::locale(""));
	// 初始化资源
	//InitResource(hInstance);
	HWND hWnd = ::FindWindow(TEXT("UnityWndClass"), TEXT("原神"));
	if (hWnd == 0) {
		MessageBoxA(0, "请先运行游戏", "Tips", MB_OK);
		return 0;
	}
	// 创建无窗口系统菜单、无最大化的窗体
	MainWnd* pMainWnd = new MainWnd(szWindowClass);
	pMainWnd->Create(::GetDesktopWindow(), szTitle, UI_WNDSTYLE_FRAME, WS_EX_TOPMOST);
	pMainWnd->CenterWindow();
	pMainWnd->ShowWindow();
	CPaintManagerUI::MessageLoop();
	// 清理资源
	CPaintManagerUI::Term();
	// OLE
	::OleUninitialize();
	// COM
	::CoUninitialize();

	return 0;
}

