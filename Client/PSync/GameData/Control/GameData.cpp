#include "GameData.h"

using setAsPlayerOriginal_t = void(__stdcall*)(uint64_t* x, char y);
setAsPlayerOriginal_t setAsPlayerOriginal;

using triggerComponentCtorOriginal_t = void* (__fastcall*)(uint64_t* x, uint64_t* y);
triggerComponentCtorOriginal_t triggerComponentCtorOriginal;

using triggerComponentDtorOriginal_t = void* (__fastcall*)(uint64_t* x);
triggerComponentDtorOriginal_t triggerComponentDtorOriginal;

using enterTriggerOriginal_t = char(__fastcall*)(uint64_t* x, uint64_t* y, uint64_t z);
enterTriggerOriginal_t enterTriggerOriginal;

using getWorldSpaceAABBOriginal_t = uint64_t * (__fastcall*)(uint64_t x, uint64_t* y);
getWorldSpaceAABBOriginal_t getWorldSpaceAABBOriginal;

using startUnloadingLevel_t = void(__fastcall*)(uint64_t* x);
startUnloadingLevel_t startUnloadingLevelOriginal;

using startLoadingLevel_t = void(__fastcall*)(uint64_t x, uint64_t y, uint64_t z, int64_t a, char b, uint8_t c);
startLoadingLevel_t startLoadingLevelOriginal;

using notifyClientLevelLoadingComplete_t = void(__fastcall*)(uint64_t* x, const int64_t* y);
notifyClientLevelLoadingComplete_t notifyClientLevelLoadingCompleteOriginal;

using characterControllerCtor_t = uint64_t * (__fastcall*)(uint64_t* x, uint64_t* y, uint64_t* z, int a4);
characterControllerCtor_t characterControllerOriginal;

using characterControllerMoveRelative_t = void(__fastcall*)(uint64_t* x, float* y, float z, float w);
characterControllerMoveRelative_t characterControllerMoveCapsuleOriginal;

baseTweakablesGetTweakable_t baseTweakablesGetTweakable;
baseTweakablesSetTweakable_t baseTweakablesSetTweakable;

ptr* playerController;
std::vector<triggerPtr> triggerPtrs;
bool mapIsLoaded;
int triggerCount = 0;

void* __fastcall triggerComponentCtor(uint64_t* pointer, uint64_t* pointer_b) {
	triggerPtr tO = { pointer, (uint64_t**)(pointer + 4), (uint64_t**)(pointer + 1), (uint64_t*)(pointer + 29) };
	uint64_t* aabb = (pointer + 29);

	// don't include triggers with no physx component, can't draw them otherwise
	if (aabb) {
		for (int i = 0; i < triggerPtrs.size(); i++) {
			if (triggerPtrs.at(i).trigger == NULL) {
				triggerPtrs.at(i) = tO;
				break;
			}
		}
	}

	return triggerComponentCtorOriginal(pointer, pointer_b);
}

void* __fastcall triggerComponentDtor(uint64_t* pointer) {
	for (int i = 0; i < triggerPtrs.size(); i++)
		if (triggerPtrs.at(i).trigger == pointer) {
			triggerPtrs.at(i).trigger = NULL;
			triggerPtrs.at(i).gameobject = NULL;
			triggerPtrs.at(i).genericentity = NULL;
			triggerPtrs.at(i).networkstate = NULL;
			// clean triggercomponent too?
		}

	return triggerComponentDtorOriginal(pointer);
}

char __fastcall enterTrigger(uint64_t* pointer, uint64_t* pointer_b, uint64_t a3) {
	printf("trigger: 0x%llx gameobjectstate: 0x%llx a3: %lu\n", pointer, pointer_b, a3);

	triggerCount++;
	return enterTriggerOriginal(pointer, pointer_b, a3);
}

void __stdcall setAsPlayerCharacter(uint64_t* pointer, char isPlayer) {
	if (isPlayer == 1) {
		printf("setAsPlayerCharacter was called: 0x%llx, %d\n", pointer, isPlayer);
		ptr* playerCharacter = reinterpret_cast<ptr*>(pointer + 0xAC);
		playerController = pointer;

		ptr* playerControllerState = reinterpret_cast<ptr*>(playerController + 0x20);
		printf("playerControllerState was called: 0x%llx\n", playerControllerState);
	}

	setAsPlayerOriginal(pointer, isPlayer);
}

