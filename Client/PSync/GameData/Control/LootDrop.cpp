#include "LootDrop.h"

RelativeValueModComponentState_GetTypeIDStatic_t RelativeValueModComponentState_GetTypeIDStatic;
ItemDescriptionComponentState_GetTypeIDStatic_t ItemDescriptionComponentState_GetTypeIDStatic;
ItemStackComponentState_GetTypeIDStatic_t ItemStackComponentState_GetTypeIDStatic;
ModDescriptionComponentState_GetTypeIDStatic_t ModDescriptionComponentState_GetTypeIDStatic;
GameObjectState_GetComponentByTypeId_t GameObjectState_GetComponentByTypeId;

// the reason why we have both GlobalIDMap_GetPointer and GlobalIDMap_GetPointer_Instance is that 
// if the game is still initializing sm_pInstance does not yet exist
// within the control executable and is only within the rl dll so GlobalIDMap_GetPointer can just lock the game up
// whereby GlobalIDMap_GetPointer_Instance will always work but requires a ptr to sm_pInstance
// TODO: maybe we should just do a function that fetches it automatically lol?
using GlobalIDMap_GetPointer_Instance_t = uint64_t * (__fastcall*)(uint64_t* a1, uint64_t* a2);
GlobalIDMap_GetPointer_Instance_t GlobalIDMap_GetPointer_Instance;

using isLootDropOrItemStackRelatedSomething_t = bool(__fastcall*)(LootDropComponentSingletonState *LootDropSingleton, LootDropEntry *lootDropEntryStart, LootDropNumToPick *lootDropNumToPick);
isLootDropOrItemStackRelatedSomething_t isLootDropOrItemStackRelatedSomething;

uint64_t* sm_pInstance = nullptr;
bool lootdropSingletonExists = false;
bool modifyLootDropsCheat = false;

void CreatePreAllocatedEntityList();

uint64_t currentObjectiveHash = 0x0;
// uint64_t currentMissionHash = 0x0; // not in use

