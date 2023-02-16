#pragma once
#include "BaseUI.h"
#include "../Config/NieRAutomataConfig.h"

class NieRAutomataUI : public virtual BaseUI {
public:
	void RenderGUI();
protected:
	void ConfigTab();
};