#pragma once
#define _WINSOCKAPI_
#include "BaseGameData.h"
#include "../Config/NieRAutomataConfig.h"
#include "Windows.h"

class NieRAutomataGameData : public BaseGameData {
public:
	NieRAutomataConfig* config;

	inline void InitGameData() override {
		uint64_t processStartAddr = reinterpret_cast<uint64_t>(GetModuleHandle(nullptr));

		if (!config->oldPatch) {
			ptrPlayerPositionAddr = reinterpret_cast<ptr*>(processStartAddr + 0x1020948);
			ptrViewMatrixAddr = reinterpret_cast<ptr*>(processStartAddr + 0x11553B0);
		}
		else {
			ptrPlayerPositionAddr = reinterpret_cast<ptr*>(processStartAddr + 0x16053E8);
			ptrViewMatrixAddr = reinterpret_cast<ptr*>(processStartAddr + 0x19C73C0);
		}
		
	}
	
	inline Vector3* GetPlayerPos() override {
		if (*ptrPlayerPositionAddr != NULL) {
			playerPos = *reinterpret_cast<Vector3*>(*ptrPlayerPositionAddr + 0x50);
			return &playerPos;
		}
		return nullptr;
	}

	inline Matrix4* GetViewMatrix() override {
		if (*ptrViewMatrixAddr != NULL) {
			viewMatrix = *reinterpret_cast<Matrix4*>(ptrViewMatrixAddr);
			return &viewMatrix;
		}
		return nullptr;
	}
};