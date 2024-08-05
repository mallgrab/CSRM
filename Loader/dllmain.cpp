#include <Windows.h>

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
	XInput1_4.oDllMain = GetProcAddress(XInput1_4.dll, "DllMain");
	XInput1_4.oXInputEnable = GetProcAddress(XInput1_4.dll, "XInputEnable");
	XInput1_4.oXInputGetAudioDeviceIds = GetProcAddress(XInput1_4.dll, "XInputGetAudioDeviceIds");
	XInput1_4.oXInputGetBatteryInformation = GetProcAddress(XInput1_4.dll, "XInputGetBatteryInformation");
	XInput1_4.oXInputGetCapabilities = GetProcAddress(XInput1_4.dll, "XInputGetCapabilities");
	XInput1_4.oXInputGetKeystroke = GetProcAddress(XInput1_4.dll, "XInputGetKeystroke");
	XInput1_4.oXInputGetState = GetProcAddress(XInput1_4.dll, "XInputGetState");
	XInput1_4.oXInputSetState = GetProcAddress(XInput1_4.dll, "XInputSetState");
}
#pragma endregion



BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
	switch (ul_reason_for_call) {
	case DLL_PROCESS_ATTACH:
	{
		DisableThreadLibraryCalls(hModule);

		char path[MAX_PATH];
	
		GetCurrentDirectory(sizeof(path), path);
		strcat_s(path, "\\CSRM.dll");
		HMODULE csrm = LoadLibrary(path);

		GetWindowsDirectory(path, sizeof(path));
		strcat_s(path, "\\System32\\XInput1_4.dll");
		XInput1_4.dll = LoadLibrary(path);
		setupFunctions();

		break;
	}
	case DLL_PROCESS_DETACH:
		//FreeLibraryAndExitThread(hModule, 0);
		break;
	}
	return 1;
}
