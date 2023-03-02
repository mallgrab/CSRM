#pragma once
#define _WINSOCKAPI_
#include <Windows.h>
#include "../BaseGameData.h"
#include "../ocular/kiero/minhook/include/MinHook.h"
#include "BaseTweakable.h"

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

struct triggerPtr {
	uint64_t* trigger;
	uint64_t** gameobject;
	uint64_t** networkstate;
	uint64_t* genericentity;
	triggerObject triggerobject;
};

class ControlGameData : public BaseGameData {
public:
	void InitGameData() override;
	Vector3* GetPlayerPos() override;
	Matrix4* GetViewMatrix() override;
	std::vector<triggerPtr> GetTriggers();
	bool GetMapIsLoaded();
	float* getPlayerCapsuleSpeed();
	float getPlayerPhysxSpeed();
	float getPlayerPosSpeed();
	void SetPlayerPos(Vector3 newPos);
	Vector3 *GetPlayerPos_Real();
	int getTriggerCount(void);
};