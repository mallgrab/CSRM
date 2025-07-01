#include "DebugPanel.h"

// TODO: don't hook, theres an array of all the debug panels at
// ((((Control_DX11.exe + 1166ED0) + 0x30) + 0x320) + 0x8)
// DebugPanel::Controller at 0x320
// Panels at 0x8

/* Debug Panel Info
	to draw the panel call DrawMenu in one of our render loops
	interaction is done by sending mouse/key events
		idk if the controller or the panel needs to get its event called
		the devmenu dll atleast calls that virtual method
*/

DebugPanel* panel = {};

using DebugPanelCtor_t = uint64_t(__fastcall*)(uint64_t a1);
DebugPanelCtor_t DebugPanelCtorOrig;
uint64_t DebugPanelCtor(uint64_t a1)
{
	uint64_t result = DebugPanelCtorOrig(a1);
	panel = (DebugPanel*)a1;

	return result;
}