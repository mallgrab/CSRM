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

// TODO: clean up after ourselves we dont need all this junk
struct GenericEntityState
{
	uint64_t* vtable;
	char tmp[0x60];
	uint64_t* componentsPtr;
	uint32_t amountOfComponents;
};

using ItemComponentState_GetTypeIDStatic_t = uint32_t(__fastcall*)();
ItemComponentState_GetTypeIDStatic_t ItemComponentState_GetTypeIDStatic;
using ItemDescriptionComponentState_GetTypeIDStatic_t = uint32_t(__fastcall*)();
ItemDescriptionComponentState_GetTypeIDStatic_t ItemDescriptionComponentState_GetTypeIDStatic;
using AddToInventoryComponentState_GetTypeIDStatic_t = uint32_t(__fastcall*)();
AddToInventoryComponentState_GetTypeIDStatic_t AddToInventoryComponentState_GetTypeIDStatic;
using LootDropItemComponentState_GetTypeIDStatic_t = uint32_t(__fastcall*)();
LootDropItemComponentState_GetTypeIDStatic_t LootDropItemComponentState_GetTypeIDStatic;
using ItemStackComponentState_GetTypeIDStatic_t = uint32_t(__fastcall*)();
ItemStackComponentState_GetTypeIDStatic_t ItemStackComponentState_GetTypeIDStatic;

using GameObjectState_GetComponentByTypeId_t = uint64_t(__fastcall*)(uint64_t Component, uint32_t TypeIDStatic);
GameObjectState_GetComponentByTypeId_t GameObjectState_GetComponentByTypeId;

using DynamicEntitySpawnerSpawnAt_t = GenericEntityState *(__fastcall*)(__int64* a1, __int64* a2, __int64 a3, __int64 a4);
DynamicEntitySpawnerSpawnAt_t DynamicEntitySpawnerSpawnAtOrig;
GenericEntityState* DynamicEntitySpawnerSpawnAt(__int64* a1, __int64* a2, __int64 a3, __int64 a4)
{
	GenericEntityState* result = DynamicEntitySpawnerSpawnAtOrig(a1, a2, a3, a4);

	uint32_t itemComponentTypeID = ItemComponentState_GetTypeIDStatic();
	uint64_t itemComponent = GameObjectState_GetComponentByTypeId((uint64_t)result, itemComponentTypeID);

	uint32_t itemDescriptionComponentTypeID = ItemDescriptionComponentState_GetTypeIDStatic();
	uint64_t itemDescriptionComponent = GameObjectState_GetComponentByTypeId((uint64_t)result, itemDescriptionComponentTypeID);

	uint32_t addToInventoryComponentTypeID = AddToInventoryComponentState_GetTypeIDStatic();
	uint64_t addToInventoryComponent = GameObjectState_GetComponentByTypeId((uint64_t)result, addToInventoryComponentTypeID);

	uint32_t lootDropItemComponentTypeID = LootDropItemComponentState_GetTypeIDStatic();
	uint64_t lootDropItemComponent = GameObjectState_GetComponentByTypeId((uint64_t)result, lootDropItemComponentTypeID);

	uint32_t itemStackComponentTypeID = ItemStackComponentState_GetTypeIDStatic();
	uint64_t itemStackComponent = GameObjectState_GetComponentByTypeId((uint64_t)result, itemStackComponentTypeID);
	
	uint64_t processStartAddr = (uint64_t)(GetModuleHandle(nullptr));
	uint64_t gameInventoryComponent = (uint64_t)*(char**)(*(char**)(0x1111108 + processStartAddr + 0x8) + 0x38);
	uint64_t inventory = (uint64_t)*(char**)(gameInventoryComponent + 0x40);
	uint32_t inventoryAmountOfItems = (uint32_t)*(char*)(gameInventoryComponent + 0x48);
	uint64_t lastInventoryGlobalID = (uint64_t)*(char**)((uint64_t*)inventory + inventoryAmountOfItems * 2);

	// TODO: Figure out what impacts the type

	/*
		itemArray = *(GameInventoryComponentState + 0x40);// items array
		endOfItemArray = &itemArray[2 * *(GameInventoryComponentState + 0x48)];// amount of items
	
		itemGlobalID = *itemArray;
		ItemEntityFromGlobalID = GetItemEntityFromGlobalID(&itemGlobalID);
		
		TypeIDStatic = LootDropItemComponentState::getTypeIDStatic();
		LootDropItemComponentState = r::GameObjectState::getComponentByTypeId(ItemEntityFromGlobalID, TypeIDStatic);
	*/

	/*
		LootDropSingletonTypeID = LootDropSingletonComponentState::getTypeIDStatic();
		charactermanager?
		r::GameObjectState::getComponentByTypeId
	*/

	if (itemDescriptionComponent != NULL)
		printf("item description %s\n", *(char**)(*(char**)(itemDescriptionComponent + 0x38) + 0x10));

	if (itemStackComponent != NULL)
	{
		uint64_t* counter = (uint64_t*)(char*)(itemStackComponent + 0x40);
		*counter = -10000;
	}

	return result;
}

