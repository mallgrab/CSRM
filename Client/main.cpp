#include "ocular/ocular.h"
#include "ocular/DLLProxy.h"
#include "PSync/PSyncFactory.h"

#include "PSync/GameData/Control/StartupString.h"

FILE* stream;

#define LOG_CONSOLE
//#define LOG_NOTHING

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

#if HOOK_DXGI_FLIP_DISCARD
//hooks D3D11CreateDeviceAndSwapChain & overrides default presentation mode to use independent flip model
//this improves performance & latency in windowed mode for DX11 games, Windows 10+ only
//only doing this on Control as it's causing issues in Nier: Automata
bool setDX11IndependentFlipModel = false;

typedef HRESULT(*D3D11CreateDeviceAndSwapChain_t)(
	_In_opt_        IDXGIAdapter* pAdapter,
	D3D_DRIVER_TYPE      DriverType,
	HMODULE              Software,
	UINT                 Flags,
	_In_opt_  const D3D_FEATURE_LEVEL* pFeatureLevels,
	UINT                 FeatureLevels,
	UINT                 SDKVersion,
	_In_opt_  const DXGI_SWAP_CHAIN_DESC* pSwapChainDesc,
	_Out_opt_       IDXGISwapChain** ppSwapChain,
	_Out_opt_       ID3D11Device** ppDevice,
	_Out_opt_       D3D_FEATURE_LEVEL* pFeatureLevel,
	_Out_opt_       ID3D11DeviceContext** ppImmediateContext
	);
D3D11CreateDeviceAndSwapChain_t imp_D3D11CreateDeviceAndSwapChain = NULL;
// D3D11CreateDeviceAndSwapChain

HRESULT WINAPI hkD3D11CreateDeviceAndSwapChain(IDXGIAdapter* pAdapter, D3D_DRIVER_TYPE DriverType, HMODULE Software, UINT Flags, const D3D_FEATURE_LEVEL* pFeatureLevels, UINT FeatureLevels, UINT SDKVersion, DXGI_SWAP_CHAIN_DESC* pSwapChainDesc, IDXGISwapChain** ppSwapChain, ID3D11Device** ppDevice, D3D_FEATURE_LEVEL* pFeatureLevel, ID3D11DeviceContext** ppImmediateContext)
{
	if (setDX11IndependentFlipModel && pSwapChainDesc && pSwapChainDesc->BufferCount >= 2) //if (*ppSwapChain == nullptr)
	{
		pSwapChainDesc->Flags |= DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING;
		pSwapChainDesc->SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD; //lower latency than sequential
	}

	HRESULT hResult = imp_D3D11CreateDeviceAndSwapChain(pAdapter, DriverType, Software, Flags, pFeatureLevels, FeatureLevels, SDKVersion, pSwapChainDesc, ppSwapChain, ppDevice, pFeatureLevel, ppImmediateContext);
	
	printf("made swapchain\n");
	return hResult;
}
#endif

#define HOOK_PRINTF 0
// TODO: check if we can hook network printf only so we can then sort them by modules

#if HOOK_PRINTF
using stdio_common_vsnprintf_s_t = int(__fastcall*)(uint64_t options, char* buffer, size_t bufferCount, size_t maxCount, char* format, _locale_t locale, va_list arglist);
stdio_common_vsnprintf_s_t stdio_common_vsnprintf_s_Func;
int stdio_common_vsnprintf_s(uint64_t options, char* buffer, size_t bufferCount, size_t maxCount, char* format, _locale_t locale, va_list arglist)
{
	int result = stdio_common_vsnprintf_s_Func(options, buffer, bufferCount, maxCount, format, locale, arglist);
#if HOOK_PRINTF & 1
	if (buffer && buffer[0] != '\0') printf("%s\n", buffer);
#endif
	return result;
}

using stdio_common_vsprintf_t = int(__fastcall*)(uint64_t options, void* buffer, size_t bufferCount, void* format, void* locale, void* arglist);
stdio_common_vsprintf_t stdio_common_vsprintf_Func;
int stdio_common_vsprintf(uint64_t options, char* buffer, size_t bufferCount, void* format, void* locale, void* arglist)
{
	int result = stdio_common_vsprintf_Func(options, buffer, bufferCount, format, locale, arglist);
#if HOOK_PRINTF & 2
	if (buffer && buffer[0] != '\0') printf("%s\n", buffer);
#endif
	return result;
}

using stdio_common_vsprintf_s_t = int(__fastcall*)(uint64_t options, void* buffer, size_t bufferCount, void* format, void* locale, void* arglist);
stdio_common_vsprintf_s_t stdio_common_vsprintf_s_Func;
int stdio_common_vsprintf_s(uint64_t options, char* buffer, size_t bufferCount, void* format, void* locale, void* arglist)
{
	int result = stdio_common_vsprintf_s_Func(options, buffer, bufferCount, format, locale, arglist);
#if HOOK_PRINTF & 4
	if (buffer && buffer[0] != '\0') printf("%s\n", buffer);
#endif
	return result;
}
#endif

