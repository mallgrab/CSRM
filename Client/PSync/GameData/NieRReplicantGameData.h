#pragma once
#define _WINSOCKAPI_
#include "BaseGameData.h"
#include "Windows.h"

class NieRReplicantGameData : public BaseGameData {
public:
	inline void InitGameData() override {
		uint64_t processStartAddr = reinterpret_cast<uint64_t>(GetModuleHandle(nullptr));
		ptrPlayerPositionAddr = reinterpret_cast<ptr*>(processStartAddr + 0x26F9560);
		ptrViewMatrixAddr = reinterpret_cast<ptr*>(processStartAddr + 0x26DBAC0);
	}

	inline Vector3* GetPlayerPos() override {
		if (*ptrPlayerPositionAddr != NULL) {
			playerPos.x = *reinterpret_cast<float*>(*ptrPlayerPositionAddr + 0x9C);
			playerPos.y = *reinterpret_cast<float*>(*ptrPlayerPositionAddr + 0xAC);
			playerPos.z = *reinterpret_cast<float*>(*ptrPlayerPositionAddr + 0xBC);
			return &playerPos;
		}
		return nullptr;
	}

	inline Matrix4* GetViewMatrix() override {
		viewMatrix = *reinterpret_cast<Matrix4*>(ptrViewMatrixAddr);    // Static Address
		return &viewMatrix;
	}
};