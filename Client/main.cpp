#include "ocular/ocular.h"
#include "ocular/DLLProxy.h"

#include "PSync/PSyncFactory.h"

FILE* stream;

#define LOG_CONSOLE

void ConsoleSetup() {
#if defined(LOG_CONSOLE)
	AllocConsole();
	freopen_s((FILE**)stdout, "CONOUT$", "w", stdout);
	freopen_s((FILE**)stdout, "CONOUT$", "w", stderr);
	freopen_s((FILE**)stdout, "CONOUT$", "r", stdin);
#elif defined(LOG_FILE)
	freopen_s(&stream, "log.txt", "w", stdout);
	freopen_s(&stream, "log.txt", "w", stderr);
	freopen_s(&stream, "log.txt", "w", stdin);
#elif defined(LOG_NOTHING)
	stream = fopen("nul", "w");

	freopen_s(&stream, "nul", "w", stdout);
	freopen_s(&stream, "nul", "w", stderr);
	freopen_s(&stream, "nul", "w", stdin);
#endif
}

DWORD WINAPI MainThread(LPVOID lpReserved) {
	ConsoleSetup();
	OcularDLLProxy::Init();
	
	OcularHook oHook = Ocular::Init();
	PSyncFactory psync;

	psync.PSyncMod(oHook);

	return TRUE;
}

BOOL WINAPI DllMain(HMODULE hMod, DWORD dwReason, LPVOID lpReserved) {
	switch (dwReason)
	{
	case DLL_PROCESS_ATTACH:
		DisableThreadLibraryCalls(hMod);
		CreateThread(nullptr, 0, MainThread, hMod, 0, nullptr);
		break;
	case DLL_PROCESS_DETACH:
		fflush(stdout);
		fclose(stream);
		Ocular::Shutdown();
		break;
	}
	return TRUE;
}