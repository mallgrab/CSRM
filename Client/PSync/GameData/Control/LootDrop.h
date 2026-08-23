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
#include "GameData.h"

#define IGNORE_MAX_INT 0xFFFFFFFFFFFF
#define LOOT_TABLE_GAP 0x18

extern bool modifyLootDropsCheat;

struct GenericEntityState
{
	uint64_t* vtable;
	char tmp2[0x10];
	uint64_t globalID;
	char tmp[0x50-0x8];
	uint64_t* componentsPtr;
	uint32_t amountOfComponents;
	uint32_t maxAmountOfComponents;
};

/*
	loot drop has a globalid that points to a tagcomponent
	the tagcomponent also has the same globalid as the loot drop
*/
struct currentDropTable
{
	char tmp1[0x20];
	uint32_t* lootDropArray;
	uint32_t lootDropArrayCounter;
	uint32_t lootDropArrayMaxCounter;
	uint32_t seedNumber;
	uint32_t unused;
	uint64_t* possibleLootDropGlobalIDs;
	uint32_t possibleLootDropCounter;
	uint32_t possibleLootDropMaxCounter;
};

struct dropManipulation {
	uint64_t objectiveHash;
	uint64_t objectID;
	uint64_t materialCounter;
	uint64_t dropRate;
	char dropName[500];
	float attributeRating;
	bool modifyAttribute;
	bool isMaterial;
	bool isCollected;
	bool isActivated;
};

// TODO: for every modifyLootDrops call check if the indexArray is in our vector, if its not add it to the lootDropRestore vector
struct lootDropRestore_t
{
	uint32_t* indexArray;		// before we modify the array, malloc and fill it up the exact same way the array looks
	uint32_t* indexPtr;			// store the index array ptr
	size_t indexMaxCount;
	uint64_t* lootDropPtr;		// store the location we overwrote the globalid at (not the lootdrop globalid array ptr)
	uint64_t lootDropGlobalID;	// store the globalID we are about to overwrite
};

struct Inventory {
	char tmp[0x40];
	uint64_t* inventory;
	uint32_t currentAmount;
	uint32_t maxAmount;
};

struct ItemDescriptionComponent {
	uint64_t* vtable;
	uint64_t tmp;
	char* itemName;
	char* itemName2;
};

struct ItemDescription {
	uint64_t* vtable;
	char tmp[0x30];
	ItemDescriptionComponent* component;
};

struct ModDescriptionString {
	char tmp[0x48];
	char* modName;
	char* modName2;
};

struct ModDescriptionComponent {
	uint64_t* vtable;
	char tmp[0x30];
	ModDescriptionString* description;
};

struct ModDescription {
	uint64_t* vtable;
	char tmp[0x30];
	ModDescriptionComponent* component;
};

struct LootDropContent
{
  uint64_t tmp1; // TODO: prob add this in ida too
  uint32_t seedNumber;
  uint32_t number1;
  uint32_t *indexArray;
  uint32_t number2;
  uint32_t number3;
  uint64_t *lootDropArray;
  uint32_t lootDropArrayAmount;
  uint32_t lootDropArrayMaxAmount;
  uint64_t namelistID;
  uint64_t namelistID2;
  uint64_t *lootDropGlobalIDs;
  uint32_t lootdropGlobalIDAmount;
  uint32_t lootdropGlobalIDMaxAmount;
  uint64_t number9;
  char tmp[16];
};

struct LootDropMod
{
  uint64_t globalID;
  uint64_t *ptr;
  uint64_t num1;
  uint64_t num2;
};

struct EntityComponentState
{
  uint64_t *vftable;
  uint32_t flags;
  uint32_t tmp1;
  uint64_t num1;
  uint64_t globalID;
  uint64_t tmp2;
  uint64_t *owner;
  uint64_t tmp3;
  uint64_t *component;
};

struct LootDropNumToPick
{
  uint32_t num1;
  uint32_t num2;
  uint64_t globalID;
  uint64_t *ptr;
  uint32_t num3;
  uint32_t num4;
  uint32_t lootdropTableToPick;
  uint32_t num5;
};

