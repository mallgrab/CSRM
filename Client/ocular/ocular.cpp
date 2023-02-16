#include "ocular.h"

OcularHook Ocular::Init() {
	bool hooked = false;
	OcularHook oHook;

	while (!oHook.hooked) {
		//std::cout << "[Ocular] Initializing kiero..." << std::endl;
		kiero::Status::Enum kiero_init = kiero::init(kiero::RenderType::Auto);
		if (kiero_init == kiero::Status::Success) {
			switch (kiero::getRenderType())
			{
			case kiero::RenderType::D3D9:
				oHook = D3D9Hook();
				std::cout << "[Ocular] Kiero initialized using D3D9!" << std::endl;
				break;
			case kiero::RenderType::D3D11:
				oHook = D3D11Hook();
				std::cout << "[Ocular] Kiero initialized using D3D11!" << std::endl;
				break;
			}
		}
		else {
			std::cout << "[Ocular] Kiero failed to initialize: " << kiero_init << std::endl;
		}
	}

	return oHook;
}

void Ocular::Shutdown() {
	kiero::shutdown();
}