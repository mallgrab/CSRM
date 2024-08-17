#include "LootDrop.h"
#include <libloaderapi.h>

using GlobalIDMap_GetPointer_t = uint64_t * (__fastcall*)(uint64_t* a1, uint64_t* a2);
GlobalIDMap_GetPointer_t GlobalIDMap_GetPointer;

using RelativeValueModComponentState_GetTypeIDStatic_t = uint32_t(__fastcall*)();
RelativeValueModComponentState_GetTypeIDStatic_t RelativeValueModComponentState_GetTypeIDStatic;

using ItemDescriptionComponentState_GetTypeIDStatic_t = uint32_t(__fastcall*)();
ItemDescriptionComponentState_GetTypeIDStatic_t ItemDescriptionComponentState_GetTypeIDStatic;

using ItemStackComponentState_GetTypeIDStatic_t = uint32_t(__fastcall*)();
ItemStackComponentState_GetTypeIDStatic_t ItemStackComponentState_GetTypeIDStatic;

using ModDescriptionComponentState_GetTypeIDStatic_t = uint32_t(__fastcall*)();
ModDescriptionComponentState_GetTypeIDStatic_t ModDescriptionComponentState_GetTypeIDStatic;

using GameObjectState_GetComponentByTypeId_t = uint64_t(__fastcall*)(uint64_t Component, uint32_t TypeIDStatic);
GameObjectState_GetComponentByTypeId_t GameObjectState_GetComponentByTypeId;

uint64_t* sm_pInstance = nullptr;
bool lootdropSingletonExists = false;
bool modifyLootDropsCheat = false;

//uint64_t currentObjectiveHash = 0xDEADA55;
uint64_t currentObjectiveHash = 0x0;
uint64_t currentMissionHash = 0x0;

std::vector<lootDropRestore_t> lootDropRestore = {};

uint64_t objectiveMissionHashes[] = {
	0x10469758bd9f0051,
	0x367a9559d4a9c051,
	0x363675f1bcf28051,
	0x9acfc98a58b8050,
	0x2ca177693eb94051,
	0x31689a1f87650051,
	0x2507179ee174c051,
	0x13e456183f2a4051,
	0x1c7478e81fa9c051,
	0x2202525bea888051,
	0x3bd1d7e0cbaf4051,
	0x37dfc5865122c051,
	0x12f8db6bbd0a0051,
	0x3996910a8aad4051,
};

/*
	LOOT_ITEM_MOD_COMMON_DEATH_FEEDER
	energy renewal		Death Feeder				0x5b57da04e4e0054
	
	LOOT_ITEM_MOD_COMMON_MENTAL_FOCUS
	energy recovery		Mental Focus				0xf72884f35910054
	
		objective hash 10469758bd9f0051
		objective hash 367a9559d4a9c051

	LOOT_ITEM_RESOURCE_COMMON_INSTRUCTIVE_PATTERN_TITLE
	12 intrusive patterns							0x33257ce200bbc054

	LOOT_ITEM_RESOURCE_UNCOMMON_ASTRAL_BLIP_TITLE
	4 astral blips									0x2c6b227fef64054

	LOOT_ITEM_MOD_UNCOMMON_LIGHT_FOOT
	tier 2 dodge efficiency 12%		light foot		0x30dfea3a43960054

	LOOT_ITEM_MOD_UNCOMMON_DAMAGE
	tier 2 damage boost								0x251d4ccb1aaf8054

		objective hash 2507179ee174c051
		objective hash 13e456183f2a4051
		objective hash 1c7478e81fa9c051
		objective hash 2202525bea888051
		objective hash 3bd1d7e0cbaf4051
		objective hash 37dfc5865122c051
		objective hash 12f8db6bbd0a0051
		objective hash 3996910a8aad4051
*/


