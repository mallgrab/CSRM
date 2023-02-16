#pragma once
#include "../Utils.h"
#include "../Config/BaseConfig.h"
#include <vector>

#if _WIN32 || _WIN64
#if _WIN64
typedef uint64_t ptr;
#else
typedef uint32_t ptr;
#define ENVIRONMENT32
#endif
#endif

class BaseGameData {
protected:
	ptr* ptrPlayerPositionAddr = nullptr;
	ptr* ptrViewMatrixAddr = nullptr;

	Vector3 playerPos;
	Matrix4 viewMatrix;
public:
	BaseConfig* config;

	virtual inline void InitGameData() {};
	virtual inline Vector3* GetPlayerPos() { return nullptr; };
	virtual inline Matrix4* GetViewMatrix() { return nullptr; };

	bool uiToggle = true;
};