struct LootDropGlobalIDDataPoolEntry
{
  uint64_t globalID;
  uint64_t num1;
  uint32_t num2;
  uint32_t num3;
  uint64_t globalID2;
  uint64_t num4;
  uint32_t num5;
  uint32_t num6;
  uint64_t *ptr;
  uint32_t amount;
  uint32_t maxAmount;
  uint64_t num7;
};

struct GlobalIDDataPool
{
  char tmp[1380];
  uint32_t num;
  struct LootDropGlobalIDDataPoolEntry *lootdropPool;
  char tmp2[1024];
};

struct LootDropStartLinkedList
{
  struct LootDropLinkedList *left;
  struct LootDropLinkedList *parent;
  struct LootDropLinkedList *right;
  char color;
  bool addNodeLeft;
  char tmp[6];
  uint64_t globalID;
};

struct LootdropSingletonLinkedEntry
{
  struct LootDropStartLinkedList list;
  struct LootDropEntry **entry;
  struct LootDropContent content;
};

struct LootDropStateLinkedList
{
  struct LootdropSingletonLinkedEntry *linkedList;
  uint32_t amount;
  uint32_t maxAmount;
};

struct LootDropState
{
  struct LootDropStateLinkedList lootdrop;
  struct LootDropStateLinkedList linkedList2;
  uint64_t *globalIDs;
  uint32_t globalIDCounter;
  uint32_t globalIDMaxCounter;
  uint64_t num3;
  uint32_t num4;
  uint32_t num5;
  char tmp4;
  char spinLock;
  char tmp5[0x2];
  uint32_t num6;
  uint32_t num7;
  uint32_t num8;
  uint64_t globalID;
  uint64_t *ptr;
  uint32_t num9;
  uint32_t num10;
  uint64_t globalID_2;
  uint64_t globalID_3;
  uint64_t num11;
  uint64_t num12;
  uint32_t num13;
  uint32_t num14;
  uint64_t globalID_4;
  uint64_t num15;
  uint64_t num16;
  uint64_t globalID_5;
  uint32_t num17;
  uint32_t num18;
  uint64_t *string;
};

struct LootDropComponentSingletonState
{
  struct EntityComponentState componentState;
  struct LootDropState lootdropState;
};

struct LootDropEntry
{
  uint64_t globalID;
  struct LootDropSmallEntry *ptr;
  uint64_t number1;
  uint32_t killCounter;
  uint32_t number4;
  uint64_t globalID2;
  uint32_t amountTable;
  uint32_t maxAmountTable;
  uint64_t *ptr2;
  uint32_t amount;
  uint32_t maxAmount;
  uint64_t number3;
  uint64_t singletonGlobalID;
  uint64_t num2;
  uint64_t num3;
};

struct LootdropTopEntry
{
  uint64_t poolGlobalID;
  uint64_t *ptr1;
  uint64_t tmp1;
  uint64_t singletonGlobalID;
  uint64_t *ptr2;
  uint64_t tmp2;
  struct LootDropEntry *lootdropEntry;
  uint32_t amount;
  uint32_t maxAmount;
};

struct LootDropGlobalIDPoolEntry
{
  struct LootdropTopEntry *ptrTopEntry;
  uint64_t *globalIDArray;
  uint32_t amount;
  uint32_t maxAmount;
};

struct LootDropLinkedEntry
{
  uint64_t globalID;
  struct LootDropEntry **lootdropEntry;
  uint32_t amount;
  uint32_t maxAmount;
};

struct LootDropLinkedList
{
  struct LootDropStartLinkedList list;
  struct LootDropEntry **entry;
  uint32_t amount;
  uint32_t amountMax;
  char type;
  char tmp2[7];
};

struct LootDropSmallEntry
{
  LootDropMod *mod;
  uint32_t amount;
  uint32_t amountMax;
  uint32_t num1;
  uint32_t num2;
};

