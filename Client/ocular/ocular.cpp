#include "ocular.h"
D3D11Hook oHook;

OcularHook Ocular::Init() {
	bool hooked = false;

	while (!oHook.hooked) {
		kiero::Status::Enum kiero_init = kiero::init(kiero::RenderType::Auto);
		if (kiero_init == kiero::Status::Success)
			oHook = D3D11Hook();
		else
			std::cout << "[Ocular] Kiero failed to initialize: " << kiero_init << std::endl;
	}

	return oHook;
}

void Ocular::Shutdown() {
	kiero::shutdown();
}