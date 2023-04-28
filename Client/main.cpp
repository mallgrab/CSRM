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

#define HOOK_DXGI_FLIP_DISCARD 1
//hooks D3D11CreateDeviceAndSwapChain & overrides default presentation mode to use independent flip model
//this improves performance & latency in windowed mode for DX11 games, Windows 10+ only
//only doing this on Control as it's causing issues in Nier: Automata

#if HOOK_DXGI_FLIP_DISCARD
static bool setDX11IndependentFlipModel = false;
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
	if (setDX11IndependentFlipModel && pSwapChainDesc->BufferCount != 1) //if (*ppSwapChain == nullptr)
	{
		pSwapChainDesc->Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING;
		pSwapChainDesc->SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	}

	HRESULT hResult = imp_D3D11CreateDeviceAndSwapChain(pAdapter, DriverType, Software, Flags, pFeatureLevels, FeatureLevels, SDKVersion, pSwapChainDesc, ppSwapChain, ppDevice, pFeatureLevel, ppImmediateContext);
	printf("made swapchain\n");

	return hResult;
}
#endif

DWORD WINAPI MainThread(LPVOID lpReserved) {
	wchar_t path[FILENAME_MAX];
	wchar_t filename[FILENAME_MAX];

	ConsoleSetup();
	MH_Initialize();

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

	OcularDLLProxy::Init();

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