dropManipulation modDropList[] = {
	//{0xDEADA55, 0x5b57da04e4e0054, 0, "LOOT_ITEM_MOD_COMMON_DEATH_FEEDER", 0.0f, false, false, false, false},
	//{0xDEADA55, 0x5b57da04e4e0054, 0, "LOOT_ITEM_MOD_COMMON_DEATH_FEEDER", 0.0f, false, false, false, false},

	{0x10469758bd9f0051, 0x5b57da04e4e0054, 0, "LOOT_ITEM_MOD_COMMON_DEATH_FEEDER", 0.0f, false, false, false, false},
	{0x10469758bd9f0051, 0xf72884f35910054, 0, "LOOT_ITEM_MOD_COMMON_MENTAL_FOCUS", 0.0f, false, false, false, false},

	{0x2507179ee174c051, 0x33257ce200bbc054, 12, "LOOT_ITEM_RESOURCE_COMMON_INSTRUCTIVE_PATTERN_TITLE", 0.0f, false, false, false, false},
	{0x2507179ee174c051, 0x2c6b227fef64054, 6, "LOOT_ITEM_RESOURCE_UNCOMMON_ASTRAL_BLIP_TITLE", 0.0f, false, false, false, false},
	{0x2507179ee174c051, 0x30dfea3a43960054, 0, "LOOT_ITEM_MOD_UNCOMMON_LIGHT_FOOT", 12.0f, true, false, false, false},
	{0x2507179ee174c051, 0x251d4ccb1aaf8054, 0, "LOOT_ITEM_MOD_UNCOMMON_DAMAGE", 0.0f, false, false, false, false},
};

// TODO: make this not work on boxes
void modifyLootDrops(currentDropTable* a2)
{
	if (!lootdropSingletonExists)
		return;

	bool skipModDropModification = false;

	size_t modDropListSize = sizeof(modDropList) / sizeof(dropManipulation);
	
	// check if we are already waiting for a drop modification to finish
	for (int i = 0; i < modDropListSize; i++)
		if (modDropList[i].isActivated)
			skipModDropModification = true;

	for (int i = 0; i < modDropListSize; i++)
	{
		bool skipThisDropListModification = false;

		if (modDropList[i].objectiveHash != currentObjectiveHash)
			continue;

		if (modDropList[i].isCollected)
			continue;

		if (!(a2->possibleLootDropMaxCounter >= 2))
			continue;

		for (int i = 0; i < lootDropRestore.size(); i++)
			if ((uint64_t)lootDropRestore[i].indexPtr == (uint64_t)a2->lootDropArray)
				skipThisDropListModification = true;

		if (skipThisDropListModification)
			continue;

		// don't activate more drop modifications we are waiting for one to finish
		if (!modDropList[i].isActivated && skipModDropModification)
			continue;

		// set both to false once we dropped and gathered the mod/material
		modDropList[i].isActivated = true;

		// backup index array from the lootdrop table we are about to modify
		uint32_t* backupIndexArray = (uint32_t*)malloc(sizeof(uint32_t) * a2->lootDropArrayMaxCounter);
		memcpy(backupIndexArray, a2->lootDropArray, sizeof(uint32_t) * a2->lootDropArrayMaxCounter);

		// store so we can restore the lootdrop table later
		lootDropRestore.push_back({ 
			backupIndexArray, 
			a2->lootDropArray, 
			a2->lootDropArrayMaxCounter, 
			(uint64_t*)((char*)a2->possibleLootDropGlobalIDs + (2 * LOOT_TABLE_GAP)), 
			*(uint64_t*)(char*)(a2->possibleLootDropGlobalIDs + (2 * LOOT_TABLE_GAP))
		});

		// modify globalID drop table
		char* globalIDFromDropTable = (char*)a2->possibleLootDropGlobalIDs + (2 * LOOT_TABLE_GAP);
		memcpy(globalIDFromDropTable, &modDropList[i].objectID, sizeof(uint64_t));
		//memcpy(globalIDFromDropTable, &cfg->lootTableItemGlobalIDs[cfg->lootTableIndex], sizeof(uint64_t));

		// TODO: modify lootdrop index table, subtract backwards and add our index every 2nd, 3rd time
		for (int v=0; v < a2->lootDropArrayMaxCounter; v++)
			a2->lootDropArray[v] = 2;

		printf("done\n");
	}
}

void restoreLootDropIndexArrayPointers()
{
	if (!lootdropSingletonExists)
		return;

	// TODO: restore lootdrop index back to normal
	for (int i = 0; i < lootDropRestore.size(); i++)
	{
		memcpy(lootDropRestore[i].indexPtr, lootDropRestore[i].indexArray, sizeof(uint32_t) * lootDropRestore[i].indexMaxCount);
		memcpy(lootDropRestore[i].lootDropPtr, &lootDropRestore[i].lootDropGlobalID, sizeof(uint64_t));
	}

	lootDropRestore.clear();

	printf("cleaned up the lootdrop tables\n");
}

