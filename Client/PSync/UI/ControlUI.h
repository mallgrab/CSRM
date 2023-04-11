#pragma once
#include "BaseUI.h"
#include "../GameData/Control/GameData.h"
#include "../Config/BaseConfig.h"

class ControlUI : public virtual BaseUI {
public:
	void KeyPress(WPARAM key);
	void Init();
	void RenderGUI();
	void RenderOSD();
protected:
	void ConfigTab();
	void DebugTab();
	void DrawPlayerObjects();
	void G_DebugBoxLines(ImDrawList* drawList, ImVec2 screenSize, Matrix4* viewMatrix, Vector3* position, float scale, ImColor color, float thickness);
};