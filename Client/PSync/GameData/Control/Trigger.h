#pragma once
#include "../Utils.h"
#include "../BaseGameData.h"
#include "../ocular/kiero/minhook/include/MinHook.h"
#include "RemedyLib.h"

#define MAX_TRIGGERS 4096 //1024 //8192

extern std::vector<coregame::TriggerComponent*> triggerPtrs;
extern int triggerCount;

using triggerComponentCtorOriginal_t = void* (__fastcall*)(coregame::TriggerComponent* x, uint64_t* y);
extern triggerComponentCtorOriginal_t triggerComponentCtorOriginal;

using triggerComponentDtorOriginal_t = void* (__fastcall*)(coregame::TriggerComponent* x);
extern triggerComponentDtorOriginal_t triggerComponentDtorOriginal;

using enterTriggerOriginal_t = char(__fastcall*)(uint64_t* x, uint64_t* y, uint64_t z);
extern enterTriggerOriginal_t enterTriggerOriginal;

void* __fastcall triggerComponentCtor(coregame::TriggerComponent* pointer, uint64_t* pointer_b);
void* __fastcall triggerComponentDtor(coregame::TriggerComponent* pointer);
char __fastcall enterTrigger(uint64_t* pointer, uint64_t* pointer_b, uint64_t a3);

void TriggerInstallHooks(uint64_t coregameDllAddr);
void TriggerInitialize();
std::vector<coregame::TriggerComponent*> TriggerGetVector();
int TriggerGetCount(void);