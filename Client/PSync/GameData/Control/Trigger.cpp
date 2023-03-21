#include "Trigger.h"

triggerComponentCtorOriginal_t triggerComponentCtorOriginal;
triggerComponentDtorOriginal_t triggerComponentDtorOriginal;
enterTriggerOriginal_t enterTriggerOriginal;

std::vector<coregame::TriggerComponent*> triggerPtrs;
int triggerCount = 0;

void* __fastcall triggerComponentCtor(coregame::TriggerComponent* pointer, uint64_t* pointer_b) {
	void* result = triggerComponentCtorOriginal(pointer, pointer_b);
	coregame::TriggerComponent* trigger = pointer;

	for (int i = 0; i < triggerPtrs.size(); i++)
	{
		if (triggerPtrs.at(i) == nullptr)
		{
			triggerPtrs.at(i) = trigger;
			break;
		}
	}

	triggerCount++;
	return result;
}

void* __fastcall triggerComponentDtor(coregame::TriggerComponent* pointer) {
	for (int i = 0; i < triggerPtrs.size(); i++)
	{
		if (triggerPtrs.at(i) == pointer)
		{
			triggerPtrs.at(i) = nullptr;
			break;
		}
	}

	triggerCount--;
	return triggerComponentDtorOriginal(pointer);
}

char __fastcall enterTrigger(uint64_t* pointer, uint64_t* pointer_b, uint64_t a3) {
	printf("trigger: 0x%llx gameobjectstate: 0x%llx a3: %lu\n", pointer, pointer_b, a3);

	return enterTriggerOriginal(pointer, pointer_b, a3);
}

void TriggerInstallHooks(uint64_t coregameDllAddr)
{
	ptr* triggerComponentCtorFunctionAddr = reinterpret_cast<ptr*>(coregameDllAddr + 0x1c6110);
	ptr* triggerComponentDtorFunctionAddr = reinterpret_cast<ptr*>(coregameDllAddr + 0x1c62b0);
	ptr* enterTriggerFunctionAddr = reinterpret_cast<ptr*>(coregameDllAddr + 0x1c6db0);

	if (MH_CreateHook(triggerComponentCtorFunctionAddr, &triggerComponentCtor, reinterpret_cast<LPVOID*>(&triggerComponentCtorOriginal)) != MH_OK) throw;
	if (MH_EnableHook(triggerComponentCtorFunctionAddr) != MH_OK) throw;

	if (MH_CreateHook(triggerComponentDtorFunctionAddr, &triggerComponentDtor, reinterpret_cast<LPVOID*>(&triggerComponentDtorOriginal)) != MH_OK) throw;
	if (MH_EnableHook(triggerComponentDtorFunctionAddr) != MH_OK) throw;

	if (MH_CreateHook(enterTriggerFunctionAddr, &enterTrigger, reinterpret_cast<LPVOID*>(&enterTriggerOriginal)) != MH_OK) throw;
	if (MH_EnableHook(enterTriggerFunctionAddr) != MH_OK) throw;

	printf("triggerComponentCtor: 0x%llx\n", (uint64_t)triggerComponentCtorFunctionAddr);
	printf("entertrigger: 0x%llx\n", (uint64_t)enterTriggerFunctionAddr);
}

void TriggerInitialize()
{
	triggerPtrs.reserve(MAX_TRIGGERS);
	for (int i = 0; i < MAX_TRIGGERS; i++)
		triggerPtrs.push_back({});
}

std::vector<coregame::TriggerComponent*> TriggerGetVector()
{
	return triggerPtrs;
}


int TriggerGetCount(void)
{
	return triggerCount;
}