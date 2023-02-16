#include "OcularHook.h"

std::vector<std::function<void()>> OcularHook::preframeFunctions;
std::vector<std::function<void()>> OcularHook::renderFunctions;
std::vector<std::function<void(WPARAM key)>> OcularHook::keypressFunctions;

bool* OcularHook::useImguiWndProcHandler = nullptr;

void OcularHook::SetImGuiWndProcHandlerToggle(bool* boolPtr) {
	useImguiWndProcHandler = boolPtr;
}

bool OcularHook::GetImGuiWndProcHandlerToggle() {
	if (useImguiWndProcHandler == nullptr) {
		return false;
	}
	else {
		return *useImguiWndProcHandler;
	}
}

// Preframe Functions
void OcularHook::BindPreframeFunction(std::function<void()> function) {
	auto func = std::bind(function);
	preframeFunctions.push_back(func);
}

void OcularHook::ClearPreframeFunctions() {
	preframeFunctions.clear();
}

void OcularHook::Preframe() {
	for (auto func : preframeFunctions) {
		func();
	}
}

// Render Functions
void OcularHook::BindRenderFunction(std::function<void()> function) {
	auto func = std::bind(function);
	renderFunctions.push_back(func);
}

void OcularHook::ClearRenderFunctions() {
	renderFunctions.clear();
}

void OcularHook::RenderFrame() {
	ImGui::NewFrame();

	for (auto func : renderFunctions) {
		func();
	}

	ImGui::EndFrame();
	ImGui::Render();
}

// Keypress Functions
void OcularHook::BindKeypressFunction(std::function<void(WPARAM key)> function) {
	auto func = std::bind(function, std::placeholders::_1);
	keypressFunctions.push_back(func);
}

void OcularHook::ClearKeypressFunctions() {
	keypressFunctions.clear();
}

void OcularHook::HandleKeypress(WPARAM key) {
	for (auto func : keypressFunctions) {
		func(key);
	}
}