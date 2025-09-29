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

using characterControllerCtor_t = uint64_t* (__fastcall*)(uint64_t* x, uint64_t* y, uint64_t* z, int a4);
characterControllerCtor_t characterControllerCtorOriginal;

using characterControllerDtor_t = uint64_t* (__fastcall*)(uint64_t* x, uint64_t* y);
characterControllerDtor_t characterControllerDtorOriginal;

using characterControllerMoveRelative_t = void(__fastcall*)(uint64_t* x, float* y, float z, float w);
characterControllerMoveRelative_t characterControllerMoveCapsuleOriginal;

using isFreeCameraOn_t = bool (__fastcall*)(ptr** inputManagerInstance);
isFreeCameraOn_t isFreeCameraOn;

using setFreeCamera_t = void (__fastcall*)(ptr** inputManagerInstance, bool a2);
setFreeCamera_t setFreeCamera;

ptr** inputManagerInstance;
ptr* playerController;
bool mapIsLoaded = false;
bool doWeExist = false;

// maybe remove this function since we are already getting playerCharacterController from the character controller ctor hook
// we can then fetch entity components from the game's ecs
// playerControllerState is not used
// playerCharacter is used to get the physx component which we would get anyways with component system funcs
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
	PrintLootDropInformationFromPools();

	mapIsLoaded = true;
	notifyClientLevelLoadingCompleteOriginal(x, y);
}

uint64_t* __fastcall getWorldSpaceAABB(uint64_t x, uint64_t* y) {
	uint64_t* result = getWorldSpaceAABBOriginal(x, y);
	return result;
}

ptr* playerCharacterController = nullptr;
ptr* playerEntity = nullptr;

uint64_t* __fastcall characterControllerCtor(uint64_t* x, uint64_t* y, uint64_t* z, int a4) {
	if (a4 == 1)
	{
 		doWeExist = true;

		auto result = characterControllerCtorOriginal(x, y, z, a4);
		playerCharacterController = x;
		playerEntity = *(ptr**)(playerCharacterController + 1);

		return x;
	}

	characterControllerCtorOriginal(x, y, z, a4);
	return x;
}

void __fastcall characterControllerDtor(uint64_t* x, uint64_t* y) {

	if (x == playerCharacterController)
	{
		printf("we are gone\n");
		doWeExist = false;
		playerEntity = nullptr;
		playerCharacterController = nullptr;
	}

	characterControllerDtorOriginal(x,y);
}

// really hacky way of making the player teleport without the weird delay from the physx capsule velocity function
// proper way would be to do playerentity -> playerentitystate -> TransformComponentState -> TransformComponentState::teleport
// however we would need to know the simd transformation arguments and how to construct those to call the function
// so for now this is "ok"
bool isPlayerLoadingPosition = false;
bool callOriginalPhysxVelocity = true;
static uint64_t loadingPositionCounter = 0;

