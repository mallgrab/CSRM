#pragma once
#include "../Utils.h"
#include "../BaseGameData.h"

struct BaseTweakable;

using GetTweakable_t = BaseTweakable* (__stdcall*)(const char* name);
extern GetTweakable_t GetTweakableFunc;

using SetTweakable_t = int64_t(__fastcall*)(char* tweakableName, const char* value, char isInitialized);
extern SetTweakable_t SetTweakableFunc;

using GetTweakableStrValue_t = char** (__fastcall*)(char* rString, char* tweakableName, char formated);
extern GetTweakableStrValue_t GetTweakableStrValueFunc;

struct BaseTweakable_vtbl
{
	virtual void* BaseTweakableDtor(char a2) = 0;
	virtual bool InitialTweakable() = 0;
};

struct BaseTweakable
{
	BaseTweakable_vtbl* vtable;
	char* name = nullptr;
	char* name2 = nullptr;
	int32_t field_10 = 0;
	int32_t field_14 = 0;
	int32_t field_18 = 0;
	int32_t field_22 = 0;
	int8_t strBuffer[128] = {0};
	int type = 0;
	char isChanged = 0;
	int8_t gap3[3] = {0};
	int32_t initialValues[2] = {0};
	char* tmp = nullptr;

	BaseTweakable() {}

	BaseTweakable(const char* name)
	{
		BaseTweakable* tweakable = GetTweakableFunc(name);

		if (tweakable == nullptr)
		{
			printf("Tweakable name %s was not found!\n", name);
			return;
		}

		*this = *tweakable;
		this->tmp = (char*)malloc(sizeof(char) * 8); // we are leaking memory, not my problem.
	}

	char* GetTweakableStrValue()
	{
		if (this->name == nullptr)
			return nullptr;

		GetTweakableStrValueFunc(tmp, (char*)strBuffer, 0);
		return *(char**)tmp;
	}

	// Changing the tweakable value does not update the tmp pointer.
	// Putting GetTweakableStrValue after the SetTweakable Function call caused a race condition when freeing memory.
	void SetTweakableStrValue(const char* value)
	{
		if (this->name == nullptr)
			return;

		SetTweakableFunc((char*)strBuffer, value, 1);
	}
};

extern BaseTweakable IntroScreenTweakable;
extern BaseTweakable CoherentGTLiveViewsTweakable;
extern BaseTweakable SSAATweakable;
extern BaseTweakable MotionBlurTweakable;
extern BaseTweakable AbilityLevitateTweakable;

void BaseTweakableInstallHooks(LPCWSTR dllName);
void BaseTweakableInitialize();