void startUnloadingLevel(uint64_t* ptr) {
	printf("start unloading level\n");

	mapIsLoaded = false;
	triggerCount = 0;
	startUnloadingLevelOriginal(ptr);
}

void startLoadingLevel(uint64_t x, uint64_t y, uint64_t z, int64_t a, char b, uint8_t c) {
	printf("start loading level\n");

	mapIsLoaded = false;
	triggerCount = 0;
	startLoadingLevelOriginal(x, y, z, a, b, c);
}

void notifyClientLevelLoadingComplete(uint64_t* x, const int64_t* y) {
	printf("we are done loading the level\n");

	mapIsLoaded = true;
	notifyClientLevelLoadingCompleteOriginal(x, y);
}

uint64_t* __fastcall getWorldSpaceAABB(uint64_t x, uint64_t* y) {
	uint64_t* result = getWorldSpaceAABBOriginal(x, y);
	return result;
}

ptr* playerCharacterController = nullptr;
uint64_t* __fastcall characterControllerCtor(uint64_t* x, uint64_t* y, uint64_t* z, int a4) {
	printf("character controller constructor, 0x%llx, 0x%llx, 0x%llx, %d\n", x, y, z, a4);

	if (a4 == 1)
		playerCharacterController = x;

	characterControllerOriginal(x, y, z, a4);
	return x;
}

float speedOfPlayer[4];
void characterControllerMoveCapsule(uint64_t* x, float* y, float z, float w)
{
	speedOfPlayer[0] = *y;			// x
	speedOfPlayer[1] = *(y + 1);	// y
	speedOfPlayer[2] = *(y + 2);	// z
	speedOfPlayer[3] = sqrtf(speedOfPlayer[0] * speedOfPlayer[0] + speedOfPlayer[2] * speedOfPlayer[2]);

	characterControllerMoveCapsuleOriginal(x, y, z, w);
}

float* ControlGameData::getPlayerCapsuleSpeed()
{
	return &speedOfPlayer[0];
}

float ControlGameData::getPlayerPhysxSpeed()
{
	if (playerCharacterController == nullptr)
		return 0.0f;

	float velocity = *(float*)(playerCharacterController + 8);
	float ground = *(float*)(playerCharacterController + 9);
	float speed = sqrtf(velocity * velocity + ground * ground);

	return speed;
}

#if 0
float ControlGameData::getPlayerPosSpeed()
{
	static Vector3 previous = { 0.0f, 0.0f, 0.0f };
	Vector3 currentPos = playerPos;

	const float frametime = ImGui::GetIO().DeltaTime;
	Vector3 velocity = currentPos - previous;
	float speed = sqrtf(velocity.x * velocity.x + velocity.z * velocity.z) / frametime;
	if (speed > 0)
		printf("deltaTime %f speed %f\n", frametime, speed);

	previous = playerPos;
	return speed;
}
#else
float ControlGameData::getPlayerPosSpeed()
{
	float newSpeed = 0;
	static float speed = newSpeed;
	Vector3 currentPos = *GetPlayerPos();
	static Vector3 previousPos = currentPos;

	static int lastUpdateTime = 0;
	const int curTime = clock();
	const int delta = curTime - lastUpdateTime; //this shit HAS to be throttled I guess?

	if (delta >= 33)
	{ //30hz
		const float deltaTime = delta * 0.001f;
		//Vector3 difference = currentPos - previousPos;
		Vector3 difference = currentPos - previousPos;
		difference.x /= deltaTime;
		difference.y /= deltaTime;
		difference.z /= deltaTime;

		newSpeed = sqrtf(difference.x * difference.x + difference.z * difference.z);
		speed = newSpeed;

		lastUpdateTime = curTime;
		previousPos = currentPos;

		if (speed > 0)
			printf("getPlayerPosSpeed(): deltaTime %f speed %f\n", deltaTime, speed);
	}

	return speed;
}
#endif

struct Tweakable_t
{
	ptr* vftable;
	ptr** namePtr;
	ptr** namePtr2;
	uint32_t a;
	uint32_t b;
	uint32_t charAmount;
	uint32_t d;
};

