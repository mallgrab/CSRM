#pragma once
#include "OcularHook.h"

#include <d3d11.h>
#include <dxgi.h>
#include <iostream>

#include "../kiero/kiero.h"
#include "../imgui/imgui.h"
#include "../imgui/imgui_impl_win32.h"
#include "../imgui/imgui_impl_dx11.h"

class D3D11Hook : public OcularHook {
	private:
		static HRESULT __stdcall hkPresent(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags);
	public:
		D3D11Hook();
		static LRESULT __stdcall WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
};