struct LootDropTop
{
  uint64_t globalID;
  uint64_t *ptr1;
  uint32_t num1;
  uint32_t num2;
  uint64_t globalID2;
  uint64_t *globalIDsMaybe;
  uint32_t num3;
  uint32_t num4;
  struct LootDropEntry *entry;
  uint32_t amount;
  uint32_t maxAmount;
  uint32_t num5;
  uint32_t num6;
  uint64_t globalID3;
  uint64_t *ptr4;
  uint32_t num7;
  uint32_t num8;
  uint64_t globalID4;
};

struct LootDropTableEntryList
{
  struct LootDropStartLinkedList list;
  struct LootDropEntry **entries;
  uint32_t amountLootDropEntrys;
  uint32_t amountLootDropEntrysMax;
  uint32_t num1;
  uint32_t num2;
};

void initLootDropHooks(uint64_t processStartAddr, HMODULE rlModule, uint64_t coregameDllAddr);
void PrintLootDropInformationFromPools(uint64_t poolID, uint64_t singletonLootDropID);

using RelativeValueModComponentState_GetTypeIDStatic_t = uint32_t(__fastcall*)();
extern RelativeValueModComponentState_GetTypeIDStatic_t RelativeValueModComponentState_GetTypeIDStatic;

using ItemDescriptionComponentState_GetTypeIDStatic_t = uint32_t(__fastcall*)();
extern ItemDescriptionComponentState_GetTypeIDStatic_t ItemDescriptionComponentState_GetTypeIDStatic;

using ItemStackComponentState_GetTypeIDStatic_t = uint32_t(__fastcall*)();
extern ItemStackComponentState_GetTypeIDStatic_t ItemStackComponentState_GetTypeIDStatic;

using ModDescriptionComponentState_GetTypeIDStatic_t = uint32_t(__fastcall*)();
extern ModDescriptionComponentState_GetTypeIDStatic_t ModDescriptionComponentState_GetTypeIDStatic;

using GameObjectState_GetComponentByTypeId_t = uint64_t(__fastcall*)(uint64_t Component, uint32_t TypeIDStatic);
extern GameObjectState_GetComponentByTypeId_t GameObjectState_GetComponentByTypeId;

/*
	common = 1 (grey)
	uncommon = II (green)
	rare = III (blue)
	legendary = IV (orange)
	exquisite = V (red)

	//personal mods
	mental fortitude = energy boost
	mental focus = energy recovery
	improved physique = health boost
	rapid recovery = health recovery
	fleet foot = speed boost (UNUSED/CUT/REMOVED)
	launcher = launch efficiency
	efficient shield = shield efficiency
	reinforced shield = shield boost
	shield reload = shield reload efficiency
	light foot = dodge efficiency
	compeller = sieze efficiency

	//weapon mods
	reload
	accuracy
	dead-eye = headshot boost
	bloodthirst = consecutive kills boost
	controlled fire = pierce aimed damage boost
	adrenaline = low health boost
	death feeder = energy renewaldamage
	ammo efficiency
	rate of fire
	sustained fire = ammo refund
	eternal fire = 100% ammo refund (grip special mod)
	deadly rain = levitation ammo efficiency

	//specific weapon mods
	gyroscope = spin groping efficiency
	receiver = spin rate of fire boost
	missile blast radius = charge blast boost
	missile velocity = charge velocity boost
	pellet count = shatter projectile boost
	pellet scatter = shatter projectile choke
	stabilizer = shatter recoil efficiency
	zoom = pierce zoom boost
	bosoter = pierce charging efficiency
*/


// TODO (later): find out how the lootdrop table from the singleton gets created
/*
	game reads a string that contains the entire array a lootdrop index array should have
	inside of writeLootDropIndexes

	how does it get the string?
*/

/*
void printInventoryAddressLocation()
{
	uint64_t processStartAddr = (uint64_t)(GetModuleHandle(nullptr));
	uint64_t gameInventoryComponent = (uint64_t) * (char**)(*(char**)(0x1111108 + processStartAddr + 0x8) + 0x38);
	uint64_t inventory = (uint64_t) * (char**)(gameInventoryComponent + 0x40);

	// not recent but its just the drop we get when we open up a box atm
	uint64_t recentItemDrop = inventory+0x460;
	uint64_t* ptr = GlobalIDMap_GetPointer((uint64_t*)recentItemDrop);

	printf("inventory %llx\n", ptr);
}
*/

