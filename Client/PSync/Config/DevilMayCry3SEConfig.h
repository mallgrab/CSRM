#pragma once
#include "BaseConfig.h"

class DevilMayCry3SEConfig : public BaseConfig {
public:
	DevilMayCry3SEConfig() {
		playerObjectScale = 25.0f;
	}

	bool loadStyleSwitcher = false;

protected:
	inline void ReadExtraConfig(std::fstream& file) {
		readFromIni(file, "LoadStyleSwitcher", loadStyleSwitcher);

	}

	inline void WriteExtraConfig(std::fstream& file) {
		writeToIni(file, "LoadStyleSwitcher", loadStyleSwitcher);
	}
};