void checkPlayerInventory()
{
	uint64_t processStartAddr = (uint64_t)(GetModuleHandle(nullptr));
	Inventory* gameInventoryComponent = (Inventory*) * (char**)(*(char**)(0x1111108 + processStartAddr + 0x8) + 0x38);

	if (gameInventoryComponent == nullptr)
		return;

	for (int i = 0; i < gameInventoryComponent->currentAmount; i++)
	{
		uint64_t inventoryAddressLocation = gameInventoryComponent->inventory[i * 2];
		uint64_t* ptr = GlobalIDMap_GetPointer(*(uint64_t**)sm_pInstance, &inventoryAddressLocation);

		uint32_t itemDescriptionTypeID = ItemDescriptionComponentState_GetTypeIDStatic();
		ItemDescription* itemDescription = (ItemDescription*)GameObjectState_GetComponentByTypeId((uint64_t)ptr, itemDescriptionTypeID);

		uint32_t modDescriptionTypeID = ModDescriptionComponentState_GetTypeIDStatic();
		ModDescription* modDescription = (ModDescription*)GameObjectState_GetComponentByTypeId((uint64_t)ptr, modDescriptionTypeID);
		
		// TODO: do the same thing for materials here, we have to keep track of the count tho
		if (itemDescription != nullptr)
		{
			//printf("%s\n", itemDescription->component->itemName);

			size_t modDropListSize = sizeof(modDropList) / sizeof(dropManipulation);

			for (int v = 0; v < modDropListSize; v++)
			{
				if (!modDropList[v].isActivated)
					continue;

				if (strstr(modDropList[v].dropName, itemDescription->component->itemName) == nullptr)
					continue;

				// its a mod we don't want to do material stuff with it
				if (modDescription != nullptr)
					continue;

				uint32_t itemStackTypeID = ItemStackComponentState_GetTypeIDStatic();
				uint64_t itemStackAddr = GameObjectState_GetComponentByTypeId((uint64_t)ptr, itemStackTypeID);
				uint64_t itemCount = *(uint64_t*)(char*)(itemStackAddr+0x40);

				if (itemCount >= modDropList[v].materialCounter)
				{
					modDropList[v].isActivated = false;
					modDropList[v].isCollected = true;

					printf("collected the item\n");
					restoreLootDropIndexArrayPointers();
				}
			}
		}

		if (modDescription != nullptr && modDescription->component->description != nullptr)
		{
			//printf("%s\n", modDescription->component->description->modName);

			size_t modDropListSize = sizeof(modDropList) / sizeof(dropManipulation);

			for (int v = 0; v < modDropListSize; v++)
			{
				if (!modDropList[v].isActivated)
					continue;

				if (strstr(modDropList[v].dropName, modDescription->component->description->modName) == nullptr)
					continue;

				modDropList[v].isActivated = false;
				modDropList[v].isCollected = true;

				printf("collected the item\n");
				restoreLootDropIndexArrayPointers();
			}
		}
	}
}

using decrementDropTableCounter_ReturnOffsetCounter_t = uint64_t(__fastcall*)(uint64_t a1, currentDropTable* a2);
decrementDropTableCounter_ReturnOffsetCounter_t decrementDropTableCounter_ReturnOffsetCounterOrig;
uint64_t decrementDropTableCounter_ReturnOffsetCounter(uint64_t a1, currentDropTable* a2)
{
	if (!lootdropSingletonExists)
		decrementDropTableCounter_ReturnOffsetCounterOrig(a1, a2);

	if (!modifyLootDropsCheat)
		return decrementDropTableCounter_ReturnOffsetCounterOrig(a1, a2);

	// some sort of name list is being generated, don't do any modifications
	if (a2->possibleLootDropMaxCounter > 40)
		return decrementDropTableCounter_ReturnOffsetCounterOrig(a1, a2);

	checkPlayerInventory();

	uint64_t result = decrementDropTableCounter_ReturnOffsetCounterOrig(a1, a2);
	
	modifyLootDrops(a2);

	return result;
}

// required for us to fill up our globalid lootdrop table
using DataPoolCtor_t = uint64_t(__fastcall*)(uint64_t a1);
DataPoolCtor_t DataPoolCtorOrig;
uint64_t DataPoolCtor(uint64_t a1)
{
	uint64_t result = DataPoolCtorOrig(a1);

	// clear so we get new datapool, highly unlikely that it will happen but it could
	if (cfg->lootTableItemNames.size() != 0)
		cfg->lootTableItemNames.clear();

	uint64_t processStartAddr = (uint64_t)(GetModuleHandle(nullptr));

	char* globalIDDataPool = *(char**)(processStartAddr + 0x01166FC0);
	char* globalIDItems = *(char**)(globalIDDataPool + 0x540); // found with CE ptr finder

	uint32_t amountOfGlobalIDItems = *(uint32_t*)(char*)(globalIDDataPool + 0x548);
	uint64_t* globalIDItem = (uint64_t*)globalIDItems;

	for (int i = 0; i < amountOfGlobalIDItems; i++)
	{
		uint64_t* ptr = GlobalIDMap_GetPointer(*(uint64_t**)sm_pInstance, (uint64_t*)globalIDItem);
		ptr++; // the second ptr contains the string
		cfg->lootTableItemNames.push_back(*(char**)ptr);

		cfg->lootTableItemGlobalIDs.push_back(*(uint64_t*)globalIDItem);
		globalIDItem++;
	}

	printf("finished building loottable\n");

	return result;
}