/*
// print drop table
for (int i = 0; i < a2->possibleLootDropMaxCounter; i++)
{
	char* globalIDFromDropTable = (char*)a2->possibleLootDropGlobalIDs + (i * LOOT_TABLE_GAP);

	uint64_t* ptr = GlobalIDMap_GetPointer((uint64_t*)globalIDFromDropTable);
	ptr++; // the second ptr contains the string

	//printf("droptable: %s\n", *(char**)ptr);
}
//printf("\n");

// print index drop
if (a2->lootDropArrayCounter != 0)
{
	for (int i = 0; i < a2->lootDropArrayMaxCounter; i++)
	{
		uint32_t lootdropCurrentCounter = a2->lootDropArray[i];

		// for some reason the "max counter" is wrong, maybe it got written to it from a different thread?
		if (lootdropCurrentCounter > a2->possibleLootDropMaxCounter)
			continue;

		char* globalIDFromDropTable = (char*)a2->possibleLootDropGlobalIDs + (lootdropCurrentCounter * 0x18);
		uint64_t* ptr = GlobalIDMap_GetPointer((uint64_t*)globalIDFromDropTable);
		ptr++; // the second ptr contains the string

		if (a2->lootDropArrayCounter == (i + 1))
			printf("x droptable rng: %s\n", *(char**)ptr);
		else
			printf("droptable rng: %s\n", *(char**)ptr);
	}
	//printf("\n");
}
*/

// modify the current drop that we are suppose to get with our own selection
/*
char* globalIDFromDropTable = (char*)a2->globalIDPtr + (result * LOOT_TABLE_GAP);
uint64_t* ptr = GlobalIDMap_GetPointer((uint64_t*)globalIDFromDropTable);
ptr++;

if (strstr(*(char**)ptr, "_Nothing") != nullptr)
	if (cfg->lootTableIndex != -1)
		memcpy(globalIDFromDropTable, &lootTableItemGlobalIDs[cfg->lootTableIndex], sizeof(uint64_t));
*/

// temp thing just so we can set stuff with imgui menu
/*
char* globalIDFromDropTable = (char*)a2->possibleLootDropGlobalIDs + (result * LOOT_TABLE_GAP);
if (cfg->lootTableIndex != -1)
{
	printf("globalID from loot %llx\n", cfg->lootTableItemGlobalIDs[cfg->lootTableIndex]);
	memcpy(globalIDFromDropTable, &cfg->lootTableItemGlobalIDs[cfg->lootTableIndex], sizeof(uint64_t));
}
*/

// loot drop / inventory item related examples
/*
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

	//	itemArray = *(GameInventoryComponentState + 0x40);// items array
	//	endOfItemArray = &itemArray[2 * *(GameInventoryComponentState + 0x48)];// amount of items
	//
	//	itemGlobalID = *itemArray;
	//	ItemEntityFromGlobalID = GetItemEntityFromGlobalID(&itemGlobalID);
	//
	//	TypeIDStatic = LootDropItemComponentState::getTypeIDStatic();
	//	LootDropItemComponentState = r::GameObjectState::getComponentByTypeId(ItemEntityFromGlobalID, TypeIDStatic);

	//	LootDropSingletonTypeID = LootDropSingletonComponentState::getTypeIDStatic();
	//	charactermanager?
	//	r::GameObjectState::getComponentByTypeId

	if (itemDescriptionComponent != NULL)
		printf("item description %s\n", *(char**)(*(char**)(itemDescriptionComponent + 0x38) + 0x10));

	if (itemStackComponent != NULL)
	{
		uint64_t* counter = (uint64_t*)(char*)(itemStackComponent + 0x40);
		*counter = -10000;
	}

	return result;
}
*/