std::vector<lootDropRestore_t> lootDropRestore = {};

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
	{0x10469758bd9f0051, 0x5b57da04e4e0054, 0, 3, "LOOT_ITEM_MOD_COMMON_DEATH_FEEDER", 15.0f, false, false, false, false}, //for some reason, this is the only one that doesn't have a relative value, we can't override this one in the inventory
	{0x10469758bd9f0051, 0xf72884f35910054, 0, 3, "LOOT_ITEM_MOD_COMMON_MENTAL_FOCUS", 21.0f, true, false, false, false},

	{0x31689a1f87650051, 0x33257ce200bbc054, 12,	3, "LOOT_ITEM_RESOURCE_COMMON_INSTRUCTIVE_PATTERN_TITLE", 0.0f, false, false, false, false},
	{0x31689a1f87650051, 0x2c6b227fef64054, 6,		3, "LOOT_ITEM_RESOURCE_UNCOMMON_ASTRAL_BLIP_TITLE", 0.0f, false, false, false, false},
	{0x2507179ee174c051, 0x33257ce200bbc054, 12,	3, "LOOT_ITEM_RESOURCE_COMMON_INSTRUCTIVE_PATTERN_TITLE", 0.0f, false, false, false, false},
	{0x2507179ee174c051, 0x2c6b227fef64054, 6,		3, "LOOT_ITEM_RESOURCE_UNCOMMON_ASTRAL_BLIP_TITLE", 0.0f, false, false, false, false},
	{0x2202525bea888051, 0x33257ce200bbc054, 12,	3, "LOOT_ITEM_RESOURCE_COMMON_INSTRUCTIVE_PATTERN_TITLE", 0.0f, false, false, false, false},
	{0x2202525bea888051, 0x2c6b227fef64054, 6,		3, "LOOT_ITEM_RESOURCE_UNCOMMON_ASTRAL_BLIP_TITLE", 0.0f, false, false, false, false},
	{0x13e456183f2a4051, 0x33257ce200bbc054, 12,	3, "LOOT_ITEM_RESOURCE_COMMON_INSTRUCTIVE_PATTERN_TITLE", 0.0f, false, false, false, false},
	{0x13e456183f2a4051, 0x2c6b227fef64054, 6,		3, "LOOT_ITEM_RESOURCE_UNCOMMON_ASTRAL_BLIP_TITLE", 0.0f, false, false, false, false},
	{0x3bd1d7e0cbaf4051, 0x33257ce200bbc054, 12,	3, "LOOT_ITEM_RESOURCE_COMMON_INSTRUCTIVE_PATTERN_TITLE", 0.0f, false, false, false, false},
	{0x3bd1d7e0cbaf4051, 0x2c6b227fef64054, 6,		3, "LOOT_ITEM_RESOURCE_UNCOMMON_ASTRAL_BLIP_TITLE", 0.0f, false, false, false, false},
	{0x1c7478e81fa9c051, 0x33257ce200bbc054, 12,	3, "LOOT_ITEM_RESOURCE_COMMON_INSTRUCTIVE_PATTERN_TITLE", 0.0f, false, false, false, false},
	{0x1c7478e81fa9c051, 0x2c6b227fef64054, 6,		3, "LOOT_ITEM_RESOURCE_UNCOMMON_ASTRAL_BLIP_TITLE", 0.0f, false, false, false, false},
	
	{0x2202525bea888051, 0x30dfea3a43960054, 0, 6, "LOOT_ITEM_MOD_UNCOMMON_LIGHT_FOOT", -12.0f, true, false, false, false},
	{0x2202525bea888051, 0x251d4ccb1aaf8054, 0, 6, "LOOT_ITEM_MOD_UNCOMMON_DAMAGE", 0.0f, false, false, false, false},
	{0x13e456183f2a4051, 0x30dfea3a43960054, 0, 6, "LOOT_ITEM_MOD_UNCOMMON_LIGHT_FOOT", -12.0f, true, false, false, false},
	{0x13e456183f2a4051, 0x251d4ccb1aaf8054, 0, 6, "LOOT_ITEM_MOD_UNCOMMON_DAMAGE", 0.0f, false, false, false, false},
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

		memset(a2->lootDropArray, 0, sizeof(a2->lootDropArrayMaxCounter) * sizeof(uint32_t));

		int counter = 0;
		for (int v = 0; v < a2->lootDropArrayMaxCounter; v++)
		{
			// so we dont divide by 0
			if (modDropList[i].dropRate == 0)
			{
				a2->lootDropArray[v] = 2;
				counter++;
				continue;
			}

			if (counter % modDropList[i].dropRate == 0)
				a2->lootDropArray[v] = 2;

			counter++;
		}

		printf("done\n");
	}
}

void restoreLootDropIndexArrayPointers()
{
	if (!lootdropSingletonExists)
		return;

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
		uint64_t* ptr = (uint64_t*)GlobalIDMap_GetPointer(&inventoryAddressLocation);

		uint32_t itemDescriptionTypeID = ItemDescriptionComponentState_GetTypeIDStatic();
		ItemDescription* itemDescription = (ItemDescription*)GameObjectState_GetComponentByTypeId((uint64_t)ptr, itemDescriptionTypeID);

		uint32_t modDescriptionTypeID = ModDescriptionComponentState_GetTypeIDStatic();
		ModDescription* modDescription = (ModDescription*)GameObjectState_GetComponentByTypeId((uint64_t)ptr, modDescriptionTypeID);
		
		if (itemDescription != nullptr && modDescription == nullptr)
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
				uint32_t itemCount = *(uint32_t*)(char*)(itemStackAddr+0x40);

				if (itemCount >= modDropList[v].materialCounter)
				{
					modDropList[v].isActivated = false;
					modDropList[v].isCollected = true;

					printf("collected material %s %i\n", modDropList[v].dropName, itemCount);
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

				printf("collected mod %s\n", modDropList[v].dropName);
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
		uint64_t* ptr = GlobalIDMap_GetPointer_Instance(*(uint64_t**)sm_pInstance, (uint64_t*)globalIDItem);
		ptr++; // the second ptr contains the string
		cfg->lootTableItemNames.push_back(*(char**)ptr);

		cfg->lootTableItemGlobalIDs.push_back(*(uint64_t*)globalIDItem);
		globalIDItem++;
	}

	printf("finished building loottable\n");

	CreatePreAllocatedEntityList();

	return result;
}

