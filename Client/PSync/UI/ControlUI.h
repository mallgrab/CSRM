#pragma once
#include "BaseUI.h"
#include "../GameData/Control/GameData.h"
#include "../Config/ControlConfig.h"

class ControlUI : public virtual BaseUI {
public:
	//ControlConfig *cfg = (ControlConfig *)config;
	void KeyPress(WPARAM key);
	void Init();
	void RenderGUI();
	void RenderOSD();
protected:
	void ConfigTab();
	void DebugTab();
	void LootDropTab();
	void PracticeTab();
};