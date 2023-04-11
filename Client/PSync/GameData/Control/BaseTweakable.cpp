#include "BaseTweakable.h"

GetTweakable_t GetTweakableFunc;
SetTweakable_t SetTweakableFunc;
GetTweakableStrValue_t GetTweakableStrValueFunc;

BaseTweakable IntroScreenTweakable;
BaseTweakable CoherentGTLiveViewsTweakable;
BaseTweakable SSAATweakable;
BaseTweakable MotionBlurTweakable;
BaseTweakable AbilityLevitateTweakable;

void BaseTweakableInstallHooks(LPCWSTR dllName)
{
	HINSTANCE module = GetModuleHandle(dllName);

	if (module != NULL)
	{
		GetTweakableFunc = (GetTweakable_t)GetProcAddress(module, "?getTweakable@BaseTweakable@d@@SAPEAV12@PEBD@Z");
		SetTweakableFunc = (SetTweakable_t)GetProcAddress(module, "?setTweakable@BaseTweakable@d@@SA_NPEBD0_N@Z");
		GetTweakableStrValueFunc = (GetTweakableStrValue_t)GetProcAddress(module, "?getTweakableStrValue@BaseTweakable@d@@SA?AV?$InplaceString@$0IA@@r@@PEBD_N@Z");
	}
}

void BaseTweakableInitialize()
{
	IntroScreenTweakable = BaseTweakable("Intro Screen:Display time for section"); // set 0.0f (default 4.0f)
	IntroScreenTweakable.SetTweakableStrValue("0.0f");

	CoherentGTLiveViewsTweakable = BaseTweakable("CoherentGT:Force live views to update"); // set to 1 (default 0)
	CoherentGTLiveViewsTweakable.SetTweakableStrValue("1");

	SSAATweakable = BaseTweakable("SSAA:Temporal Jitter Scale"); // set to 0.0f (default 1.0f)
	SSAATweakable.SetTweakableStrValue("0.0f");

	MotionBlurTweakable = BaseTweakable("VectorBlur:Shutter Speed"); // set to 0.0f; (default 0.4f)
	//MotionBlurTweakable.SetTweakableStrValue("0.0f"); //leave this at default for now, since it affects how the game feels

	AbilityLevitateTweakable = BaseTweakable("Ability Levitate: Disable");
}