float speedOfPlayer[4];
void characterControllerMoveCapsule(uint64_t* x, float* y, float z, float w)
{
	if (isPlayerLoadingPosition)
	{
		using MovementModelTypeID_t = uint32_t(__fastcall*)();
		MovementModelTypeID_t MovementModelTypeID = (MovementModelTypeID_t)GetProcAddress(GetModuleHandle(L"coregame_rmdwin7_f.dll"), "?getTypeIDStatic@MovementModelComponentState@coregame@@SAIXZ");
		char* MovementModelState = (char*)GameObjectState_GetComponentByTypeId(*(uint64_t*)(playerEntity+1), MovementModelTypeID());
		uint32_t* MovementType = (uint32_t*)(char*)(MovementModelState+0x120);
		*MovementType = 0;
		loadingPositionCounter = 7;

		y[0] = 0.0f;
		y[1] = 0.0f;
		y[2] = 0.0f;

		isPlayerLoadingPosition = false;
		callOriginalPhysxVelocity = false;
	}

	if (loadingPositionCounter > 0)
	{
		loadingPositionCounter--;

		if (loadingPositionCounter == 0)
		{
			using MovementModelTypeID_t = uint32_t(__fastcall*)();
			MovementModelTypeID_t MovementModelTypeID = (MovementModelTypeID_t)GetProcAddress(GetModuleHandle(L"coregame_rmdwin7_f.dll"), "?getTypeIDStatic@MovementModelComponentState@coregame@@SAIXZ");
			char* MovementModelState = (char*)GameObjectState_GetComponentByTypeId(*(uint64_t*)(playerEntity + 1), MovementModelTypeID());
			uint32_t* MovementType = (uint32_t*)(char*)(MovementModelState + 0x120);
			*MovementType = 1;
			callOriginalPhysxVelocity = true;
		}

		y[0] = 0.0f;
		y[1] = 0.0f;
		y[2] = 0.0f;
	}

	speedOfPlayer[0] = *y;			// x
	speedOfPlayer[1] = *(y + 1);	// y
	speedOfPlayer[2] = *(y + 2);	// z
	speedOfPlayer[3] = sqrtf(speedOfPlayer[0] * speedOfPlayer[0] + speedOfPlayer[2] * speedOfPlayer[2]);

	characterControllerMoveCapsuleOriginal(x, y, z, w);
}

