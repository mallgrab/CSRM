#pragma once

#include "kiero/kiero.h"

#include "hooks/OcularHook.h"
#if _WIN32
#include "hooks/D3D9Hook.h"
#endif
#include "hooks/D3D11Hook.h"

extern D3D11Hook oHook;

namespace Ocular {
	OcularHook Init();
	void Shutdown();
}
