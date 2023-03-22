#include "GameData.h"
#include <libloaderapi.h>

using setAsPlayerOriginal_t = void(__stdcall*)(uint64_t* x, char y);
setAsPlayerOriginal_t setAsPlayerOriginal;

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

ptr* playerController;
bool mapIsLoaded;
bool inputs[64] = {};

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

// hack!!! rewrite this later once we figure out more about the gameclient instance
// theres a function that lets us set the camera mode directly but requires hooking
// the gameclient constructor since there isnt any exported function that gives us the instance
using readDigital_t = bool(__fastcall*)(ptr** inputManagerInstance, int64_t num);
readDigital_t readDigitalFunc;
bool freecamFlying = false;
bool setFreeCam = false;
int toggleCameraModeTwice = 0;

bool readDigitalHook(ptr** inputManagerInstance, int64_t num)
{
	bool origReturn = readDigitalFunc(inputManagerInstance, num);

	// 44 or 45
	if (setFreeCam && !freecamFlying && (num == 44))
	{
		freecamFlying = true;
		return true;
	}

	if (!setFreeCam && freecamFlying && (num == 44))
	{
		freecamFlying = false;
		toggleCameraModeTwice = 10;
		return false;
	}

	if (toggleCameraModeTwice && (num == 44))
	{
		toggleCameraModeTwice--;

		if (toggleCameraModeTwice % 2)
			return false;
		else
			return true;
	}

	return origReturn;
}

void ControlGameData::ToggleFreeCam()
{
	if (!setFreeCam)
		setFreeCam = true;
	else
		setFreeCam = false;
}

void ControlGameData::InitGameData()
{
	uint64_t processStartAddr = reinterpret_cast<uint64_t>(GetModuleHandle(nullptr));
	uint64_t coregameDllAddr = reinterpret_cast<uint64_t>(GetModuleHandle(L"coregame_rmdwin7_f.dll"));
	uint64_t physicsDllAddr = reinterpret_cast<uint64_t>(GetModuleHandle(L"physics_rmdwin7_f.dll"));
	uint64_t renderDllAddr = reinterpret_cast<uint64_t>(GetModuleHandle(L"renderer_rmdwin7_f.dll"));
	uint64_t rlDllAddr = reinterpret_cast<uint64_t>(GetModuleHandle(L"rl_rmdwin7_f.dll"));
	ptrViewMatrixAddr = reinterpret_cast<ptr*>(renderDllAddr + 0x123B6A0);

	ptr* setPlayerFunctionAddr = reinterpret_cast<ptr*>(physicsDllAddr + 0x5d60);
	ptr* getWorldSpaceAABBFunctionAddr = reinterpret_cast<ptr*>(physicsDllAddr + 0x80510);
	ptr* startUnloadingLevelFunctionAddr = reinterpret_cast<ptr*>(coregameDllAddr + 0x3fe20);
	ptr* startLoadingLevelFunctionAddr = reinterpret_cast<ptr*>(coregameDllAddr + 0x19db90);
	ptr* notifyClientLevelLoadingCompleteFunctionAddr = reinterpret_cast<ptr*>(coregameDllAddr + 0x19e4b0);
	ptr* characterControllerCtorAddr = reinterpret_cast<ptr*>(physicsDllAddr + 0x5d80);
	ptr* characterControllerMoveCapsuleAddr = reinterpret_cast<ptr*>(physicsDllAddr + 0x7540);

	BaseTweakableInstallHooks(rlDllAddr);
	BaseTweakableInitialize();
	TriggerInstallHooks(coregameDllAddr);
	TriggerInitialize();

	ptr* readDigital = (ptr*)GetProcAddress(GetModuleHandle(L"input_rmdwin7_f.dll"), "?readDigital@InputManager@input@@QEAA_NH_N@Z");
	//readDigitalFunc = reinterpret_cast<readDigital_t>(readDigital);

	if (MH_CreateHook(readDigital, &readDigitalHook, reinterpret_cast<LPVOID*>(&readDigitalFunc)) != MH_OK) throw;
	if (MH_EnableHook(readDigital) != MH_OK) throw;

	if (MH_CreateHook(setPlayerFunctionAddr, &setAsPlayerCharacter, reinterpret_cast<LPVOID*>(&setAsPlayerOriginal)) != MH_OK) throw;
	if (MH_EnableHook(setPlayerFunctionAddr) != MH_OK) throw;

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

	bool static printOnlyOnce = false;

	if (!printOnlyOnce)
	{
		printf("playerPos: 0x%llx\n", (ptr*)&playerPos.x);
		printOnlyOnce = true;
	}

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

using isLoadingOn_t = int64_t(__fastcall*)(ptr** inputManagerInstance);
using setDebugButtons_t = void(__fastcall*)(ptr** inputManagerInstance, bool a2);
using setDebug_t = void(__fastcall*)(ptr** inputManagerInstance, bool a2);
using setFreeCamera_t = void(__fastcall*)(ptr** inputManagerInstance, bool a2);

Matrix4* ControlGameData::GetViewMatrix()
{
	ptr** inputManagerInstance = (ptr**)GetProcAddress(GetModuleHandle(L"input_rmdwin7_f.dll"), "?sm_pInstance@InputManager@input@@0PEAV12@EA");
	ptr* isFreeCameraOn = (ptr*)GetProcAddress(GetModuleHandle(L"input_rmdwin7_f.dll"), "?isFreeCameraWithoutPlayerControlsOn@InputManager@input@@QEAA_NXZ");
	ptr* setFreeCamera = (ptr*)GetProcAddress(GetModuleHandle(L"input_rmdwin7_f.dll"), "?setFreeCameraWithoutPlayerControls@InputManager@input@@QEAAX_N@Z");
	ptr* setDebugButtons = (ptr*)GetProcAddress(GetModuleHandle(L"input_rmdwin7_f.dll"), "?setDebugButtons@InputManager@input@@QEAAX_N@Z");
	ptr* setDebug = (ptr*)GetProcAddress(GetModuleHandle(L"input_rmdwin7_f.dll"), "?setDebug@InputManager@input@@QEAAX_N@Z");


	setFreeCamera_t setFreeCameraFunc = reinterpret_cast<setFreeCamera_t>(setFreeCamera);
	setFreeCameraFunc(inputManagerInstance, true);

	isLoadingOn_t isFreeCameraOnFunc = reinterpret_cast<isLoadingOn_t>(isFreeCameraOn);
	int64_t result = isFreeCameraOnFunc(inputManagerInstance);

	setDebugButtons_t setDebugButtonsFunc = reinterpret_cast<setDebugButtons_t>(setDebugButtons);
	setDebugButtonsFunc(inputManagerInstance, true);

	setDebug_t setDebugFunc = reinterpret_cast<setDebugButtons_t>(setDebug);
	setDebugFunc(inputManagerInstance, true);

	viewMatrix = *reinterpret_cast<Matrix4*>(ptrViewMatrixAddr);
	return &viewMatrix;
}

bool ControlGameData::GetMapIsLoaded()
{
	return mapIsLoaded;
}