using StartupString_t = void(__fastcall*)(startupString* a1);
StartupString_t StartupStringFunc;
startupString* tmpString;
void StartupString(startupString* a1)
{
	printf("startup string made: 0x%llx\n", (uint64_t)a1);
	StartupStringFunc(a1);
	tmpString = a1;

#ifdef _DEBUG
	a1->devmode = true;
	a1->skipToMenu = true;
#endif

	a1->skipStartupLogos = true;
	a1->startAutoJoinMultiplayerGame = false;
}

void earlyHooks()
{
	MH_Initialize();
	ConsoleSetup();

	uint64_t processStartAddr = reinterpret_cast<uint64_t>(GetModuleHandle(nullptr));
	byte* startupStringPtr = reinterpret_cast<byte*>(processStartAddr + 0x24FE20);
	if (MH_CreateHook(startupStringPtr, &StartupString, reinterpret_cast<LPVOID*>(&StartupStringFunc)) != MH_OK) throw;
	if (MH_EnableHook(startupStringPtr) != MH_OK) throw;

	SetConsoleTitle(TEXT("CSRM"));
#if defined(LOG_CONSOLE) && !defined(_DEBUG)
	ShowWindow(GetConsoleWindow(), SW_MINIMIZE); //minimize console window on startup
#endif
}

DWORD WINAPI MainThread(LPVOID lpReserved) {
	wchar_t path[FILENAME_MAX], filename[FILENAME_MAX];

#if HOOK_PRINTF
	ptr* stdio_common_vsnprintf_s_Ptr = (ptr*)GetProcAddress(GetModuleHandle(L"api-ms-win-crt-stdio-l1-1-0.dll"), "__stdio_common_vsnprintf_s");
	if (MH_CreateHook(stdio_common_vsnprintf_s_Ptr, &stdio_common_vsnprintf_s, reinterpret_cast<LPVOID*>(&stdio_common_vsnprintf_s_Func)) != MH_OK) throw;
	if (MH_EnableHook(stdio_common_vsnprintf_s_Ptr) != MH_OK) throw;

	ptr* stdio_common_vsprintf_Ptr = (ptr*)GetProcAddress(GetModuleHandle(L"api-ms-win-crt-stdio-l1-1-0.dll"), "__stdio_common_vsprintf");
	if (MH_CreateHook(stdio_common_vsprintf_Ptr, &stdio_common_vsprintf, reinterpret_cast<LPVOID*>(&stdio_common_vsprintf_Func)) != MH_OK) throw;
	if (MH_EnableHook(stdio_common_vsprintf_Ptr) != MH_OK) throw;

	ptr* stdio_common_vsprintf_s_Ptr = (ptr*)GetProcAddress(GetModuleHandle(L"api-ms-win-crt-stdio-l1-1-0.dll"), "__stdio_common_vsprintf_s");
	if (MH_CreateHook(stdio_common_vsprintf_s_Ptr, &stdio_common_vsprintf_s, reinterpret_cast<LPVOID*>(&stdio_common_vsprintf_s_Func)) != MH_OK) throw;
	if (MH_EnableHook(stdio_common_vsprintf_s_Ptr) != MH_OK) throw;
#endif

	GetModuleFileName(NULL, path, MAX_PATH);
	_wsplitpath_s(path, NULL, NULL, NULL, NULL, filename, FILENAME_MAX, NULL, NULL);

#if HOOK_DXGI_FLIP_DISCARD
	if (!wcscmp(filename, L"Control_DX11")) {
		setDX11IndependentFlipModel = true;
		HMODULE D3D11Handle = GetModuleHandle(L"D3D11.dll");
		if (D3D11Handle) {
			ptr *createDeviceAndSwapChainPtr = (ptr *)GetProcAddress(D3D11Handle, "D3D11CreateDeviceAndSwapChain");
			if (createDeviceAndSwapChainPtr) {
				if (MH_CreateHook(createDeviceAndSwapChainPtr, &hkD3D11CreateDeviceAndSwapChain, reinterpret_cast<LPVOID*>(&imp_D3D11CreateDeviceAndSwapChain)) != MH_OK) throw;
				if (MH_EnableHook(createDeviceAndSwapChainPtr) != MH_OK) throw;
			}
		}
	}
#endif

	//OcularDLLProxy::Init();
	OcularHook oHook = Ocular::Init();
	
	PSyncFactory psync;
	psync.PSyncMod(oHook, path, filename);

	return TRUE;
}

BOOL WINAPI DllMain(HMODULE hMod, DWORD dwReason, LPVOID lpReserved) {
	switch (dwReason)
	{
	case DLL_PROCESS_ATTACH:
		DisableThreadLibraryCalls(hMod);
		earlyHooks();
		
		//OcularDLLProxy::Init();
		CreateThread(nullptr, 0, MainThread, hMod, 0, nullptr);
		break;
	case DLL_PROCESS_DETACH:
		printf("deatach called\n");
		fflush(stdout);

		Ocular::Shutdown();
		MH_RemoveHook(MH_ALL_HOOKS);
		MH_Uninitialize();
		oHook.D3D11UnHook();
		
		SetWindowLongPtrA(window_DX11, GWLP_WNDPROC, (LONG_PTR)oWndProc_DX11);

		break;
	}
	return TRUE;
}