/*
	a2 + 0x28 contains a counter that gets decremented, its used to go down the current droptable
	a2 + 0x2c contains the max counter from the current droptable

	with the counter it will pull a number out of the array at a1 + 0x18 (dereferenced)
	that number gets returned

	it will then pull out a globalid from the array at a2 + 0x38
	which it then uses to get a pointer to with r::GlobalIDMap::getPointer

	swapping that globalid out with the one from the globalid item list will result in us getting different items

	material drops only require one globalid call
	weapon/character mods need 3, each call is a different globalid (iirc)

	TODO: get a ptr or something to the globalid item list
*/ 

struct currentDropTable
{
	char tmp1[0x28];
	uint32_t counter;
	uint32_t maxCounter;
	char tmp2[0x8];
	uint64_t* globalIDPtr;
	uint32_t globalIDCounter; // uncertain... could be something else
	uint32_t globalIDMaxCounter;
};

using GlobalIDMap_GetPointer_t = uint64_t*(__fastcall*)(uint64_t* a1, uint64_t* a2);
GlobalIDMap_GetPointer_t GlobalIDMap_GetPointer;
uint64_t* sm_pInstance = nullptr;

using decrementDropTableCounter_ReturnOffsetCounter_t = uint64_t(__fastcall*)(uint64_t a1, currentDropTable* a2);
decrementDropTableCounter_ReturnOffsetCounter_t decrementDropTableCounter_ReturnOffsetCounterOrig;
uint64_t decrementDropTableCounter_ReturnOffsetCounter(uint64_t a1, currentDropTable* a2)
{
	uint64_t processStartAddr = (uint64_t)(GetModuleHandle(nullptr));
	char* globalIDDataPool = *(char**)(processStartAddr + 0x01166FC0);
	char* globalIDItems = *(char**)(globalIDDataPool + 0x540); // found with CE ptr finder
	
	uint32_t amountOfGlobalIDItems = *(uint32_t*)(char*)(globalIDDataPool + 0x548);
	uint64_t* globalIDItem = (uint64_t*)globalIDItems;

	std::vector<uint64_t> items;
	for (int i = 0; i < amountOfGlobalIDItems; i++)
	{
		items.push_back(*(uint64_t*)globalIDItem);
		uint64_t* ptr = GlobalIDMap_GetPointer(*(uint64_t**)sm_pInstance, (uint64_t*)globalIDItem);

		globalIDItem++;
	}

	uint64_t result = decrementDropTableCounter_ReturnOffsetCounterOrig(a1,a2);
	printf("material result %llx\n", result);
	
	char* globalIDFromDropTable = (char*)a2->globalIDPtr + (result * 0x18);
	uint64_t* ptr = GlobalIDMap_GetPointer(*(uint64_t**)sm_pInstance, (uint64_t*)globalIDFromDropTable);
	ptr++;
	if (strstr(*(char**)ptr, "_Nothing") != nullptr)
		memcpy(globalIDFromDropTable, &items[26], sizeof(uint64_t));

	// overwrite drop with what we want
	// TODO: find a way to restore it after the drop
	// memcpy(globalIDFromDropTable, &items[272], sizeof(uint64_t));
	
	return result;
}

