#pragma once
#define DIRECTINPUT_VERSION 0x0800

#include <iostream>
#include <string>

#include <dinput.h>
#include <xinput.h>
#include <dxgi.h>
#include <dxgi1_3.h>
#include <d3d9.h>
#include <dsound.h>
/*
namespace OcularDLLProxy {
	HMODULE dinput_lib;
	HMODULE xinput_lib;
	HMODULE dxgi_lib;
	HMODULE d3d9_lib;
	HMODULE dsound_lib;

	enum class Result {
		GetModuleHandleExFailure = -1,
		GetModuleFileNameFailure = -2,
		LoadLibraryWFailure = -3,
		DLLFilenameNotRecognized = -4,
		GetSystemDirectoryWFailure = -5,

		Success = 0,
	};

	Result LoadOriginalDLLs();

	inline void Init() {
		Result result = LoadOriginalDLLs();
		if (result != Result::Success && result != Result::DLLFilenameNotRecognized) {
			MessageBoxA(GetActiveWindow(), ("LoadOriginalDLLS Error: " + std::to_string(static_cast<int>(result))).c_str(), "OcularDLLProxy failed!", MB_OK);
			//ExitProcess(0);
		}
	}

	inline Result LoadOriginalDLLs() {
		wchar_t path[MAX_PATH];
		wchar_t filename[FILENAME_MAX];

		wchar_t buffer[MAX_PATH]{ 0 };

		HMODULE hm = NULL;

		// Get a handle to ourself
		if (GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT, (LPCWSTR)&LoadOriginalDLLs, &hm) == 0) {
			int ret = GetLastError();
			std::cout << "GetModuleHandleEx failed!" << ret << std::endl;
			return Result::GetModuleHandleExFailure;
		}

		// Get our filepath
		if (GetModuleFileName(hm, path, MAX_PATH) == 0) {
			int ret = GetLastError();
			std::cout << "GetModuleFileName failed!" << ret << std::endl;
			return Result::GetModuleFileNameFailure;
		}

		// Get our filename
		_wsplitpath_s(path, NULL, NULL, NULL, NULL, filename, FILENAME_MAX, NULL, NULL);

		// Get the system directory.
		if (GetSystemDirectoryW(buffer, MAX_PATH) != 0) {
			// Load the approprate DLL according to our filename.
			if (wcscmp(filename, L"DINPUT8") == 0 || wcscmp(filename, L"dinput8") == 0) {
				if ((dinput_lib = LoadLibraryW((std::wstring{ buffer } + L"\\dinput8.dll").c_str())) == NULL) return Result::LoadLibraryWFailure;
			}
			else if (wcscmp(filename, L"XINPUT1_4") == 0 || wcscmp(filename, L"xinput1_4") == 0) {
				if ((xinput_lib = LoadLibraryW((std::wstring{ buffer } + L"\\xinput1_4.dll").c_str())) == NULL) return Result::LoadLibraryWFailure;
			}
			else if (wcscmp(filename, L"DXGI") == 0 || wcscmp(filename, L"dxgi") == 0) {
				if ((dxgi_lib = LoadLibraryW((std::wstring{ buffer } + L"\\dxgi.dll").c_str())) == NULL) return Result::LoadLibraryWFailure;
			}
			else if (wcscmp(filename, L"D3D9") == 0 || wcscmp(filename, L"d3d9") == 0) {
				if ((d3d9_lib = LoadLibraryW((std::wstring{ buffer } + L"\\d3d9.dll").c_str())) == NULL) return Result::LoadLibraryWFailure;
			}
			else if (wcscmp(filename, L"DSOUND") == 0 || wcscmp(filename, L"dsound") == 0) {
				if ((dsound_lib = LoadLibraryW((std::wstring{ buffer } + L"\\dsound.dll").c_str())) == NULL) return Result::LoadLibraryWFailure;
			}
			else if (wcscmp(filename, L"OLE32") == 0 || wcscmp(filename, L"ole32") == 0) {
				if ((dsound_lib = LoadLibraryW((std::wstring{ buffer } + L"\\ole32.dll").c_str())) == NULL) return Result::LoadLibraryWFailure;
			}
			else {
				return Result::DLLFilenameNotRecognized;
			}
		}
		else {
			return Result::GetSystemDirectoryWFailure;
		}

		return Result::Success;
	}
	
	extern "C" {
		// dinput8.dll
		
		#if _WIN64
		inline __declspec(dllexport) HRESULT WINAPI custom_DirectInput8Create(HINSTANCE hinst, DWORD dwVersion, REFIID riidltf, LPVOID* ppvOut, LPUNKNOWN punkOuter) {
			#pragma comment(linker, "/EXPORT:DirectInput8Create=custom_DirectInput8Create")
			return ((decltype(DirectInput8Create)*)GetProcAddress(dinput_lib, "DirectInput8Create"))(hinst, dwVersion, riidltf, ppvOut, punkOuter);
		}
		#else
		
		inline __declspec(dllexport) HRESULT WINAPI custom_DirectInput8Create(HINSTANCE hinst, DWORD dwVersion, REFIID riidltf, LPVOID* ppvOut, LPUNKNOWN punkOuter) {
			#pragma comment(linker, "/EXPORT:DirectInput8Create=_custom_DirectInput8Create@20")
			return ((decltype(DirectInput8Create)*)GetProcAddress(dinput_lib, "DirectInput8Create"))(hinst, dwVersion, riidltf, ppvOut, punkOuter);
		}
		
		#endif
		
		#if _WIN64
		// xinput1_4.dll
		inline __declspec(dllexport) DWORD WINAPI custom_XInputGetState(DWORD dwUserIndex, XINPUT_STATE *pState) {
			#pragma comment(linker, "/EXPORT:XInputGetState=custom_XInputGetState")
			return ((decltype(XInputGetState)*)GetProcAddress(xinput_lib, "XInputGetState"))(dwUserIndex, pState);
		}

		// dxgi.dll
		inline __declspec(dllexport) DWORD WINAPI custom_CreateDXGIFactory(REFIID riid, void** ppFactory) {
			#pragma comment(linker, "/EXPORT:CreateDXGIFactory=custom_CreateDXGIFactory")
			return ((decltype(CreateDXGIFactory)*)GetProcAddress(dxgi_lib, "CreateDXGIFactory"))(riid, ppFactory);
		}
		
		inline __declspec(dllexport) DWORD WINAPI custom_CreateDXGIFactory1(REFIID riid, void** ppFactory) {
			#pragma comment(linker, "/EXPORT:CreateDXGIFactory1=custom_CreateDXGIFactory1")
			return ((decltype(CreateDXGIFactory1)*)GetProcAddress(dxgi_lib, "CreateDXGIFactory1"))(riid, ppFactory);
		}
		
		inline __declspec(dllexport) DWORD WINAPI custom_CreateDXGIFactory2(UINT flags, REFIID riid, void** ppFactory) {
			#pragma comment(linker, "/EXPORT:CreateDXGIFactory2=custom_CreateDXGIFactory2")
			return ((decltype(CreateDXGIFactory2)*)GetProcAddress(dxgi_lib, "CreateDXGIFactory2"))(flags, riid, ppFactory);
		}
		#endif

		#if _WIN64
			//Hello There
		#else
		// d3d9.dll
		/*
		inline __declspec(dllexport) IDirect3D9* WINAPI custom_Direct3DCreate9(UINT SDKVersion) {
			#pragma comment(linker, "/EXPORT:Direct3DCreate9=custom_Direct3DCreate9")
			return ((decltype(Direct3DCreate9)*)GetProcAddress(d3d9_lib, "Direct3DCreate9"))(SDKVersion);
		}
		
		// dsound.dll
		inline __declspec(dllexport) HRESULT WINAPI custom_DirectSoundFullDuplexCreate
			(LPCGUID pcGuidCaptureDevice,
			LPCGUID pcGuidRenderDevice,
			LPCDSCBUFFERDESC pcDSCBufferDesc,
			LPCDSBUFFERDESC pcDSBufferDesc,
			HWND hWnd,
			DWORD dwLevel,
			LPDIRECTSOUNDFULLDUPLEX* ppDSFD,
			LPDIRECTSOUNDCAPTUREBUFFER8* ppDSCBuffer8,
			LPDIRECTSOUNDBUFFER8* ppDSBuffer8,
			LPUNKNOWN pUnkOuter) {
			#pragma comment(linker, "/EXPORT:DirectSoundFullDuplexCreate=_custom_DirectSoundFullDuplexCreate@20")
			return ((decltype(DirectSoundFullDuplexCreate)*)GetProcAddress(dsound_lib, "DirectSoundFullDuplexCreate"))
				(pcGuidCaptureDevice,
				pcGuidRenderDevice,
				pcDSCBufferDesc,
				pcDSBufferDesc,
				hWnd,
				dwLevel,
				ppDSFD,
				ppDSCBuffer8,
				ppDSBuffer8,
				pUnkOuter);
		
		}
		//#endif
		
	}
}
*/