using completeObjective_t = uint64_t(__fastcall*)(uint64_t a1, uint64_t a2, uint64_t a3);
completeObjective_t completeObjectiveOrig;
uint64_t completeObjective(uint64_t a1, uint64_t a2, uint64_t a3)
{
	if (!modifyLootDropsCheat)
		return completeObjectiveOrig(a1, a2, a3);

	static bool derefNextStackPtr = false;

	uint64_t z = *(uint64_t*)_AddressOfReturnAddress();
	auto baseAddr = reinterpret_cast<uint64_t>(GetModuleHandle(nullptr));	
	auto offset = z - baseAddr;

	//if (derefNextStackPtr && a3 > 0x20000) // a3 sometimes isnt a pointer and we will crash if we try to read from it
	if (derefNextStackPtr && (offset == 0x3efda5 || offset == 0x3f0010))
	{
		printf("correct %llx\n", offset);
 		uint64_t objectiveHash = *(uint64_t*)a3;

		if (objectiveHash > IGNORE_MAX_INT)
		{
			uint64_t* globalIDPtr = (uint64_t*)GlobalIDMap_GetPointer((uint64_t*)a3);
			if (globalIDPtr != nullptr)
			{
				char* stringPtr = *(char**)((char*)globalIDPtr + 0x10);

				// don't include side objectives
				if (strstr(stringPtr, "BA_") == nullptr && strstr(stringPtr, "SM_") == nullptr)
				{
 					currentObjectiveHash = objectiveHash;

					// TODO: hack we can always reset all the moddroplist elements to false
					// theres a bug where if a current moddroplist is activated and the user goes past the objective hash
					// and he didnt get the drop, the moddroplist element will always be active which stops it from activating the other drops
					// since we are checking if theres a drop thats currently active
					size_t modDropListSize = sizeof(modDropList) / sizeof(dropManipulation);
					for (int i = 0; i < modDropListSize; i++)
					{
						modDropList[i].isActivated = false;
						modDropList[i].isCollected = false;
					}

					// start of a new game, restore the lootdrop table
					/*
					if (currentObjectiveHash == 0x80c6da414868051)
					{
						printf("resetting moddroplist\n");
						size_t modDropListSize = sizeof(modDropList) / sizeof(dropManipulation);

						// check if we are already waiting for a drop modification to finish
						for (int i = 0; i < modDropListSize; i++)
						{
							modDropList[i].isActivated = false;
							modDropList[i].isCollected = false;
						}
					}
					*/

					printf("objective %s\n", stringPtr);
					printf("objective hash %llx\n", objectiveHash);
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

/*
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
	}

	return result;
}
*/

uint64_t lootDropSingleton = 0x0;

using LootDropSingletonCtor_t = uint64_t(__fastcall*)(uint64_t a1);
LootDropSingletonCtor_t LootDropSingletonCtorOrig;
uint64_t LootDropSingletonCtor(uint64_t a1)
{
	uint64_t result = LootDropSingletonCtorOrig(a1);
	lootdropSingletonExists = true;
	lootDropSingleton = a1;

	return result;
}

using EntityComponentStateDtor_t = char*(__fastcall*)(char* block, char a2);
EntityComponentStateDtor_t EntityComponentStateDtorOrig;
char* EntityComponentStateDtor(char* block, char a2)
{
	lootdropSingletonExists = false;
	lootDropSingleton = 0x0;
	
	char* result = EntityComponentStateDtorOrig(block, a2);
	return result;
}

using createItemDrop_t = GenericEntityState*(__fastcall*)(__int64 a1, uint64_t* a2, float a3, __int64 a4);
createItemDrop_t createItemDropOrig;
GenericEntityState* createItemDrop(__int64 a1, uint64_t* a2, float a3, __int64 a4)
{
	GenericEntityState* result = createItemDropOrig(a1,a2,a3,a4);
	//printf("lootdrop entity %llx\n", result);

	if (!modifyLootDropsCheat)
		return result;

	if (!lootdropSingletonExists)
		return result;

	if (result != nullptr)
	{
		uint32_t relativeValueTypeID = RelativeValueModComponentState_GetTypeIDStatic();
		uint64_t relativeValueAddr = GameObjectState_GetComponentByTypeId((uint64_t)result, relativeValueTypeID);

		size_t modDropListSize = sizeof(modDropList) / sizeof(dropManipulation);

		uint32_t itemStackTypeID = ItemStackComponentState_GetTypeIDStatic();
		uint64_t itemStackAddr = GameObjectState_GetComponentByTypeId((uint64_t)result, itemStackTypeID);

		uint32_t modDescriptionTypeID = ModDescriptionComponentState_GetTypeIDStatic();
		ModDescription* modDescription = (ModDescription*)GameObjectState_GetComponentByTypeId((uint64_t)result, modDescriptionTypeID);
		
		bool isModWeModify = false;

		for (int i = 0; i < modDropListSize; i++)
		{
			// TODO HACKHACKHACK: find a proper way of setting the relative value so it saves
			// as of now it somehow either gets the value from the FlowConnectionManager or ahead of time
			// Control_DX11.exe+0x34DD4F
			if (modDescription != nullptr && modDescription->component->description != nullptr &&
				(strstr(modDescription->component->description->modName, "LOOT_ITEM_MOD_UNCOMMON_LIGHT_FOOT") != nullptr /*||
				strstr(modDescription->component->description->modName, "LOOT_ITEM_MOD_COMMON_MENTAL_FOCUS") != nullptr*/))
			{
				isModWeModify = true;
			}

			if (relativeValueAddr != NULL && modDropList[i].modifyAttribute && isModWeModify)
			{
				float* relativeValue = (float*)(relativeValueAddr + 0x40);
				*relativeValue = modDropList[i].attributeRating;
				isModWeModify = false;
			}

			if (modDropList[i].isActivated && mapIsLoaded)
			{
				// modify the drop rate regardless
				if (itemStackAddr != NULL && modDescription == nullptr)
				{
					uint32_t* itemstackValue = (uint32_t*)(char*)(itemStackAddr + 0x40);
					*itemstackValue = 4;
				}
			}
		}
	}

	return result;
}

struct LootDropComponent
{
	uint64_t* vftable;
	uint32_t num1;
	uint32_t num2;
	uint64_t globalID;
	uint64_t num3;
	uint64_t num4;
	uint32_t num5;
	uint32_t num6;
	uint64_t num7;
	uint64_t num8;
};

std::vector<LootDropComponent*> lootdropPool;

// no idea when this gets called
using LootDropComponentRuntime3_t = uint64_t(__fastcall*)(__int64 a1, int a2, __int64 a3, unsigned __int64 a4);
LootDropComponentRuntime3_t LootDropComponentRuntimeCtor3Orig;
uint64_t LootDropComponentRuntimeCtor3(__int64 a1, int a2, __int64 a3, unsigned __int64 a4)
{
	printf("lootdrop 3\n");
	return LootDropComponentRuntimeCtor3Orig(a1, a2, a3, a4);
}

// preallocates entities before the game starts, we fetch the lootdrop pool id from the component
using LootDropComponentRuntime2_t = uint64_t(__fastcall*)(__int64 a1, __int64 a2, unsigned __int64 a3);
LootDropComponentRuntime2_t LootDropComponentRuntimeCtor2Orig;
uint64_t LootDropComponentRuntimeCtor2(__int64 a1, __int64 a2, unsigned __int64 a3)
{
	uint64_t result = LootDropComponentRuntimeCtor2Orig(a1, a2, a3);
	lootdropPool.push_back((LootDropComponent*)result);
	return result;
}

// this gets called when we are loading a level, maybe runtime too?
using LootDropComponentRuntime1_t = uint64_t(__fastcall*)(uint64_t a1, uint64_t a2);
LootDropComponentRuntime1_t LootDropComponentRuntimeCtorOrig;
uint64_t LootDropComponentRuntimeCtor(uint64_t a1, uint64_t a2)
{
	printf("lootdrop 1\n");
	return LootDropComponentRuntimeCtorOrig(a1, a2);
}

// could be good to have when we want to know what entities that are loaded into a level which component pool id they have
using LootDropComponentState_t = uint64_t(__fastcall*)(uint64_t a1);
LootDropComponentState_t LootDropComponentStateOrig;
uint64_t LootDropComponentStateCtor(uint64_t a1)
{
	printf("lootdrop state\n");
	return LootDropComponentStateOrig(a1);
}

// TODO: get the entity owner from the loot component
void CreatePreAllocatedEntityList()
{
	printf("done\n");
}

using LootDropEnemyTopFunction_t = void(__fastcall*)(uint64_t a1);
LootDropEnemyTopFunction_t LootDropEnemyTopFunctionOrig;
void LootDropEnemyTopFunction(uint64_t a1)
{
	char* ptr = (char*)a1;
	char* componentState = *(char**)(ptr + 0x38);
	uint64_t globalID = *(uint64_t*)(componentState + 0x10);

	uint32_t enemyLevel = **(uint32_t**)(a1 + 0x40);
	LootDropComponentSingletonState* singleton = (LootDropComponentSingletonState*)lootDropSingleton;

	uint64_t singletonPoolGlobalID = singleton->lootdropState.globalID;

	printf("enemy globalID %llx, enemy level %d, singletonLootDropGlobalID %llx\n", globalID, enemyLevel, singletonPoolGlobalID);
	LootDropEnemyTopFunctionOrig(a1);
}

using LootDropNewIndexArray_t = uint32_t(__fastcall*)(void **indexArrayPtr, void **a2, uint64_t *seedNumber);
LootDropNewIndexArray_t LootDropNewIndexArray;

uint32_t indexArray[60] = { 0 };

// TODO: change the variable to use function arg to let us pass any poolID
void PrintLootDropInformationFromPools()
{
	uint64_t counter = 0;
	uint64_t lootTableDataPoolTotalOffset = 0;
	uint64_t processStartAddr = (uint64_t)(GetModuleHandle(nullptr));
	char* globalIDDataPool = *(char**)(*(char**)(processStartAddr + 0x1166fc0) + 0x568);
	uint32_t globalIDDataPoolNum = *(uint32_t*)((*(char**)(processStartAddr + 0x1166fc0)) + 0x564);

	char* singleton = (char*)lootDropSingleton;
	LootDropComponentSingletonState* singletonState = (LootDropComponentSingletonState*)lootDropSingleton;
	LootDropGlobalIDDataPoolEntry* poolEntrySomeGlobalIDWillGet = nullptr;

	//uint64_t poolID = 0x2CF4D554CF4F4052; // from vendor, we get 5 pointers from it
	uint64_t poolID = 0x36ada6a3ab26c052; // starting enemies on new game
	uint64_t singletonLootDropID = 0x3681700878ea8053;

	do
	{
		char* lootdropPoolEntry = &globalIDDataPool[lootTableDataPoolTotalOffset];
		uint64_t poolGlobalID = *(uint64_t*)lootdropPoolEntry;
		uint64_t poolSingletonGlobalID = *(uint64_t*)(lootdropPoolEntry + 0x18);
		//uint64_t singletonLootDropID = *(uint64_t*)(singleton + 0x90);
		//uint64_t singletonLootDropID = singletonState->lootdropState.globalID;

		if (poolGlobalID == poolID && poolSingletonGlobalID == singletonLootDropID)
			poolEntrySomeGlobalIDWillGet = (LootDropGlobalIDDataPoolEntry*)&globalIDDataPool[lootTableDataPoolTotalOffset];

		++counter;
		lootTableDataPoolTotalOffset += 0x48;
	}
	while ( counter < globalIDDataPoolNum );


	if (!poolEntrySomeGlobalIDWillGet)
	{
		counter = 0;
		lootTableDataPoolTotalOffset = 0;

		do
		{
			char* lootdropPoolEntry = &globalIDDataPool[lootTableDataPoolTotalOffset];
			uint64_t poolGlobalID = *(uint64_t*)lootdropPoolEntry;

			if (poolGlobalID == poolID)
				poolEntrySomeGlobalIDWillGet = (LootDropGlobalIDDataPoolEntry*)&globalIDDataPool[lootTableDataPoolTotalOffset];

			++counter;
			lootTableDataPoolTotalOffset += 0x48;
		}
		while ( counter < globalIDDataPoolNum );
	}

	LootDropEntry* globalID_DropTableEntry_Start = (LootDropEntry*)poolEntrySomeGlobalIDWillGet->ptr;
	uint32_t amountOfTables = poolEntrySomeGlobalIDWillGet->amount;
	LootDropEntry* globalID_DropTableEntry_End = &globalID_DropTableEntry_Start[amountOfTables];
	std::vector<LootdropSingletonLinkedEntry*> lootdropArray = {};

	LootDropState* lootdropState = *(LootDropState**)*(char**)(singleton + 0x40);
	LootdropSingletonLinkedEntry* lootdropsingleton_entry = lootdropState->lootdrop.linkedList;
    LootdropSingletonLinkedEntry* lootdropsingleton_linkedlist_entry = (LootdropSingletonLinkedEntry*)lootdropState->lootdrop.linkedList->list.parent;
	LootdropSingletonLinkedEntry* lootdropStateLinkedList = lootdropState->lootdrop.linkedList;
	LootDropSmallEntry* linkedListSmallEntry = nullptr;
	
	// lootdrop singleton has a black red tree entry of all the lootdrop pool's, their current drop index and globalid drop table
	if (!lootdropsingleton_linkedlist_entry->list.addNodeLeft)
	{
		do
		{
			linkedListSmallEntry = (LootDropSmallEntry*)GlobalIDMap_GetPointer(&lootdropsingleton_linkedlist_entry->list.globalID);
			if ( linkedListSmallEntry >= (LootDropSmallEntry*)GlobalIDMap_GetPointer((uint64_t*)globalID_DropTableEntry_Start))
			{
				lootdropsingleton_entry = lootdropsingleton_linkedlist_entry;
				lootdropsingleton_linkedlist_entry = (LootdropSingletonLinkedEntry*)lootdropsingleton_linkedlist_entry->list.left;
			}
			else
			{
				lootdropsingleton_linkedlist_entry = (LootdropSingletonLinkedEntry*)lootdropsingleton_linkedlist_entry->list.right;
			}
		}
		while ( !lootdropsingleton_linkedlist_entry->list.addNodeLeft );

		lootdropStateLinkedList = lootdropState->lootdrop.linkedList;
	}

	LootDropContent* currentDropTable = (LootDropContent*)lootdropsingleton_entry->content.indexArray;

	if (globalID_DropTableEntry_Start != globalID_DropTableEntry_End)
	{
		do
		{
			if (GlobalIDMap_GetPointer((uint64_t*)globalID_DropTableEntry_Start) && *((char*)GlobalIDMap_GetPointer((uint64_t*)globalID_DropTableEntry_Start) + 8))
			{
				LootdropSingletonLinkedEntry* linkedEntry = lootdropState->lootdrop.linkedList;
				LootdropSingletonLinkedEntry* nextLinkedEntry = (LootdropSingletonLinkedEntry*)lootdropState->lootdrop.linkedList->list.parent;
				LootdropSingletonLinkedEntry* linkedList = lootdropState->lootdrop.linkedList;

				// lootdrop singleton has a black red tree entry shit from lootdrop.cpp
				if (!nextLinkedEntry->list.addNodeLeft)
				{
					do
					{
						LootDropSmallEntry* modPtr = (LootDropSmallEntry*)GlobalIDMap_GetPointer(&nextLinkedEntry->list.globalID);
						if ( modPtr >= GlobalIDMap_GetPointer((uint64_t*)globalID_DropTableEntry_Start) )
						{
							  linkedEntry = nextLinkedEntry;
							  nextLinkedEntry = (LootdropSingletonLinkedEntry*)nextLinkedEntry->list.left;
						}
						else
						{
							nextLinkedEntry = (LootdropSingletonLinkedEntry*)nextLinkedEntry->list.right;
						}
					}
					while ( !nextLinkedEntry->list.addNodeLeft );

					linkedList = lootdropState->lootdrop.linkedList;
				}

				LootDropNumToPick numToPick = {};
				numToPick.lootdropTableToPick = 0;

				// we only care about globalID_DropTableEntry_Start and when to jump to the next entry in the start end entry list
				if (linkedEntry != linkedList)
				{
					LootDropSmallEntry* modGlobalIDPtr = (LootDropSmallEntry*)GlobalIDMap_GetPointer((uint64_t*)globalID_DropTableEntry_Start);


					// this is what the game actually calls to filter our entries
					// the if statement within isLootDropOrItemStackRelatedSomething where it checks for if ( lootDropEntryStart->amount ) is messing us up
					// in getting consistent entries since it depends on stuff within lootdropsingleton, maybe its fine for enemies?
					// for boxes def not since it often relies on entries where lootDropEntryStart->amount is set
					/*
					if (modGlobalIDPtr >= GlobalIDMap_GetPointer(&linkedEntry->list.globalID) &&
					linkedEntry != lootdropState->lootdrop.linkedList &&
					isLootDropOrItemStackRelatedSomething((LootDropComponentSingletonState*)lootDropSingleton, globalID_DropTableEntry_Start, &numToPick))
					{
						lootdropArray.push_back(linkedEntry);
					}
					*/

					if (modGlobalIDPtr >= GlobalIDMap_GetPointer(&linkedEntry->list.globalID) && linkedEntry != lootdropState->lootdrop.linkedList)
					{
						if (globalID_DropTableEntry_Start->amount)
							lootdropArray.push_back(linkedEntry);
						else
						{
							if (isLootDropOrItemStackRelatedSomething((LootDropComponentSingletonState*)lootDropSingleton, globalID_DropTableEntry_Start, &numToPick))
								lootdropArray.push_back(linkedEntry);
						}
					}
				}
			}
			++globalID_DropTableEntry_Start;
		} while (globalID_DropTableEntry_Start != globalID_DropTableEntry_End);
	}
	
	if (lootdropArray.size() > 0)
	{
		printf("id %llx\n", lootdropArray[0]);

		uint32_t* memsetAmountArray = *(uint32_t**)((char*)lootdropArray[0] + 0x40);
		uint32_t memsetCount = *(uint32_t*)((char*)lootdropArray[0] + 0x48);
		uint32_t indexArrayCounter = 0;

		for (uint32_t i = 0; i < memsetCount; i++)
		{
			for (uint32_t v = 0; v < memsetAmountArray[i]; v++)
				indexArray[v + indexArrayCounter] = i;

			indexArrayCounter += memsetAmountArray[i];
		}

		void* startOfIndexArray = &indexArray[0];
		void* endOfIndexArray = &indexArray[indexArrayCounter];
		uint32_t seedNumber = lootdropArray[0]->content.seedNumber;

		LootDropNewIndexArray(&startOfIndexArray, &endOfIndexArray, (uint64_t*)&seedNumber);
		printf("start of indexarray %llx\n", &indexArray[0]);
	}

	printf("done lootdrop table gathering testing");
}

using PlayerProperties_t = uint64_t(__fastcall*)(uint64_t a1);
PlayerProperties_t PlayerPropertiesOrig;
uint64_t PlayerPropertiesComponentStateCtor(uint64_t a1)
{
	uint64_t result = PlayerPropertiesOrig(a1);
	printf("playerproperties: %llx\n", a1);
	return result;
}

void initLootDropHooks(uint64_t processStartAddr, HMODULE rlModule, uint64_t coregameDllAddr)
{
	GlobalIDMap_GetPointer_Instance = (GlobalIDMap_GetPointer_Instance_t)GetProcAddress(rlModule, "?getPointer@GlobalIDMap@r@@QEBAPEAXAEBVGlobalID@2@@Z");
	sm_pInstance = (uint64_t*)GetProcAddress(rlModule, "?sm_pInstance@GlobalIDMap@r@@0PEAV12@EA");
	isLootDropOrItemStackRelatedSomething = (isLootDropOrItemStackRelatedSomething_t)((char*)processStartAddr + 0x3dfd60);

	GameObjectState_GetComponentByTypeId = (GameObjectState_GetComponentByTypeId_t)GetProcAddress(rlModule, "?getComponentByTypeId@GameObjectState@r@@QEBAPEAVComponentStateBase@2@I@Z");
	RelativeValueModComponentState_GetTypeIDStatic = (RelativeValueModComponentState_GetTypeIDStatic_t)((char*)processStartAddr + 0x507240);
	ItemDescriptionComponentState_GetTypeIDStatic = (ItemDescriptionComponentState_GetTypeIDStatic_t)((char*)processStartAddr + 0x177D90);
	ModDescriptionComponentState_GetTypeIDStatic = (ItemDescriptionComponentState_GetTypeIDStatic_t)((char*)processStartAddr + 0x1AF200);
	ItemStackComponentState_GetTypeIDStatic = (ItemStackComponentState_GetTypeIDStatic_t)((char*)processStartAddr + 0x14D410);

	LootDropNewIndexArray = (LootDropNewIndexArray_t)((char*)processStartAddr + 0x29efa0);

	char* PlayerPropertiesAddr = (char*)processStartAddr + 0x1b0ef0;
	if (MH_CreateHook(PlayerPropertiesAddr, &PlayerPropertiesComponentStateCtor, reinterpret_cast<LPVOID*>(&PlayerPropertiesOrig)) != MH_OK) throw;
	if (MH_EnableHook(PlayerPropertiesAddr) != MH_OK) throw;

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

	// TODO: remove the comment we just stub it out for now cause of ida
	/*
	char* decrementDropTableCounter_ReturnOffsetCounterAddr = (char*)processStartAddr + 0x29B610;
	if (MH_CreateHook(decrementDropTableCounter_ReturnOffsetCounterAddr, &decrementDropTableCounter_ReturnOffsetCounter, reinterpret_cast<LPVOID*>(&decrementDropTableCounter_ReturnOffsetCounterOrig)) != MH_OK) throw;
	if (MH_EnableHook(decrementDropTableCounter_ReturnOffsetCounterAddr) != MH_OK) throw;
	*/

	char* completeObjectiveAddr = (char*)processStartAddr + 0x3F1AF0;
	if (MH_CreateHook(completeObjectiveAddr, &completeObjective, reinterpret_cast<LPVOID*>(&completeObjectiveOrig)) != MH_OK) throw;
	if (MH_EnableHook(completeObjectiveAddr) != MH_OK) throw;

	/*
	char* completeMissionAddr = (char*)processStartAddr + 0x3F0F50;
	if (MH_CreateHook(completeMissionAddr, &completeMission, reinterpret_cast<LPVOID*>(&completeMissionOrig)) != MH_OK) throw;
	if (MH_EnableHook(completeMissionAddr) != MH_OK) throw;
	*/

	char* LootDropComponentAddr1 = (char*)processStartAddr + 0x752430;
	if (MH_CreateHook(LootDropComponentAddr1, &LootDropComponentRuntimeCtor, reinterpret_cast<LPVOID*>(&LootDropComponentRuntimeCtorOrig)) != MH_OK) throw;
	if (MH_EnableHook(LootDropComponentAddr1) != MH_OK) throw;

	char* LootDropComponentAddr2 = (char*)processStartAddr + 0x752380;
	if (MH_CreateHook(LootDropComponentAddr2, &LootDropComponentRuntimeCtor2, reinterpret_cast<LPVOID*>(&LootDropComponentRuntimeCtor2Orig)) != MH_OK) throw;
	if (MH_EnableHook(LootDropComponentAddr2) != MH_OK) throw;

	char* LootDropComponentAddr3 = (char*)processStartAddr + 0x7523C0;
	if (MH_CreateHook(LootDropComponentAddr3, &LootDropComponentRuntimeCtor3, reinterpret_cast<LPVOID*>(&LootDropComponentRuntimeCtor3Orig)) != MH_OK) throw;
	if (MH_EnableHook(LootDropComponentAddr3) != MH_OK) throw;

	char* LootDropComponentStateAddr = (char*)processStartAddr + 0x3DC320;
	if (MH_CreateHook(LootDropComponentStateAddr, &LootDropComponentStateCtor, reinterpret_cast<LPVOID*>(&LootDropComponentStateOrig)) != MH_OK) throw;
	if (MH_EnableHook(LootDropComponentStateAddr) != MH_OK) throw;

	char* LootDropEnemyTopFunctionAddr = (char*)processStartAddr + 0x3db7b0;
	if (MH_CreateHook(LootDropEnemyTopFunctionAddr, &LootDropEnemyTopFunction, reinterpret_cast<LPVOID*>(&LootDropEnemyTopFunctionOrig)) != MH_OK) throw;
	if (MH_EnableHook(LootDropEnemyTopFunctionAddr) != MH_OK) throw;
}