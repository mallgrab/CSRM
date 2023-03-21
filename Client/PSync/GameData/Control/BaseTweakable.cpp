#include "BaseTweakable.h"

GetTweakable_t GetTweakableFunc;
SetTweakable_t SetTweakableFunc;
GetTweakableStrValue_t GetTweakableStrValueFunc;

BaseTweakable IntroScreenTweakable;
BaseTweakable CoherentGTLiveViewsTweakable;
BaseTweakable SSAATweakable;
BaseTweakable MotionBlurTweakable;
BaseTweakable AbilityLevitateTweakable;

void BaseTweakableInstallHooks(uint64_t rlDllAddr)
{
	GetTweakableFunc = (GetTweakable_t)reinterpret_cast<ptr*>(rlDllAddr + 0x1f8e60);
	SetTweakableFunc = (SetTweakable_t)reinterpret_cast<ptr*>(rlDllAddr + 0x1f9200);
	GetTweakableStrValueFunc = (GetTweakableStrValue_t)reinterpret_cast<ptr*>(rlDllAddr + 0x1f8ee0);
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
	MotionBlurTweakable.SetTweakableStrValue("0.0f");

	AbilityLevitateTweakable = BaseTweakable("Ability Levitate: Disable");
}