using completeObjective_t = uint64_t(__fastcall*)(uint64_t a1, uint64_t a2, uint64_t a3);
completeObjective_t completeObjectiveOrig;
uint64_t completeObjective(uint64_t a1, uint64_t a2, uint64_t a3)
{
	static bool derefNextStackPtr = false;

	if (derefNextStackPtr && a3 != 1 && a3 != 4)
	{
		uint64_t objectiveHash = *(uint64_t*)a3;

		if (objectiveHash > IGNORE_MAX_INT)
		{
			uint64_t* globalIDPtr = GlobalIDMap_GetPointer(*(uint64_t**)sm_pInstance, (uint64_t*)a3);
			if (globalIDPtr != nullptr)
			{
				char* stringPtr = *(char**)((char*)globalIDPtr + 0x10);

				// don't include side objectives
				if (strstr(stringPtr, "BA_") == nullptr)
				{
					currentObjectiveHash = objectiveHash;

					//printf("objective %s\n", stringPtr);
					//printf("objective hash %llx\n", objectiveHash);
				}
			}
		}

		derefNextStackPtr = false;
	}

	if (a3 == 1)
		derefNextStackPtr = true;

	uint64_t result = completeObjectiveOrig(a1, a2, a3);
	return result;
}

using completeMission_t = uint64_t(__fastcall*)(uint64_t a1, uint64_t a2, uint8_t a3);
completeMission_t completeMissionOrig;
uint64_t completeMission(uint64_t a1, uint64_t a2, uint8_t a3)
{
	uint64_t result = completeMissionOrig(a1, a2, a3);
	uint64_t hash = *(uint64_t*)result;

	if (hash != NULL && hash > IGNORE_MAX_INT)
	{
		//printf("mission hash %llx\n", hash);

		currentMissionHash = hash;

		// start of a new game, restore the lootdrop table
		if (currentMissionHash == 0x80c6da414868051)
		{
			size_t modDropListSize = sizeof(modDropList) / sizeof(dropManipulation);

			// check if we are already waiting for a drop modification to finish
			for (int i = 0; i < modDropListSize; i++)
			{
				modDropList[i].isActivated = false;
				modDropList[i].isCollected = false;
			}
		}
	}

	return result;
}

using LootDropSingletonCtor_t = uint64_t(__fastcall*)(uint64_t a1);
LootDropSingletonCtor_t LootDropSingletonCtorOrig;
uint64_t LootDropSingletonCtor(uint64_t a1)
{
	uint64_t result = LootDropSingletonCtorOrig(a1);
	lootdropSingletonExists = true;

	return result;
}

using EntityComponentStateDtor_t = char*(__fastcall*)(char* block, char a2);
EntityComponentStateDtor_t EntityComponentStateDtorOrig;
char* EntityComponentStateDtor(char* block, char a2)
{
	lootdropSingletonExists = false;
	
	char* result = EntityComponentStateDtorOrig(block, a2);
	return result;
}

using createItemDrop_t = GenericEntityState*(__fastcall*)(__int64 a1, uint64_t* a2, float a3, __int64 a4);
createItemDrop_t createItemDropOrig;
GenericEntityState* createItemDrop(__int64 a1, uint64_t* a2, float a3, __int64 a4)
{
	GenericEntityState* result = createItemDropOrig(a1,a2,a3,a4);
	//printf("lootdrop entity %llx\n", result);

	if (!lootdropSingletonExists)
		return result;

	if (result != nullptr)
	{
		uint32_t relativeValueTypeID = RelativeValueModComponentState_GetTypeIDStatic();
		uint64_t relativeValueAddr = GameObjectState_GetComponentByTypeId((uint64_t)result, relativeValueTypeID);

		size_t modDropListSize = sizeof(modDropList) / sizeof(dropManipulation);

		for (int i = 0; i < modDropListSize; i++)
		{
			if (modDropList[i].isActivated && modDropList[i].modifyAttribute)
			{
				if (relativeValueAddr != NULL)
				{
					float* relativeValue = (float*)(relativeValueAddr + 0x40);
					*relativeValue = modDropList[i].attributeRating;
				}
			}
		}
	}

	return result;
}