using PhysxCapsuleVelocityUpdate_t = bool(__fastcall*)(uint64_t a1, uint64_t a2);
PhysxCapsuleVelocityUpdate_t PhysxCapsuleVelocityUpdateOrig;
bool PhysxCapsuleVelocityUpdate(uint64_t a1, uint64_t a2)
{
	if (callOriginalPhysxVelocity)
		return PhysxCapsuleVelocityUpdateOrig(a1, a2);
	else
		return false;
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

// TODO: hack!!! rewrite this later once we figure out more about the gameclient instance
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

extern startupString* tmpString;
void ControlGameData::ToggleDeveloperMenus()
{ //scuffed, should do this another way
	if (!tmpString)
		return;

	tmpString->devmode = !tmpString->devmode;
	printf("%s developer menus\n", tmpString->devmode ? "enabled" : "disabled");
}
#if 0
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
#endif

void ControlGameData::UpdateStartupStringValues(ControlConfig *cfg) {
	// On unloading we crash here because we don't know where the startup string is in memory
	if (tmpString != nullptr)
		tmpString->alwaysInFocus = cfg->pauseOnFocusLost;
}

using setPresentInterval_t = void(__fastcall*)(void* RendererInterfaceInstance, uint64_t interval);
setPresentInterval_t setPresentIntervalFunc;
void setPresentInterval(void* RendererInterfaceInstance, uint64_t interval)
{
	setPresentIntervalFunc(RendererInterfaceInstance, 0); //force disable vsync
}

struct EncounterManager {
	void* vtable;
	char tmp[0xE0];
	uint32_t encounterType; // ?
	uint32_t _padding;
	float encounterTimer;
	uint32_t tmp2;
	uint32_t tmp3;
	uint32_t tmp4;
	uint32_t tmp5;
	uint32_t tmp6;
	uint32_t protectionType; // ?
	float protectionTimer;
};
EncounterManager* EncounterDirector = NULL;

using EncounterDirectorCtor_t  = uint64_t(__fastcall*)(EncounterManager* a1);
EncounterDirectorCtor_t EncounterDirectorCtorOrig;
uint64_t EncounterDirectorCtor(EncounterManager* a1)
{
	uint64_t result = EncounterDirectorCtorOrig(a1);
	printf("encount director %llx\n", result);
	printf("lootdropsingletoncomponentstate director %llx\n\n", result-0x100);

	EncounterDirector = (EncounterManager*)result;

	return result;
}

using EncounterDirectorDtor_t = uint64_t(__fastcall*)(EncounterManager* a1);
EncounterDirectorDtor_t EncounterDirectorDtorOrig;
uint64_t EncounterDirectorDtor(EncounterManager* a1)
{
	EncounterDirector = nullptr;

	printf("encount director dtor\n");
	return EncounterDirectorDtorOrig(a1);
}

using EncounterDirectorDecrementTimer_t = uint64_t(__fastcall*)(uint64_t a1);
EncounterDirectorDecrementTimer_t EncounterDirectorDecrementTimerOrig;
uint64_t EncounterDirectorDecrementTimer(uint64_t a1)
{
	cfg->encounterTimer = EncounterDirector->encounterTimer;
	cfg->encounterProtectionTimer = EncounterDirector->protectionTimer;

	if (cfg->forceEncounters)
	{
		cfg->disableEncounters = false;
		EncounterDirector->encounterTimer = 0.0f;
		EncounterDirector->protectionTimer = 0.0f;
	}

	if (cfg->disableEncounters)
	{
		EncounterDirector->encounterTimer = 30.0f;
		EncounterDirector->protectionTimer = 10.0f;
		return 0;
	}

	return EncounterDirectorDecrementTimerOrig(a1);
}

using scriptModify_t = uint64_t(__fastcall*)(uint64_t a1);
scriptModify_t scriptModifyOrig;
uint64_t scriptModify(uint64_t a1)
{
	char* rbp = (char*)_AddressOfReturnAddress();
	char* genericEntity = *(char**)(rbp+0x38);

	char* strContent = genericEntity+0x88;
	uint32_t* strSize_A = (uint32_t*)(genericEntity+0x90);
	uint32_t* strSize_B = (uint32_t*)(genericEntity+0x94);
	
	//memcpy(*(char**)strContent, "NULL\0", sizeof(char) * 5);
	//*strSize_A = 5;
	//*strSize_B = 5;

	/*
	for (int i = 0; i < *strSize_A; i++)
	{
		if (*(char*)(*(char**)strContent + i) == '\0')
			printf("\n");
		else
			putchar(*(char*)(*(char**)strContent+i));
	}

	printf("\n");
	*/

	return scriptModifyOrig(a1);
}

using printScript_t = void(__fastcall*)(__int64 a1, __int64 a2, const char* a3);
printScript_t printScriptOrig;
void printScript(__int64 a1, __int64 a2, const char* a3)
{
	// char* 0x48 -> 0x20, print with printf
	char* rbp = (char*)_AddressOfReturnAddress();
	char* scriptStr = *(char**)(rbp + 0x48);

	char* strContent = *(char**)(scriptStr + 0x20);
	printf("%s\n", strContent);

	printScriptOrig(a1,a2,a3);
}

printScript_t printScriptOrig2;
void printScript2(__int64 a1, __int64 a2, const char* a3)
{
	// char* 0x48 -> 0x20, print with printf
	char* rbp = (char*)_AddressOfReturnAddress();
	char* scriptStr = *(char**)(rbp + 0x48);

	char* strContent = *(char**)(scriptStr + 0x20);
	printf("%s\n", strContent);

	printScriptOrig2(a1, a2, a3);
}


/*

game allocates two GamePlayerCameraMovementComponentState
one gets used the other one doesn't for some reason?
the GamePlayerCameraMovementComponent doesn't contain a ptr reference to any of the states
GamePlayerCameraMovementComponentState ctor callstack no consistent difference
GamePlayerCameraMovementComponentState is part of an entity (camera entity?)
the camera entity is part of some attachment entity thing
to actually find the GamePlayerCameraMovementComponentState we hook the function that writes the yawpitchroll floats to the state
	wish we could just traverse the parent child entity hierarchy somehow but oh well

*/

uint64_t* GamePlayerCameraMovementPtr = nullptr;

using GamePlayerCameraMovement_t = void(__fastcall*)(uint64_t* a1);
GamePlayerCameraMovement_t GamePlayerCameraMovementComponentStateDtorOrig;
void GamePlayerCameraMovementComponentStateDtor(uint64_t* a1)
{
	if (a1 == GamePlayerCameraMovementPtr)
		GamePlayerCameraMovementPtr = nullptr;

	GamePlayerCameraMovementComponentStateDtorOrig(a1);
}

using WriteInputDataToCamera_t = void(__fastcall*)(uint64_t* a1);
WriteInputDataToCamera_t WriteInputDataToCameraOrig;
void WriteInputDataToCamera(uint64_t* a1)
{
	GamePlayerCameraMovementPtr = a1;
	WriteInputDataToCameraOrig(a1);
}

void CameraMovementHooks()
{
	uint64_t processStartAddr = reinterpret_cast<uint64_t>(GetModuleHandle(nullptr));

	char* CameraMovementStateDtorAddr = (char*)processStartAddr + 0x387090;
	char* WriteInputDataToCameraAddr = (char*)processStartAddr + 0x3872F0;

	if (MH_CreateHook(CameraMovementStateDtorAddr, &GamePlayerCameraMovementComponentStateDtor, reinterpret_cast<LPVOID*>(&GamePlayerCameraMovementComponentStateDtorOrig)) != MH_OK) throw;
	if (MH_EnableHook(CameraMovementStateDtorAddr) != MH_OK) throw;

	if (MH_CreateHook(WriteInputDataToCameraAddr, &WriteInputDataToCamera, reinterpret_cast<LPVOID*>(&WriteInputDataToCameraOrig)) != MH_OK) throw;
	if (MH_EnableHook(WriteInputDataToCameraAddr) != MH_OK) throw;
}

void ScriptReplace()
{
	uint64_t processStartAddr = reinterpret_cast<uint64_t>(GetModuleHandle(nullptr));

	char* hookAddr = (char*)processStartAddr + 0x5187A0;
	if (MH_CreateHook(hookAddr, &scriptModify, reinterpret_cast<LPVOID*>(&scriptModifyOrig)) != MH_OK) throw;
	if (MH_EnableHook(hookAddr) != MH_OK) throw;

	char* hookAddr2 = (char*)processStartAddr + 0x236160;
	if (MH_CreateHook(hookAddr2, &printScript, reinterpret_cast<LPVOID*>(&printScriptOrig)) != MH_OK) throw;
	if (MH_EnableHook(hookAddr2) != MH_OK) throw;

	char* hookAddr3 = (char*)processStartAddr + 0x526DF0;
	if (MH_CreateHook(hookAddr3, &printScript2, reinterpret_cast<LPVOID*>(&printScriptOrig2)) != MH_OK) throw;
	if (MH_EnableHook(hookAddr3) != MH_OK) throw;
}

// by forcing early ret on the update function we can manually send inputs to the game
using InputX86Update_t = void(__fastcall*)(void* inputx86);
InputX86Update_t InputX86UpdateOrig;
void InputX86Update(void* inputx86)
{
	if (cfg == nullptr)
	{
		InputX86UpdateOrig(inputx86);
		return;
	}

	if (cfg->ui->data->uiToggle)
	{
		char* mouseMoveHorizontalPtr = (char*)inputx86+0x624;
		char* mouseMoveVerticalPtr = (char*)inputx86+0x628;

		char* moveForwardPtr = (char*)inputx86 + 0xB8;
		char* moveBackwardsPtr = (char*)inputx86 + 0xB0;
		char* moveLeftPtr = (char*)inputx86 + 0x90;
		char* moveRightPtr = (char*)inputx86 + 0x8C;

		uint32_t* mouseMoveX = (uint32_t*)mouseMoveHorizontalPtr;
		uint32_t* mouseMoveY = (uint32_t*)mouseMoveVerticalPtr;

		uint32_t* moveForward = (uint32_t*)moveForwardPtr;
		uint32_t* moveBackwards = (uint32_t*)moveBackwardsPtr;
		uint32_t* moveLeft = (uint32_t*)moveLeftPtr;
		uint32_t* moveRight = (uint32_t*)moveRightPtr;

		*mouseMoveX = 0;
		*mouseMoveY = 0;
		
		*moveForward = 0;
		*moveBackwards = 0;
		*moveLeft = 0;
		*moveRight = 0;

		return;
	}
	else
		InputX86UpdateOrig(inputx86);
}

using CursorHide_t = void(__fastcall*)(__int64 a1, unsigned __int8 a2);
CursorHide_t CursorHideInGameOrig;
void CursorHideInGame(__int64 a1, unsigned __int8 a2)
{
	if (cfg == nullptr)
	{
		CursorHideInGameOrig(a1, a2);
		return;
	}

	if (cfg->ui->data->uiToggle)
		CursorHideInGameOrig(a1, true);
	else
		CursorHideInGameOrig(a1, a2);

}

using CursorLockCenter_t = BOOL(__fastcall*)(__int64 a1, float* a2);
CursorLockCenter_t CursorLockCenterOrig;
BOOL CursorLockCenter(__int64 a1, float* a2)
{
	if (cfg == nullptr)
		return CursorLockCenterOrig(a1, a2);

	if (cfg->ui->data->uiToggle)
		return 0;
	else
		return CursorLockCenterOrig(a1, a2);
}

void CursorHooks()
{
	HMODULE appModule = GetModuleHandle(L"app_rmdwin7_f.dll");
	HMODULE inputModule = GetModuleHandle(L"input_rmdwin7_f.dll");

	char* cursorHideInGameAddr = (char*)appModule + 0x67c0;
	char* cursorLockCenterAddr = (char*)appModule + 0x66f0;
	char* inputX86UpdateAddr = (char*)inputModule + 0xA380;

	if (MH_CreateHook(cursorHideInGameAddr, &CursorHideInGame, reinterpret_cast<LPVOID*>(&CursorHideInGameOrig)) != MH_OK) throw;
	if (MH_EnableHook(cursorHideInGameAddr) != MH_OK) throw;

	if (MH_CreateHook(cursorLockCenterAddr, &CursorLockCenter, reinterpret_cast<LPVOID*>(&CursorLockCenterOrig)) != MH_OK) throw;
	if (MH_EnableHook(cursorLockCenterAddr) != MH_OK) throw;

	if (MH_CreateHook(inputX86UpdateAddr, &InputX86Update, reinterpret_cast<LPVOID*>(&InputX86UpdateOrig)) != MH_OK) throw;
	if (MH_EnableHook(inputX86UpdateAddr) != MH_OK) throw;
}

GlobalIDMap_GetPointer_t GlobalIDMap_GetPointer = nullptr;

void ControlGameData::InitGameData()
{
	HMODULE coregameModule = GetModuleHandle(L"coregame_rmdwin7_f.dll");
	HMODULE inputModule = GetModuleHandle(L"input_rmdwin7_f.dll");
	HMODULE physicsModule = GetModuleHandle(L"physics_rmdwin7_f.dll");
	HMODULE rendererModule = GetModuleHandle(L"renderer_rmdwin7_f.dll");
	HMODULE physxModule = GetModuleHandle(L"physx3characterkinematic_x64.dll");
	HMODULE rlModule = GetModuleHandle(L"rl_rmdwin7_f.dll");

	uint64_t processStartAddr = reinterpret_cast<uint64_t>(GetModuleHandle(nullptr));
	uint64_t coregameDllAddr = reinterpret_cast<uint64_t>(coregameModule);
	uint64_t physicsDllAddr = reinterpret_cast<uint64_t>(physicsModule);
	uint64_t renderDllAddr = reinterpret_cast<uint64_t>(rendererModule);
	uint64_t physxModuleAddr = reinterpret_cast<uint64_t>(physxModule);
	uint64_t rlDllAddr = reinterpret_cast<uint64_t>(rlModule);
	ptrViewMatrixAddr = reinterpret_cast<ptr*>(renderDllAddr + 0x123B6A0);

	ptr* setPlayerFunctionAddr = reinterpret_cast<ptr*>(physicsDllAddr + 0x5d60);
	ptr* getWorldSpaceAABBFunctionAddr = reinterpret_cast<ptr*>(physicsDllAddr + 0x80510);
	ptr* startUnloadingLevelFunctionAddr = reinterpret_cast<ptr*>(coregameDllAddr + 0x3fe20);
	ptr* startLoadingLevelFunctionAddr = reinterpret_cast<ptr*>(coregameDllAddr + 0x19db90);
	ptr* notifyClientLevelLoadingCompleteFunctionAddr = reinterpret_cast<ptr*>(coregameDllAddr + 0x19e4b0);
	ptr* characterControllerCtorAddr = reinterpret_cast<ptr*>(physicsDllAddr + 0x5d80);
	ptr* characterControllerDtorAddr = reinterpret_cast<ptr*>(physicsDllAddr + 0x5e70);
	ptr* characterControllerMoveCapsuleAddr = reinterpret_cast<ptr*>(physicsDllAddr + 0x7540);
	ptr* physxCapsuleVelocityUpdateAddr = reinterpret_cast<ptr*>(physxModuleAddr + 0x15C20);

	GlobalIDMap_GetPointer = reinterpret_cast<GlobalIDMap_GetPointer_t>(processStartAddr + 0x85f80);

#if 0 //not early enough
	//patch out single instance check
	ptr* singleInstanceAddr = reinterpret_cast<ptr*>(processStartAddr + 0x30aa08);
	if ( (byte)*singleInstanceAddr == 0x75)
		*singleInstanceAddr = 0xEB; //change JNZ to JMP (75->EB)
#endif

	ShapeEngine::InstallHooks(L"renderer_rmdwin7_f.dll");
	BaseTweakableInstallHooks(L"rl_rmdwin7_f.dll");
	BaseTweakableInitialize();
	TriggerInstallHooks(coregameDllAddr);
	TriggerInitialize();
	initLootDropHooks(processStartAddr, rlModule, coregameDllAddr);

	ptr* setPresentIntervalPtr = (ptr*)GetProcAddress(rendererModule, "?setPresentInterval@RendererInterfaceWrapper@rend@@QEAAXH@Z");
	if (MH_CreateHook(setPresentIntervalPtr, &setPresentInterval, reinterpret_cast<LPVOID*>(&setPresentIntervalFunc)) != MH_OK) throw;
	if (MH_EnableHook(setPresentIntervalPtr) != MH_OK) throw;

	inputManagerInstance = (ptr**)GetProcAddress(inputModule, "?sm_pInstance@InputManager@input@@0PEAV12@EA");
	ptr* readDigitalPtr = (ptr*)GetProcAddress(inputModule, "?readDigital@InputManager@input@@QEAA_NH_N@Z");
	ptr* isFreeCameraOnPtr = (ptr*)GetProcAddress(inputModule, "?isFreeCameraWithoutPlayerControlsOn@InputManager@input@@QEAA_NXZ");
	ptr* setFreeCameraPtr = (ptr*)GetProcAddress(inputModule, "?setFreeCameraWithoutPlayerControls@InputManager@input@@QEAAX_N@Z");

	isFreeCameraOn = reinterpret_cast<isFreeCameraOn_t>(isFreeCameraOnPtr);
	setFreeCamera = reinterpret_cast<setFreeCamera_t>(setFreeCameraPtr);

	char* encounterDirectorCtorAddr = (char*)processStartAddr + 0x296460;
	if (MH_CreateHook(encounterDirectorCtorAddr, &EncounterDirectorCtor, reinterpret_cast<LPVOID*>(&EncounterDirectorCtorOrig)) != MH_OK) throw;
	if (MH_EnableHook(encounterDirectorCtorAddr) != MH_OK) throw;

	char* encounterDirectorDtorAddr = (char*)processStartAddr + 0x296720;
	if (MH_CreateHook(encounterDirectorDtorAddr, &EncounterDirectorDtor, reinterpret_cast<LPVOID*>(&EncounterDirectorDtorOrig)) != MH_OK) throw;
	if (MH_EnableHook(encounterDirectorDtorAddr) != MH_OK) throw;

	char* EncounterDirectorDecrementTimerAddr = (char*)processStartAddr + 0x298510;
	if (MH_CreateHook(EncounterDirectorDecrementTimerAddr, &EncounterDirectorDecrementTimer, reinterpret_cast<LPVOID*>(&EncounterDirectorDecrementTimerOrig)) != MH_OK) throw;
	if (MH_EnableHook(EncounterDirectorDecrementTimerAddr) != MH_OK) throw;

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

	if (MH_CreateHook(characterControllerCtorAddr, &characterControllerCtor, reinterpret_cast<LPVOID*>(&characterControllerCtorOriginal)) != MH_OK) throw;
	if (MH_EnableHook(characterControllerCtorAddr) != MH_OK) throw;

	if (MH_CreateHook(characterControllerDtorAddr, &characterControllerDtor, reinterpret_cast<LPVOID*>(&characterControllerDtorOriginal)) != MH_OK) throw;
	if (MH_EnableHook(characterControllerDtorAddr) != MH_OK) throw;

	if (MH_CreateHook(characterControllerMoveCapsuleAddr, &characterControllerMoveCapsule, reinterpret_cast<LPVOID*>(&characterControllerMoveCapsuleOriginal)) != MH_OK) throw;
	if (MH_EnableHook(characterControllerMoveCapsuleAddr) != MH_OK) throw;

	if (MH_CreateHook(physxCapsuleVelocityUpdateAddr, &PhysxCapsuleVelocityUpdate, reinterpret_cast<LPVOID*>(&PhysxCapsuleVelocityUpdateOrig)) != MH_OK) throw;
	if (MH_EnableHook(physxCapsuleVelocityUpdateAddr) != MH_OK) throw;

	ScriptReplace();
	CursorHooks();
	CameraMovementHooks();

	printf("process physics: 0x%llx\n", (uint64_t)physicsDllAddr);
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
{
	if (!playerController || !playerCharacterController || !mapIsLoaded || !doWeExist)
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
		printf("playerPos: 0x%llx\n", (uint64_t)physx3characterkinematic + 63);
		printOnlyOnce = true;
	}

	return &playerPos;
}

void ControlGameData::SetPlayerPos(Vector3 newPos)
{
	if (!playerController || !playerCharacterController || !mapIsLoaded || !doWeExist)
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

typedef void(WINAPI* setViewToWorldF_t)(void* rendView, Matrix4 viewmatrix);
void ControlGameData::SetPitchYawRoll(Vector3 rotation)
{
	if (!playerEntity)
		return;

	uint64_t processStartAddr = reinterpret_cast<uint64_t>(GetModuleHandle(nullptr));
	using CameraMovement_GetTypeIDStatic_t = uint32_t(__fastcall*)();
	CameraMovement_GetTypeIDStatic_t CameraMovement_GetTypeIDStatic = (CameraMovement_GetTypeIDStatic_t)((char*)processStartAddr + 0x3868C0);
	uint32_t CameraMovement_TypeID = CameraMovement_GetTypeIDStatic();
	char* CameraMovementState = (char*)GameObjectState_GetComponentByTypeId((uint64_t)playerEntity, CameraMovement_TypeID);

	printf("calling\n");

	//GameObjectState_GetComponentByTypeId()

//	GamePlayerCameraMovementComponentState
//	0xB8 pitch roll yaw


/*
	HMODULE rendModule = GetModuleHandle(L"renderer_rmdwin7_f.dll");

	void* rendView = (char*)rendModule + 0x1234730;

	setViewToWorldF_t setViewToWorldF = (setViewToWorldF_t)GetProcAddress(GetModuleHandle(L"renderer_rmdwin7_f.dll"), "?setViewToWorldF@View@rend@@QEAAXAEBV?$Matrix4x3Template@M@m@@@Z");
	setViewToWorldF(rendView, viewMatrix);
*/
}

Matrix4* ControlGameData::GetViewMatrix()
{
	viewMatrix = *reinterpret_cast<Matrix4*>(ptrViewMatrixAddr);
	return &viewMatrix;
}