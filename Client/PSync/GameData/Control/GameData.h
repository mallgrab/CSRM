#pragma once
#define _WINSOCKAPI_
#include <Windows.h>
#include "../BaseGameData.h"
#include "../../Config/ControlConfig.h"
#include "../ocular/kiero/minhook/include/MinHook.h"
#include "BaseTweakable.h"
#include "ShapeEngine.h"
#include "Trigger.h"
#include "RemedyLib.h"
#include "StartupString.h"
#include "LootDrop.h"

struct Point {
	Vector3 a;
	Vector3 b;
};

struct triggerObject {
	uint64_t* trigger;
	Vector3 pos;
	uint32_t* onEnter;
	Point minmax;
};

class ControlGameData : public BaseGameData {
public:
	void InitGameData() override;
	Vector3* GetPlayerPos() override;
	Matrix4* GetViewMatrix() override;
	bool GetMapIsLoaded();
	float* getPlayerCapsuleSpeed();
	float getPlayerPhysxSpeed();
	float getPlayerPosSpeed();
	void SetPlayerPos(Vector3 newPos);
	Vector3* GetPlayerPos_Real();
	bool playerIsOnGround();
	void ToggleFreeCam();
	void ToggleDeveloperMenus();
	void UpdateStartupStringValues(ControlConfig *cfg);
};
