#pragma once
#include "BaseUI.h"
#include "../GameData/DevilMayCry3SEGameData.h"
#include "../Config/DevilMayCry3SEConfig.h"

class DevilMayCry3SEUI : public virtual BaseUI {
public:
	void RenderGUI();
protected:
	ImVec2 DrawOctahedron(ImDrawList* drawList, ImVec2 screenSize, Matrix4* viewMatrix, Vector3* position, float scale, ImColor color, float thickness);
	void DrawPlayerObjects();
	void ConfigTab();
};