#pragma once
#define _WINSOCKAPI_
#include "BaseGameData.h"
#include "Windows.h"

class DevilMayCry3SEGameData : public BaseGameData {
private:
	float fov;

public:
	inline void InitGameData() override {
		uint64_t processStartAddr = reinterpret_cast<uint64_t>(GetModuleHandle(nullptr));
		ptrPlayerPositionAddr = reinterpret_cast<ptr*>(processStartAddr + 0x188A600 + 0x4C);
		ptrViewMatrixAddr = reinterpret_cast<ptr*>(processStartAddr + 0x76BAAC);
	}

	inline Vector3* GetPlayerPos() override {
		if (ptrPlayerPositionAddr != NULL && *ptrPlayerPositionAddr != 0xFFFFFFFF) {
			playerPos = *reinterpret_cast<Vector3*>(ptrPlayerPositionAddr);
			return &playerPos;
		}
		return nullptr;
	}

	inline Matrix4* GetViewMatrix() override {
		if (*ptrViewMatrixAddr != NULL) {
			viewMatrix = *reinterpret_cast<Matrix4*>(*ptrViewMatrixAddr + 0x1C);
			return &viewMatrix;
		}
		return nullptr;
	}

	inline float* GetFOV() {
		if (*ptrViewMatrixAddr != NULL) {
			fov = *reinterpret_cast<float*>(*ptrViewMatrixAddr + 0x14);
			return &fov;
		}
		return nullptr;
	}
};