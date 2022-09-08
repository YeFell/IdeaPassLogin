
#include "pch.h"
#include "../min-hook/MinHook.h"
#include "windows.h"
#include "string"
#include "thread"


PVOID oldCreateWindowsExW;

int num;
HWND MyCreateWindowExW(
	DWORD     dwExStyle,
	LPCWSTR   lpClassName,
	LPCWSTR   lpWindowName,
	DWORD     dwStyle,
	int       X,
	int       Y,
	int       nWidth,
	int       nHeight,
	HWND      hWndParent,
	HMENU     hMenu,
	HINSTANCE hInstance,
	LPVOID    lpParam
) {
	std::wstring className(lpClassName);
	std::wstring passClassName(L"SunAwtDialog");
	if (className == passClassName)
	{
		MH_DisableHook(MH_ALL_HOOKS);
		return (HWND)1;
	}
	return reinterpret_cast<HWND(*)(DWORD, LPCWSTR, LPCWSTR, DWORD, int, int, int, int, HWND, HMENU, HINSTANCE, LPVOID)>
		(oldCreateWindowsExW)
		(dwExStyle, lpClassName, lpWindowName, dwStyle, X, Y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam);
}


BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	{
		std::thread thread([]()->void {
			auto time = GetTickCount64();
			do
			{
				if ((GetTickCount64() - time) > 120000)
					break;
				Sleep(100);

			} while (!FindWindow(L"SunAwtToolkit", NULL));

			if (MH_OK == MH_Initialize())
			{
				MH_CreateHookApi(L"User32.dll", "CreateWindowExW", MyCreateWindowExW, &oldCreateWindowsExW);
				MH_EnableHook(MH_ALL_HOOKS);
			}});
		thread.detach();
		break;
	}
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
		break;
	case DLL_PROCESS_DETACH:
		MH_DisableHook(MH_ALL_HOOKS);
		MH_Uninitialize();
		break;
	}
	return TRUE;
}

