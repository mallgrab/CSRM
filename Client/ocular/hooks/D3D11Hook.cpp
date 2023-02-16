// Resources used from https://github.com/rdbo/ImGui-DirectX-11-Kiero-Hook
#include "D3D11Hook.h"

typedef LRESULT(CALLBACK* WNDPROC)(HWND, UINT, WPARAM, LPARAM);
extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

static HWND window;
static WNDPROC oWndProc;

ID3D11Device* pDevice = NULL;
ID3D11DeviceContext* pContext = NULL;
ID3D11RenderTargetView* mainRenderTargetView;

typedef HRESULT(__stdcall* Present) (IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags);
Present oPresent;
typedef HRESULT(__stdcall* ResizeBuffers)(IDXGISwapChain* pThis, UINT BufferCount, UINT Width, UINT Height, DXGI_FORMAT NewFormat, UINT SwapChainFlags);
ResizeBuffers oResizeBuffers;

ID3D11Texture2D* prevBackBuffer;


static bool init = false;

void InitImGui() {
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags = ImGuiConfigFlags_NoMouseCursorChange;
	ImGui_ImplWin32_Init(window);
	ImGui_ImplDX11_Init(pDevice, pContext);
}

LRESULT __stdcall D3D11Hook::WndProc(const HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	if (!oWndProc)
		printf("its over!!!!!!\n");

	if (uMsg == WM_KEYDOWN) {
		HandleKeypress(wParam);
	}

	if (GetImGuiWndProcHandlerToggle() && init) {
		ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam);
		//return true;	// Uncommenting this should block input when the menu is open but I dunno it's unreliable
	}

	return CallWindowProc(oWndProc, hWnd, uMsg, wParam, lParam);
}

void SetupRenderTargets(IDXGISwapChain* pSwapchain) {
	std::cout << "[Ocular][D3D11 Hook] Setting up render targets." << std::endl;
	ID3D11Texture2D* pBackBuffer;
	pSwapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
	pDevice->CreateRenderTargetView(pBackBuffer, NULL, &mainRenderTargetView);
	prevBackBuffer = pBackBuffer;
	pBackBuffer->Release();

	pContext->OMSetRenderTargets(1, &mainRenderTargetView, NULL);
}

// If ResizeBuffer is called, let's setup the render targets again, they might have broken.
HRESULT __stdcall hkResizeBuffers(IDXGISwapChain* pThis, UINT BufferCount, UINT Width, UINT Height, DXGI_FORMAT NewFormat, UINT SwapChainFlags) {
	std::cout << "[Ocular][D3D11 Hook] ResizeBuffers called." << std::endl;
	if (mainRenderTargetView) {
		pContext->OMSetRenderTargets(0, 0, 0);
		mainRenderTargetView->Release();
	}
	HRESULT result = oResizeBuffers(pThis, BufferCount, Width, Height, NewFormat, SwapChainFlags);

	std::cout << "[Ocular][D3D11 Hook] ResizeBuffers -> Resolution: " << Width << ", " << Height << std::endl;

	if (!pDevice) {
		std::cout << "[Ocular][D3D11 Hook] ResizeBuffers -> Device did not exist!" << std::endl;
	}
	else if (init) {	
		SetupRenderTargets(pThis);

		// Some users have problems with WndProc getting broken after window is changed, let's make sure to grab it again to fix this? (Hopefully)
		DXGI_SWAP_CHAIN_DESC sd;
		pThis->GetDesc(&sd);
		window = sd.OutputWindow;
		(WNDPROC)SetWindowLongPtrA(window, GWLP_WNDPROC, (LONG_PTR)D3D11Hook::WndProc);
		
		std::cout << "[Ocular][D3D11 Hook] Recreated WndProc." << std::endl;
	}

	return result;
}

// We've hooked the Present function, let's setup the render targets and ImGui if this is our first time.
HRESULT __stdcall D3D11Hook::hkPresent(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags) {
	if (!init)
	{
		std::cout << "[Ocular][D3D11 Hook] Initializing ImGui implementation..." << std::endl;
		if (SUCCEEDED(pSwapChain->GetDevice(__uuidof(ID3D11Device), (void**)&pDevice)))
		{
			pDevice->GetImmediateContext(&pContext);
			DXGI_SWAP_CHAIN_DESC sd;
			pSwapChain->GetDesc(&sd);
			window = sd.OutputWindow;

			SetupRenderTargets(pSwapChain);
			oWndProc = (WNDPROC)SetWindowLongPtrA(window, GWLP_WNDPROC, (LONG_PTR)WndProc);

			InitImGui();
			init = true;
			std::cout << "[Ocular][D3D11 Hook] ImGui implementation initialized!" << std::endl;
		}

		else
			return oPresent(pSwapChain, SyncInterval, Flags);
	}
	
	ID3D11Texture2D* curBackBuffer;
	pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&curBackBuffer);
	if (curBackBuffer && prevBackBuffer != curBackBuffer) {
		std::cout << "[Ocular][D3D11 Hook] BackBuffer Changed!" << std::endl;

		if (mainRenderTargetView) {
			mainRenderTargetView->Release();
		}

		pDevice->CreateRenderTargetView(curBackBuffer, NULL, &mainRenderTargetView);
		prevBackBuffer = curBackBuffer;
	}
	curBackBuffer->Release();
	

	Preframe();
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	RenderFrame();

	pContext->OMSetRenderTargets(1, &mainRenderTargetView, NULL);
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	return oPresent(pSwapChain, SyncInterval, Flags);
}


D3D11Hook::D3D11Hook() {
	if (kiero::bind(8, (void**)&oPresent, hkPresent) != kiero::Status::Success) return;
	std::cout << "[Ocular] Kiero bound D3D11 Present." << std::endl;
	if (kiero::bind(13, (void**)&oResizeBuffers, hkResizeBuffers) != kiero::Status::Success) return;
	std::cout << "[Ocular] Kiero bound D3D11 ResizeBuffers." << std::endl;
	hooked = true;
	std::cout << "[Ocular] Hooked!" << std::endl;
}