// for later when we want to know what the globalid arrays contain for items
/*
	r::GlobalIDMap::getPointer(r::GlobalIDMap::sm_pInstance, a1, &v4));

	?getPointer@GlobalIDMap@r@@QEBAPEAXAEBVGlobalID@2@AEAI@Z	rl_rmdwin7_f
	?getPointer@GlobalIDMap@r@@QEBAPEAXAEBVGlobalID@2@@Z		rl_rmdwin7_f
	?sm_pInstance@GlobalIDMap@r@@0PEAV12@EA						rl_rmdwin7_f

*/

using EncounterDirectorCtor_t  = uint64_t(__fastcall*)(uint64_t a1);
EncounterDirectorCtor_t EncounterDirectorCtorOrig;
uint64_t EncounterDirectorCtor(uint64_t a1)
{
	uint64_t result = EncounterDirectorCtorOrig(a1);
	printf("encount director %llx\n", result);

	return result;
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

	GameObjectState_GetComponentByTypeId = (GameObjectState_GetComponentByTypeId_t)GetProcAddress(rlModule, "?getComponentByTypeId@GameObjectState@r@@QEBAPEAVComponentStateBase@2@I@Z");
	GlobalIDMap_GetPointer = (GlobalIDMap_GetPointer_t)GetProcAddress(rlModule, "?getPointer@GlobalIDMap@r@@QEBAPEAXAEBVGlobalID@2@@Z");
	sm_pInstance = (uint64_t*)GetProcAddress(rlModule, "?sm_pInstance@GlobalIDMap@r@@0PEAV12@EA");
	
	ItemComponentState_GetTypeIDStatic = (ItemComponentState_GetTypeIDStatic_t)GetProcAddress(coregameModule, "?getTypeIDStatic@ItemComponentState@coregame@@SAIXZ");
	ItemDescriptionComponentState_GetTypeIDStatic = (ItemDescriptionComponentState_GetTypeIDStatic_t)(char*)(processStartAddr + 0x177D90);
	AddToInventoryComponentState_GetTypeIDStatic = (AddToInventoryComponentState_GetTypeIDStatic_t)(char*)(processStartAddr + 0xF6BC0);
	LootDropItemComponentState_GetTypeIDStatic = (LootDropItemComponentState_GetTypeIDStatic_t)(char*)(processStartAddr + 0x3DCD00);
	ItemStackComponentState_GetTypeIDStatic = (LootDropItemComponentState_GetTypeIDStatic_t)(char*)(processStartAddr + 0x14D410);

	char* DynamicEntitySpawnerSpawnAtAddr = (char*)coregameDllAddr+0x771F0;
	if (MH_CreateHook(DynamicEntitySpawnerSpawnAtAddr, &DynamicEntitySpawnerSpawnAt, reinterpret_cast<LPVOID*>(&DynamicEntitySpawnerSpawnAtOrig)) != MH_OK) throw;
	if (MH_EnableHook(DynamicEntitySpawnerSpawnAtAddr) != MH_OK) throw;

	char* decrementDropTableCounter_ReturnOffsetCounterAddr = (char*)processStartAddr + 0x29B610;
	if (MH_CreateHook(decrementDropTableCounter_ReturnOffsetCounterAddr, &decrementDropTableCounter_ReturnOffsetCounter, reinterpret_cast<LPVOID*>(&decrementDropTableCounter_ReturnOffsetCounterOrig)) != MH_OK) throw;
	if (MH_EnableHook(decrementDropTableCounter_ReturnOffsetCounterAddr) != MH_OK) throw;

	char* EncounterDirectorCtorAddr = (char*)processStartAddr + 0x296460;
	if (MH_CreateHook(EncounterDirectorCtorAddr, &EncounterDirectorCtor, reinterpret_cast<LPVOID*>(&EncounterDirectorCtorOrig)) != MH_OK) throw;
	if (MH_EnableHook(EncounterDirectorCtorAddr) != MH_OK) throw;

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
		printf("playerPos: 0x%llx\n", (uint64_t)physx3characterkinematic + 63);
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