//should move this to a class or somethin?
struct Tweakable
{
	byte* start;
	char* name;
	byte* valueType;
	byte* valueModified;
	byte* valueByte;
	uint32_t* valueInt;
	float* valueFloat;

	Tweakable(ptr* t)
	{
		Tweakable_t* p = (Tweakable_t*)t;

		this->name = (char*)p->namePtr;
		this->start = (byte*)t;

		this->valueModified = (byte*)(this->start + 0xAC + 0x04);
		this->valueByte = (byte*)(this->start + 0xAC + 0x04 + 0x01);
		this->valueFloat = (float*)(this->start + 0xAC + 0x04);
		this->valueInt = (uint32_t*)(this->start + 0xAC + 0x04);
		this->valueType = (byte*)(this->start + 0xA8);
	};

	void setTweakableFloat(float value)
	{
		if (*this->valueType == 2)
		{
			*this->valueFloat = value;
			*this->valueModified = 1;
		}
	}

	void setTweakableInt(uint32_t value)
	{
		if (*this->valueType == 1)
		{
			*this->valueInt = value;
			*this->valueModified = 1;
		}
	}

	void setTweakableByte(byte value)
	{
		if (*this->valueType == 0)
		{
			*this->valueByte = value;
			*this->valueModified = 1;
		}
	}

	void setTweakableVec2(float value[2])
	{
		if (*this->valueType == 3)
		{
			this->valueFloat[0] = value[0];
			this->valueFloat[1] = value[1];

			*this->valueModified = 1;
		}
	}

	void setTweakableVec3(float value[3])
	{
		if (*this->valueType == 4)
		{
			this->valueFloat[0] = value[0];
			this->valueFloat[1] = value[1];
			this->valueFloat[2] = value[2];

			*this->valueModified = 1;
		}
	}

	void setTweakableVec4(float value[4])
	{
		if (*this->valueType == 5)
		{
			this->valueFloat[0] = value[0];
			this->valueFloat[1] = value[1];
			this->valueFloat[2] = value[2];
			this->valueFloat[3] = value[3];

			*this->valueModified = 1;
		}
	}
};


//this is stupid i'm sorry
Tweakable *SSAATweakablePtr = nullptr;
Tweakable *MotionBlurTweakablePtr = nullptr;
Tweakable *AbilityLevitateTweakablePtr = nullptr;
void ControlGameData::Tweakable_SetLevitateDisable(bool disableLevitation) {
	if (!AbilityLevitateTweakablePtr) return; //should never happen
	AbilityLevitateTweakablePtr->setTweakableByte((byte)disableLevitation);
}

void ControlGameData::Tweakable_SetTemporalSSAA(bool temporalSSAA) {
	if (!SSAATweakablePtr) return; //should never happen
	SSAATweakablePtr->setTweakableFloat(temporalSSAA ? 1.0f : 0.0f);
}

void ControlGameData::Tweakable_SetMotionBlur(bool motionBlur) {
	if (!MotionBlurTweakablePtr) return; //should never happen
	MotionBlurTweakablePtr->setTweakableFloat(motionBlur ? 0.4f : 0.0f);
}