void initLootDropHooks(uint64_t processStartAddr, HMODULE rlModule, uint64_t coregameDllAddr)
{
	GlobalIDMap_GetPointer = (GlobalIDMap_GetPointer_t)GetProcAddress(rlModule, "?getPointer@GlobalIDMap@r@@QEBAPEAXAEBVGlobalID@2@@Z");
	sm_pInstance = (uint64_t*)GetProcAddress(rlModule, "?sm_pInstance@GlobalIDMap@r@@0PEAV12@EA");

	GameObjectState_GetComponentByTypeId = (GameObjectState_GetComponentByTypeId_t)GetProcAddress(rlModule, "?getComponentByTypeId@GameObjectState@r@@QEBAPEAVComponentStateBase@2@I@Z");
	RelativeValueModComponentState_GetTypeIDStatic = (RelativeValueModComponentState_GetTypeIDStatic_t)((char*)processStartAddr + 0x507240);
	ItemDescriptionComponentState_GetTypeIDStatic = (ItemDescriptionComponentState_GetTypeIDStatic_t)((char*)processStartAddr + 0x177D90);
	ModDescriptionComponentState_GetTypeIDStatic = (ItemDescriptionComponentState_GetTypeIDStatic_t)((char*)processStartAddr + 0x1AF200);
	ItemStackComponentState_GetTypeIDStatic = (ItemStackComponentState_GetTypeIDStatic_t)((char*)processStartAddr + 0x14D410);

	char* createItemDropAddr = (char*)processStartAddr + 0x34D980;
	if (MH_CreateHook(createItemDropAddr, &createItemDrop, reinterpret_cast<LPVOID*>(&createItemDropOrig)) != MH_OK) throw;
	if (MH_EnableHook(createItemDropAddr) != MH_OK) throw;

	char* EntityComponentStateDtorAddr = (char*)processStartAddr + 0x3DEFF0;
	if (MH_CreateHook(EntityComponentStateDtorAddr, &EntityComponentStateDtor, reinterpret_cast<LPVOID*>(&EntityComponentStateDtorOrig)) != MH_OK) throw;
	if (MH_EnableHook(EntityComponentStateDtorAddr) != MH_OK) throw;

	char* lootDropSingletonCtorAddr = (char*)processStartAddr + 0x3DEEC0;
	if (MH_CreateHook(lootDropSingletonCtorAddr, &LootDropSingletonCtor, reinterpret_cast<LPVOID*>(&LootDropSingletonCtorOrig)) != MH_OK) throw;
	if (MH_EnableHook(lootDropSingletonCtorAddr) != MH_OK) throw;

	char* dataPoolCtorAddr = (char*)processStartAddr + 0x48D4A0;
	if (MH_CreateHook(dataPoolCtorAddr, &DataPoolCtor, reinterpret_cast<LPVOID*>(&DataPoolCtorOrig)) != MH_OK) throw;
	if (MH_EnableHook(dataPoolCtorAddr) != MH_OK) throw;

	char* decrementDropTableCounter_ReturnOffsetCounterAddr = (char*)processStartAddr + 0x29B610;
	if (MH_CreateHook(decrementDropTableCounter_ReturnOffsetCounterAddr, &decrementDropTableCounter_ReturnOffsetCounter, reinterpret_cast<LPVOID*>(&decrementDropTableCounter_ReturnOffsetCounterOrig)) != MH_OK) throw;
	if (MH_EnableHook(decrementDropTableCounter_ReturnOffsetCounterAddr) != MH_OK) throw;

	char* completeObjectiveAddr = (char*)processStartAddr + 0x3F1AF0;
	if (MH_CreateHook(completeObjectiveAddr, &completeObjective, reinterpret_cast<LPVOID*>(&completeObjectiveOrig)) != MH_OK) throw;
	if (MH_EnableHook(completeObjectiveAddr) != MH_OK) throw;

	char* completeMissionAddr = (char*)processStartAddr + 0x3F0F50;
	if (MH_CreateHook(completeMissionAddr, &completeMission, reinterpret_cast<LPVOID*>(&completeMissionOrig)) != MH_OK) throw;
	if (MH_EnableHook(completeMissionAddr) != MH_OK) throw;
}