#pragma once
#define _WINSOCKAPI_
#include "BaseGameData.h"
#include "Windows.h"

class DarkSouls3GameData : public BaseGameData {
public:
	inline void InitGameData() override {
		uint64_t processStartAddr = reinterpret_cast<uint64_t>(GetModuleHandle(nullptr));
		ptrPlayerPositionAddr = reinterpret_cast<ptr*>(processStartAddr + 0x4768E78);
		ptrViewMatrixAddr = reinterpret_cast<ptr*>(processStartAddr + 0x477F7D8);		// Not the actual pointer, going to another online world (like invading) destroys the pointers
	}

	inline Vector3* GetPlayerPos() override {
		if (*ptrPlayerPositionAddr != NULL) {
			ptr* p1 = reinterpret_cast<ptr*>(*ptrPlayerPositionAddr + 0x40);
			if (*p1 == NULL) return nullptr;
			ptr* p2 = reinterpret_cast<ptr*>(*p1 + 0x28);
			if (*p2 == NULL) return nullptr;
			playerPos = *reinterpret_cast<Vector3*>(*p2 + 0x80);
			return &playerPos;
		}
		return nullptr;
	}

	inline Matrix4* GetViewMatrix() override {
		if (*ptrViewMatrixAddr != NULL) {
			uint64_t* p1 = reinterpret_cast<uint64_t*>(*ptrViewMatrixAddr + 0x620);
			if (*p1 == NULL) return nullptr;
			viewMatrix = *reinterpret_cast<Matrix4*>(*p1 + 0x2C0 + 0x60);
			return &viewMatrix;
		}
		return nullptr;
	}
};