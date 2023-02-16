#pragma once
#define _WINSOCKAPI_
#include <vector>
#include <functional>
#include <Windows.h>
#include <iostream>

#include "../imgui/imgui.h"

class OcularHook {
private:
	static std::vector<std::function<void()>> preframeFunctions;
	static std::vector<std::function<void()>> renderFunctions;
	static std::vector<std::function<void(WPARAM key)>> keypressFunctions;
	static bool* useImguiWndProcHandler;
public:
	bool hooked = false;
	static void SetImGuiWndProcHandlerToggle(bool* boolPtr);
	static bool GetImGuiWndProcHandlerToggle();

	static void BindPreframeFunction(std::function<void()> function);
	static void ClearPreframeFunctions();

	static void BindRenderFunction(std::function<void()> function);
	static void ClearRenderFunctions();

	static void BindKeypressFunction(std::function<void(WPARAM key)> function);
	static void ClearKeypressFunctions();
protected:
	static void Preframe();
	static void RenderFrame();
	static void HandleKeypress(WPARAM key);
};