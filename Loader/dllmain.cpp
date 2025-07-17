#include <Windows.h>
#include <Shlwapi.h>
#include <iostream>
#include <filesystem>

#pragma region Proxy
struct XInput1_4_dll {
	HMODULE dll;
	FARPROC oDllMain;
	FARPROC oXInputEnable;
	FARPROC oXInputGetAudioDeviceIds;
	FARPROC oXInputGetBatteryInformation;
	FARPROC oXInputGetCapabilities;
	FARPROC oXInputGetKeystroke;
	FARPROC oXInputGetState;
	FARPROC oXInputSetState;
} XInput1_4;

extern "C" {
	FARPROC PA = 0;
	int runASM();

	void fDllMain() { PA = XInput1_4.oDllMain; runASM(); }
	void fXInputEnable() { PA = XInput1_4.oXInputEnable; runASM(); }
	void fXInputGetAudioDeviceIds() { PA = XInput1_4.oXInputGetAudioDeviceIds; runASM(); }
	void fXInputGetBatteryInformation() { PA = XInput1_4.oXInputGetBatteryInformation; runASM(); }
	void fXInputGetCapabilities() { PA = XInput1_4.oXInputGetCapabilities; runASM(); }
	void fXInputGetKeystroke() { PA = XInput1_4.oXInputGetKeystroke; runASM(); }
	void fXInputGetState() { PA = XInput1_4.oXInputGetState; runASM(); }
	void fXInputSetState() { PA = XInput1_4.oXInputSetState; runASM(); }
}

void setupFunctions() {
	printf("setup functions start\n");
	XInput1_4.oDllMain = GetProcAddress(XInput1_4.dll, "DllMain");
	XInput1_4.oXInputEnable = GetProcAddress(XInput1_4.dll, "XInputEnable");
	XInput1_4.oXInputGetAudioDeviceIds = GetProcAddress(XInput1_4.dll, "XInputGetAudioDeviceIds");
	XInput1_4.oXInputGetBatteryInformation = GetProcAddress(XInput1_4.dll, "XInputGetBatteryInformation");
	XInput1_4.oXInputGetCapabilities = GetProcAddress(XInput1_4.dll, "XInputGetCapabilities");
	XInput1_4.oXInputGetKeystroke = GetProcAddress(XInput1_4.dll, "XInputGetKeystroke");
	XInput1_4.oXInputGetState = GetProcAddress(XInput1_4.dll, "XInputGetState");
	XInput1_4.oXInputSetState = GetProcAddress(XInput1_4.dll, "XInputSetState");
	printf("setup functions end\n");
}
#pragma endregion

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
	switch (ul_reason_for_call) {
	case DLL_PROCESS_ATTACH:
	{
		wchar_t path[MAX_PATH], exePath[MAX_PATH];
		DisableThreadLibraryCalls(hModule);

		GetModuleFileNameW(NULL, exePath, sizeof(exePath));
		PathStripPathW(exePath);

		if (wcscmp(exePath, L"Control.exe") != 0)
		{
			GetModuleFileNameW(hModule, path, sizeof(path));
			PathRemoveFileSpecW(path);

			std::wstring pluginsPath = path;
			pluginsPath += L"\\plugins";

			for (auto& entry : std::filesystem::directory_iterator(pluginsPath))
				if (strcmp(entry.path().extension().generic_string().c_str(), ".dll") == 0)
					LoadLibraryW(entry.path().c_str());
		}

		GetWindowsDirectoryW(path, sizeof(path));
		wcscat_s(path, L"\\System32\\XInput1_4.dll");
		XInput1_4.dll = LoadLibraryW(path);
		setupFunctions();

		break;
	}
	case DLL_PROCESS_DETACH:
		break;
	}
	return 1;
}