#define MAX_TRIGGERS 4096 //1024 //8192
void ControlGameData::InitGameData()
{
	triggerPtrs.reserve(MAX_TRIGGERS); // enough?
	for (int i = 0; i < MAX_TRIGGERS; i++)
		triggerPtrs.push_back({});
	// actually instead we should allocate 1024 triggers that are set to null
	// then oncce it gets used it shouldn't be null anymore
	// if it gets unallocated we set it back to null so we only need to have a small if statement inside the render code
	// to check if a trigger ptr is null or not
	// idk how else to not get fucked by multithreading lmao

	uint64_t processStartAddr = reinterpret_cast<uint64_t>(GetModuleHandle(nullptr));
	uint64_t coregameDllAddr = reinterpret_cast<uint64_t>(GetModuleHandle(L"coregame_rmdwin7_f.dll"));
	uint64_t physicsDllAddr = reinterpret_cast<uint64_t>(GetModuleHandle(L"physics_rmdwin7_f.dll"));
	uint64_t renderDllAddr = reinterpret_cast<uint64_t>(GetModuleHandle(L"renderer_rmdwin7_f.dll"));
	uint64_t rlDllAddr = reinterpret_cast<uint64_t>(GetModuleHandle(L"rl_rmdwin7_f.dll"));
	ptrViewMatrixAddr = reinterpret_cast<ptr*>(renderDllAddr + 0x123B6A0);

	ptr* setPlayerFunctionAddr = reinterpret_cast<ptr*>(physicsDllAddr + 0x5d60);
	ptr* triggerComponentCtorFunctionAddr = reinterpret_cast<ptr*>(coregameDllAddr + 0x1c6110);
	ptr* triggerComponentDtorFunctionAddr = reinterpret_cast<ptr*>(coregameDllAddr + 0x1c62b0);
	ptr* enterTriggerFunctionAddr = reinterpret_cast<ptr*>(coregameDllAddr + 0x1c6db0);
	ptr* getWorldSpaceAABBFunctionAddr = reinterpret_cast<ptr*>(physicsDllAddr + 0x80510);
	ptr* startUnloadingLevelFunctionAddr = reinterpret_cast<ptr*>(coregameDllAddr + 0x3fe20);
	ptr* startLoadingLevelFunctionAddr = reinterpret_cast<ptr*>(coregameDllAddr + 0x19db90);
	ptr* notifyClientLevelLoadingCompleteFunctionAddr = reinterpret_cast<ptr*>(coregameDllAddr + 0x19e4b0);
	ptr* characterControllerCtorAddr = reinterpret_cast<ptr*>(physicsDllAddr + 0x5d80);
	ptr* characterControllerMoveCapsuleAddr = reinterpret_cast<ptr*>(physicsDllAddr + 0x7540);

	baseTweakablesGetTweakable = (baseTweakablesGetTweakable_t)reinterpret_cast<ptr*>(rlDllAddr + 0x1f8e60);
	baseTweakablesSetTweakable = (baseTweakablesSetTweakable_t)reinterpret_cast<ptr*>(rlDllAddr + 0x1f9200);

	//skip the intro epilepsy & autosave warnings
	Tweakable IntroScreenTweakable = Tweakable((ptr*)baseTweakablesGetTweakable("Intro Screen:Display time for section"));
	IntroScreenTweakable.setTweakableFloat(0.0f); //default 4.0f

	baseTweakablesSetTweakable((char*)"Intro Screen:Display time for section", (char*)"30.0", 1);

	//fix janky menu/loading screen background animation
	Tweakable CoherentGTLiveViewsTweakable = Tweakable((ptr*)baseTweakablesGetTweakable("CoherentGT:Force live views to update"));
	CoherentGTLiveViewsTweakable.setTweakableByte(1); //default 0
	/*
	//force vsync off in menus - ok this actually doesn't do anything ??
	Tweakable VSyncTweakable = Tweakable((ptr*)baseTweakablesGetTweakable("VSync:Enabled"));
	VSyncTweakable.setTweakableByte(0); //default 1
	*/
	//disable TAA
	static Tweakable SSAATweakable = Tweakable((ptr*)baseTweakablesGetTweakable("SSAA:Temporal Jitter Scale"));
	SSAATweakable.setTweakableFloat(0.0f); //default 1.0f
	SSAATweakablePtr = &SSAATweakable;

	//disable motionblur (should be optional?)
	static Tweakable MotionBlurTweakable = Tweakable((ptr*)baseTweakablesGetTweakable("VectorBlur:Shutter Speed"));
	//MotionBlurTweakable.setTweakableFloat(0.0f); //default 0.4f
	MotionBlurTweakablePtr = &MotionBlurTweakable;

	//disables levitation, useful for practice
	static Tweakable AbilityLevitateTweakable = Tweakable((ptr*)baseTweakablesGetTweakable("Ability Levitate: Disable"));
	//AbilityLevitateTweakable.setTweakableByte(1);
	AbilityLevitateTweakablePtr = &AbilityLevitateTweakable;

	if (MH_CreateHook(setPlayerFunctionAddr, &setAsPlayerCharacter, reinterpret_cast<LPVOID*>(&setAsPlayerOriginal)) != MH_OK) throw;
	if (MH_EnableHook(setPlayerFunctionAddr) != MH_OK) throw;

	if (MH_CreateHook(triggerComponentCtorFunctionAddr, &triggerComponentCtor, reinterpret_cast<LPVOID*>(&triggerComponentCtorOriginal)) != MH_OK) throw;
	if (MH_EnableHook(triggerComponentCtorFunctionAddr) != MH_OK) throw;

	if (MH_CreateHook(triggerComponentDtorFunctionAddr, &triggerComponentDtor, reinterpret_cast<LPVOID*>(&triggerComponentDtorOriginal)) != MH_OK) throw;
	if (MH_EnableHook(triggerComponentDtorFunctionAddr) != MH_OK) throw;

	if (MH_CreateHook(enterTriggerFunctionAddr, &enterTrigger, reinterpret_cast<LPVOID*>(&enterTriggerOriginal)) != MH_OK) throw;
	if (MH_EnableHook(enterTriggerFunctionAddr) != MH_OK) throw;

	if (MH_CreateHook(getWorldSpaceAABBFunctionAddr, &getWorldSpaceAABB, reinterpret_cast<LPVOID*>(&getWorldSpaceAABBOriginal)) != MH_OK) throw;
	if (MH_EnableHook(getWorldSpaceAABBFunctionAddr) != MH_OK) throw;

	if (MH_CreateHook(startUnloadingLevelFunctionAddr, &startUnloadingLevel, reinterpret_cast<LPVOID*>(&startUnloadingLevelOriginal)) != MH_OK) throw;
	if (MH_EnableHook(startUnloadingLevelFunctionAddr) != MH_OK) throw;

	if (MH_CreateHook(startLoadingLevelFunctionAddr, &startLoadingLevel, reinterpret_cast<LPVOID*>(&startLoadingLevelOriginal)) != MH_OK) throw;
	if (MH_EnableHook(startLoadingLevelFunctionAddr) != MH_OK) throw;

	if (MH_CreateHook(notifyClientLevelLoadingCompleteFunctionAddr, &notifyClientLevelLoadingComplete, reinterpret_cast<LPVOID*>(&notifyClientLevelLoadingCompleteOriginal)) != MH_OK) throw;
	if (MH_EnableHook(notifyClientLevelLoadingCompleteFunctionAddr) != MH_OK) throw;

	if (MH_CreateHook(characterControllerCtorAddr, &characterControllerCtor, reinterpret_cast<LPVOID*>(&characterControllerOriginal)) != MH_OK) throw;
	if (MH_EnableHook(characterControllerCtorAddr) != MH_OK) throw;

	if (MH_CreateHook(characterControllerMoveCapsuleAddr, &characterControllerMoveCapsule, reinterpret_cast<LPVOID*>(&characterControllerMoveCapsuleOriginal)) != MH_OK) throw;
	if (MH_EnableHook(characterControllerMoveCapsuleAddr) != MH_OK) throw;

	printf("process physics: 0x%llx\n", processStartAddr);
	printf("setplayer: 0x%llx\n", (uint64_t)setPlayerFunctionAddr);
	printf("triggerComponentCtor: 0x%llx\n", (uint64_t)triggerComponentCtorFunctionAddr);
	printf("entertrigger: 0x%llx\n", (uint64_t)enterTriggerFunctionAddr);
	printf("getWorldSpaceAABB: 0x%llx\n", (uint64_t)getWorldSpaceAABBFunctionAddr);
	printf("startUnloadingLevel: 0x%llx\n", (uint64_t)startUnloadingLevelFunctionAddr);
	printf("startLoadingLevel: 0x%llx\n", (uint64_t)startLoadingLevelFunctionAddr);
	printf("notifyClientLevelLoadingCompleteFunctionAddr: 0x%llx\n", (uint64_t)notifyClientLevelLoadingCompleteFunctionAddr);
	printf("characterControllerCtorAddr: 0x%llx\n", (uint64_t)characterControllerCtorAddr);
	printf("characterControllerMoveCapsuleAddr: 0x%llx\n", (uint64_t)characterControllerMoveCapsuleAddr);
}

