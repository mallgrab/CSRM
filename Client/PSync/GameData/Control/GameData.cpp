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

using isFreeCameraOn_t = bool (__fastcall*)(ptr** inputManagerInstance);
isFreeCameraOn_t isFreeCameraOn;

using setFreeCamera_t = void (__fastcall*)(ptr** inputManagerInstance, bool a2);
setFreeCamera_t setFreeCamera;

ptr** inputManagerInstance;
ptr* playerController;
bool mapIsLoaded;

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
	startUnloadingLevelOriginal(ptr);
}

void startLoadingLevel(uint64_t x, uint64_t y, uint64_t z, int64_t a, char b, uint8_t c) {
	printf("start loading level\n");

	mapIsLoaded = false;
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
#ifdef _DEBUG
	printf("character controller constructor, 0x%llx, 0x%llx, 0x%llx, %d\n", x, y, z, a4);
#endif

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

bool ControlGameData::playerIsOnGround()
{
	if (!playerController || !playerCharacterController || !mapIsLoaded)
		return false;

	bool *onGround = (bool*)(playerController + 21) + 6;
	return *onGround;
}

// hack!!! rewrite this later once we figure out more about the gameclient instance
// theres a function that lets us set the camera mode directly but requires hooking
// the gameclient constructor since there isnt any exported function that gives us the instance
using readDigital_t = bool(__fastcall*)(ptr** inputManagerInstance, int64_t num);
readDigital_t readDigitalFunc;

bool toggleFreeCam = false;

bool readDigitalHook(ptr** inputManagerInstance, int64_t num)
{
	bool origReturn = readDigitalFunc(inputManagerInstance, num);

	if (toggleFreeCam && (num == 44))
	{
		if (!isFreeCameraOn(inputManagerInstance))
		{
			setFreeCamera(inputManagerInstance, true);
			toggleFreeCam = false;
			return true;
		}
	}
			
	if (toggleFreeCam && (num == 46 || num == 47))
	{
		toggleFreeCam = false;
		return true;
	}

	return origReturn;
}

void ControlGameData::ToggleFreeCam()
{
	toggleFreeCam = true;
}

void ControlGameData::EnableDeveloperMenus()
{ //scuffed, should do this another way
	/* To restore the Mission select screen (Basically enables developer mode I guess?)
	patch 1404F8C8				cmp		byte ptr [rax+130h], 0
	just change it to mov ,1 instead. And then maybe skip the jump after that. */
	//Control_DX11.exe+40F8C8
	uint64_t processStartAddr = reinterpret_cast<uint64_t>(GetModuleHandle(nullptr));
	byte *devModeAddr = reinterpret_cast<byte*>(processStartAddr + 0x40f8c8);
	DWORD  oldProtect;
	if (!VirtualProtect(devModeAddr, sizeof(uint64_t*), PAGE_EXECUTE_READWRITE, &oldProtect))
		throw("we died\n");

	//cmp byte ptr[rax+130h], 0 -> mov byte ptr[rax+130h], 1
	//80 B8 30 01 00 00 00 -> C6 80 30 01 00 00 01
	*devModeAddr = 0xC6; devModeAddr++;
	*devModeAddr = 0x80; devModeAddr++;
	*devModeAddr = 0x30; devModeAddr++;
	*devModeAddr = 0x01; devModeAddr++;
	*devModeAddr = 0x00; devModeAddr++;
	*devModeAddr = 0x00; devModeAddr++;
	*devModeAddr = 0x01;

	/*
	//doesn't seem to do anything
	//JZ 0x14040FD23 -> NOP NOP NOP
	//0F 84 4E 04 00 00 -> 66 90 66 90 66 90
	*devModeAddr = 0x66; devModeAddr++;
	*devModeAddr = 0x90; devModeAddr++;
	*devModeAddr = 0x66; devModeAddr++;
	*devModeAddr = 0x90; devModeAddr++;
	*devModeAddr = 0x66; devModeAddr++;
	*devModeAddr = 0x90; devModeAddr++;
	*/
}

using setPresentInterval_t = void(__fastcall*)(void* RendererInterfaceInstance, uint64_t interval);
setPresentInterval_t setPresentIntervalFunc;
void setPresentInterval(void* RendererInterfaceInstance, uint64_t interval)
{
	interval = 0; //force disable vsync
	setPresentIntervalFunc(RendererInterfaceInstance, interval);
}

void ControlGameData::InitGameData()
{
	HMODULE coregameModule = GetModuleHandle(L"coregame_rmdwin7_f.dll");
	HMODULE inputModule = GetModuleHandle(L"input_rmdwin7_f.dll");
	HMODULE physicsModule = GetModuleHandle(L"physics_rmdwin7_f.dll");
	HMODULE rendererModule = GetModuleHandle(L"renderer_rmdwin7_f.dll");
	HMODULE rlModule = GetModuleHandle(L"rl_rmdwin7_f.dll");

	uint64_t processStartAddr = reinterpret_cast<uint64_t>(GetModuleHandle(nullptr));
	uint64_t coregameDllAddr = reinterpret_cast<uint64_t>(coregameModule);
	uint64_t physicsDllAddr = reinterpret_cast<uint64_t>(physicsModule);
	uint64_t renderDllAddr = reinterpret_cast<uint64_t>(rendererModule);
	uint64_t rlDllAddr = reinterpret_cast<uint64_t>(rlModule);
	ptrViewMatrixAddr = reinterpret_cast<ptr*>(renderDllAddr + 0x123B6A0);

	ptr* setPlayerFunctionAddr = reinterpret_cast<ptr*>(physicsDllAddr + 0x5d60);
	ptr* getWorldSpaceAABBFunctionAddr = reinterpret_cast<ptr*>(physicsDllAddr + 0x80510);
	ptr* startUnloadingLevelFunctionAddr = reinterpret_cast<ptr*>(coregameDllAddr + 0x3fe20);
	ptr* startLoadingLevelFunctionAddr = reinterpret_cast<ptr*>(coregameDllAddr + 0x19db90);
	ptr* notifyClientLevelLoadingCompleteFunctionAddr = reinterpret_cast<ptr*>(coregameDllAddr + 0x19e4b0);
	ptr* characterControllerCtorAddr = reinterpret_cast<ptr*>(physicsDllAddr + 0x5d80);
	ptr* characterControllerMoveCapsuleAddr = reinterpret_cast<ptr*>(physicsDllAddr + 0x7540);

#if 0 //not early enough
	//patch out single instance check
	ptr* singleInstanceAddr = reinterpret_cast<ptr*>(processStartAddr + 0x30aa08);
	if ( (byte)*singleInstanceAddr == 0x75)
		*singleInstanceAddr = 0xEB; //change JNZ to JMP (75->EB)
#endif

#ifdef _DEBUG //dev moed ?
	EnableDeveloperMenus();
#endif

	ShapeEngine::InstallHooks(L"renderer_rmdwin7_f.dll");
	BaseTweakableInstallHooks(L"rl_rmdwin7_f.dll");
	BaseTweakableInitialize();
	TriggerInstallHooks(coregameDllAddr);
	TriggerInitialize();

	ptr* setPresentIntervalPtr = (ptr*)GetProcAddress(rendererModule, "?setPresentInterval@RendererInterfaceWrapper@rend@@QEAAXH@Z");
	if (MH_CreateHook(setPresentIntervalPtr, &setPresentInterval, reinterpret_cast<LPVOID*>(&setPresentIntervalFunc)) != MH_OK) throw;
	if (MH_EnableHook(setPresentIntervalPtr) != MH_OK) throw;

	inputManagerInstance = (ptr**)GetProcAddress(inputModule, "?sm_pInstance@InputManager@input@@0PEAV12@EA");
	ptr* readDigitalPtr = (ptr*)GetProcAddress(inputModule, "?readDigital@InputManager@input@@QEAA_NH_N@Z");
	ptr* isFreeCameraOnPtr = (ptr*)GetProcAddress(inputModule, "?isFreeCameraWithoutPlayerControlsOn@InputManager@input@@QEAA_NXZ");
	ptr* setFreeCameraPtr = (ptr*)GetProcAddress(inputModule, "?setFreeCameraWithoutPlayerControls@InputManager@input@@QEAAX_N@Z");

	isFreeCameraOn = reinterpret_cast<isFreeCameraOn_t>(isFreeCameraOnPtr);
	setFreeCamera = reinterpret_cast<setFreeCamera_t>(setFreeCameraPtr);

	if (MH_CreateHook(readDigitalPtr, &readDigitalHook, reinterpret_cast<LPVOID*>(&readDigitalFunc)) != MH_OK) throw;
	if (MH_EnableHook(readDigitalPtr) != MH_OK) throw;

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

Matrix4* ControlGameData::GetViewMatrix()
{
	viewMatrix = *reinterpret_cast<Matrix4*>(ptrViewMatrixAddr);
	return &viewMatrix;
}

bool ControlGameData::GetMapIsLoaded()
{
	return mapIsLoaded;
}