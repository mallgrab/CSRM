#pragma once
#include "BaseConfig.h"

class NieRAutomataConfig : public BaseConfig {
public:
	bool oldPatch = false;

protected:
	inline void ReadExtraConfig(std::fstream& file) {
		readFromIni(file, "OldPatch", oldPatch);

	}

	inline void WriteExtraConfig(std::fstream& file) {
		writeToIni(file, "OldPatch", oldPatch);
	}

};