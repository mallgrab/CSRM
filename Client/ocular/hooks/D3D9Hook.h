#pragma once
#include "OcularHook.h"

#include <d3d9.h>
//#include <d3dx9.h>
#include <dxgi.h>
#include <iostream>

#include "../kiero/kiero.h"
#include "../imgui/imgui.h"
#include "../imgui/imgui_impl_win32.h"
#include "../imgui/imgui_impl_dx9.h"

class D3D9Hook : public OcularHook {
private:
	static long __stdcall hkEndScene(LPDIRECT3DDEVICE9 pDevice);
	static LRESULT __stdcall WndProc(const HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
public:
	D3D9Hook();
};