Vector3 *ControlGameData::GetPlayerPos()
{ //this is stupid and is only being used so the UI doesn't crash things while we're fast traveling within a map
	float a[2], b[2];

	if (!playerController || !playerCharacterController || !mapIsLoaded)
		return nullptr;

	memcpy(a, playerController + 0x12, sizeof(uint64_t));
	memcpy(b, playerController + 0x13, sizeof(uint64_t));

	playerPos.x = a[0];
	playerPos.y = a[1];
	playerPos.z = b[0]; // b[1] is just padding

	return &playerPos;
}

Vector3* ControlGameData::GetPlayerPos_Real()
{ //this is causing crashes during fast travel
	if (!playerController || !playerCharacterController || !mapIsLoaded)
		return nullptr;

	ptr* physx3characterkinematic = *(ptr**)(playerCharacterController + 12);
	if (!physx3characterkinematic)
		return nullptr;

	float* x = (float*)(physx3characterkinematic + 63) + 1;
	float* y = (float*)(physx3characterkinematic + 63) + 3;
	float* z = (float*)(physx3characterkinematic + 63) + 5;

	playerPos.x = *x;
	playerPos.y = *y;
	playerPos.z = *z;

	return &playerPos;
}

void ControlGameData::SetPlayerPos(Vector3 newPos)
{
	if (!playerController || !playerCharacterController || !mapIsLoaded)
		return;

	ptr* physx3characterkinematic = *(ptr**)(playerCharacterController + 12);
	if (!physx3characterkinematic)
		return;

	float* x = (float*)(physx3characterkinematic + 63) + 1;
	float* y = (float*)(physx3characterkinematic + 63) + 3;
	float* z = (float*)(physx3characterkinematic + 63) + 5;

	*x = newPos.x;
	*y = newPos.y;
	*z = newPos.z;
}

