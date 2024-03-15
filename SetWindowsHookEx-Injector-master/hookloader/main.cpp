#include <Windows.h>
#include <iostream>

using namespace std;
BOOL IsChineseLocale() {
	LCID lcid = GetUserDefaultLCID();
	switch (lcid) {
	case 0x0804: // 简体中文（中国大陆）
	case 0x0404: // 繁体中文（台湾）
	case 0x0C04: // 繁体中文（香港）
	case 0x1404: // 繁体中文（澳门）
		return TRUE;
	default:
		return FALSE;
	}
}

int wmain(int argc, wchar_t* argv[], wchar_t* envp[]) {
	if (!IsChineseLocale()) {
		wcout << L"注入失败，请联系作者" << endl;
		system("pause");
		return EXIT_FAILURE;
	}
	wchar_t* dllPath = argv[1];

	if (GetFileAttributes(dllPath) == INVALID_FILE_ATTRIBUTES) {
		cout << "没放高端辅助" << endl;
		system("pause");
		return EXIT_FAILURE;
	}

	HWND hwnd = FindWindowW(L"stingray_window", NULL); //Game window classname
	if (hwnd == NULL) {
		cout << "先开游戏" << endl;
		system("pause");
		return EXIT_FAILURE;
	}

	// Getting the thread of the window and the PID
	DWORD pid = NULL;
	DWORD tid = GetWindowThreadProcessId(hwnd, &pid);
	if (tid == NULL) {
		cout << "不准开杀软" << endl;
		system("pause");
		return EXIT_FAILURE;
	}

	// Loading DLL
	HMODULE dll = LoadLibraryEx(dllPath, NULL, DONT_RESOLVE_DLL_REFERENCES); //Loading dll from params
	if (dll == NULL) {
		cout << "高端辅助注入失败" << endl;
		system("pause");
		return EXIT_FAILURE;
	}

	// Getting exported function address
	HOOKPROC addr = (HOOKPROC)GetProcAddress(dll, "NextHook"); //export see dllmain.cpp "C" __declspec(dllexport) int NextHook(int code, WPARAM wParam, LPARAM lParam)
	if (addr == NULL) {
		cout << "高端辅助注入失败2" << endl;
		system("pause");
		return EXIT_FAILURE;
	}

	// Setting the hook in the hook chain
	HHOOK handle = SetWindowsHookEx(WH_GETMESSAGE, addr, dll, tid); // Or WH_KEYBOARD if you prefer to trigger the hook manually
	if (handle == NULL) {
		cout << "高端辅助注入失败3" << endl;
		system("pause");
		return EXIT_FAILURE;
	}

	// Triggering the hook
	PostThreadMessage(tid, WM_NULL, NULL, NULL);

	// Waiting for user input to remove the hook
	cout << "终于可以玩原神了" << endl;
	cout << "任意键卸载原神" << endl;
	system("pause > nul");

	// Unhooking
	BOOL unhook = UnhookWindowsHookEx(handle);
	if (unhook == FALSE) {
		cout << "原神卸载失败" << endl;
		system("pause");
		return EXIT_FAILURE;
	}

	cout << "原神卸载成功" << endl;
	system("pause > nul");
	return EXIT_SUCCESS;
}
