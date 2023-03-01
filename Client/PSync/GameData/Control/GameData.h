#pragma once
#define _WINSOCKAPI_
#include "../BaseGameData.h"
#include "Windows.h"
#include "../ocular/kiero/minhook/include/MinHook.h"

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

using baseTweakablesGetTweakable_t = uint64_t * (__stdcall*)(const char* name);
extern baseTweakablesGetTweakable_t baseTweakablesGetTweakable;

using baseTweakablesSetTweakable_t = int64_t(__fastcall*)(char* tweakableName, char* value, char isChanged);
extern baseTweakablesSetTweakable_t baseTweakablesSetTweakable;

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

	//ass
	void Tweakable_SetLevitateDisable(bool disableLevitation);
	void Tweakable_SetTemporalSSAA(bool temporalSSAA);
	void Tweakable_SetMotionBlur(bool motionBlur);

	int getTriggerCount(void);
};