Matrix4* ControlGameData::GetViewMatrix()
{
	viewMatrix = *reinterpret_cast<Matrix4*>(ptrViewMatrixAddr);
	return &viewMatrix;
}

std::vector<triggerPtr> ControlGameData::GetTriggers()
{
	for (int i = 0; i < triggerPtrs.size(); i++) {
		if (triggerPtrs.at(i).trigger == NULL)
			continue;

		uint64_t* gameobjectPtr = *triggerPtrs.at(i).gameobject;
		uint64_t* networkstatePtr = *triggerPtrs.at(i).networkstate;
		uint64_t* triggerPtr = triggerPtrs.at(i).trigger;
		uint64_t* triggerEntityPtr = triggerPtrs.at(i).genericentity;
		uint64_t triggerEntity = *triggerPtrs.at(i).genericentity;

		Point p;
		float tmpFloats[6];
		uint64_t* floatLoc = reinterpret_cast<uint64_t*>(&tmpFloats);

		if (gameobjectPtr != NULL) {
			uint32_t* onEnterLoc = reinterpret_cast<uint32_t*>(networkstatePtr + 9);
			float* posLocPtr = reinterpret_cast<float*>(gameobjectPtr + 2);
			float posLoc[3];
			posLoc[0] = posLocPtr[0];
			posLoc[1] = posLocPtr[1];
			posLoc[2] = posLocPtr[2];

			// trigger might not be finished constructed yet
			if (posLoc == NULL || onEnterLoc == NULL || triggerEntity == NULL)
				continue;

			Vector3 pos = { posLoc[0], posLoc[1], posLoc[2] };
			uint32_t* onEnterPtr = onEnterLoc;
			triggerObject tO = { triggerPtr, pos, onEnterLoc };

			memset(&tmpFloats, 0.0f, sizeof(float) * 6);
			getWorldSpaceAABB(triggerEntity, floatLoc);

			p.a.x = tmpFloats[0];
			p.a.y = tmpFloats[1];
			p.a.z = tmpFloats[2];

			p.b.x = tmpFloats[3];
			p.b.y = tmpFloats[4];
			p.b.z = tmpFloats[5];

			if (triggerPtrs.at(i).triggerobject.trigger == nullptr) {
				triggerPtrs.at(i).triggerobject = tO;
				triggerPtrs.at(i).triggerobject.minmax = p;
			}
		}
	}

	return triggerPtrs;
}

bool ControlGameData::GetMapIsLoaded()
{
	return mapIsLoaded;
}

int ControlGameData::getTriggerCount(void)
{
	return triggerCount;
}
