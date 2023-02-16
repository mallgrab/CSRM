// Resources used from https://github.com/Rebzzel/kiero/blob/master/examples/imgui/impl/d3d9_impl.cpp https://github.com/rdbo/ImGui-DirectX-9-Kiero-Hook/blob/master/ImGui%20DirectX%209%20Kiero%20Hook%202/main.cpp
#include "D3D9Hook.h"

typedef LRESULT(CALLBACK* WNDPROC)(HWND, UINT, WPARAM, LPARAM);
extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

static HWND window;
static WNDPROC oWndProc;

typedef HRESULT(_stdcall* Reset)(IDirect3DDevice9*, D3DPRESENT_PARAMETERS*);
Reset oReset;
typedef HRESULT(__stdcall* EndScene)(IDirect3DDevice9*);
EndScene oEndScene;

static bool init = false;

void InitImGui(LPDIRECT3DDEVICE9 pDevice) {
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags = ImGuiConfigFlags_NoMouseCursorChange;
	io.MouseDrawCursor = true;
	ImGui_ImplWin32_Init(window);
	ImGui_ImplDX9_Init(pDevice);
}

LRESULT __stdcall D3D9Hook::WndProc(const HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	
	if (uMsg == WM_KEYDOWN) {
		HandleKeypress(wParam);
	}


	if (GetImGuiWndProcHandlerToggle() && init) {
		ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam);
		//return true;	// Uncommenting this should block input when the menu is open but I dunno it's unreliable
	}
	
	return CallWindowProc(oWndProc, hWnd, uMsg, wParam, lParam);
}

long __stdcall D3D9Hook::hkEndScene(IDirect3DDevice9* pDevice) {
	if (!init) {
		std::cout << "[Ocular][D3D9 Hook] Initializing ImGui implementation..." << std::endl;
		D3DDEVICE_CREATION_PARAMETERS params;
		pDevice->GetCreationParameters(&params);
		window = params.hFocusWindow;
		if (oWndProc == NULL) oWndProc = (WNDPROC)GetWindowLongPtr(window, GWLP_WNDPROC);

		(WNDPROC)SetWindowLongPtr(window, GWLP_WNDPROC, (LONG_PTR)WndProc);

		InitImGui(pDevice);
		init = true;
		std::cout << "[Ocular][D3D9 Hook] ImGui implementation initialized!" << std::endl;
	}
	else {
		Preframe();
		ImGui_ImplDX9_NewFrame();
		ImGui_ImplWin32_NewFrame();

		RenderFrame();

		ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
	}

	return oEndScene(pDevice);
}

HRESULT __stdcall hkReset(IDirect3DDevice9* pDevice, D3DPRESENT_PARAMETERS* pPresentationParameters) {
	std::cout << "[Ocular][D3D9 Hook] D3D9 Reset." << std::endl;

	//window = pPresentationParameters->hDeviceWindow;
	//oWndProc = (WNDPROC)SetWindowLongPtr(window, GWL_WNDPROC, (LONG_PTR)WndProc);

	long result;
	if (init) {
		ImGui_ImplDX9_Shutdown();
		ImGui_ImplWin32_Shutdown();
		ImGui_ImplDX9_InvalidateDeviceObjects();
		result = oReset(pDevice, pPresentationParameters);
		ImGui_ImplDX9_CreateDeviceObjects();
		init = false;
	}
	else {
		result = oReset(pDevice, pPresentationParameters);
	}

	return result;
}

D3D9Hook::D3D9Hook() {
	if (kiero::bind(16, (void**)&oReset, hkReset) != kiero::Status::Success) return;
	std::cout << "[Ocular] Kiero bound D3D9 Reset: " << oReset << std::endl;
	if (kiero::bind(42, (void**)&oEndScene, hkEndScene) != kiero::Status::Success) return;
	std::cout << "[Ocular] Kiero bound D3D9 EndScene: " << oEndScene << std::endl;
	hooked = true;
	std::cout << "[Ocular] Hooked! " << std::endl;
}