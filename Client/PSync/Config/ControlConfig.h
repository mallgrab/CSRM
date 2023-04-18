#pragma once
#include "BaseConfig.h"

//CSRM
class ControlConfig : public BaseConfig {
public:
	// Triggers
	bool drawTriggers = false;
	bool drawTriggersHotkey = true;
	float triggerOpacity = 0.42f;

	// Save/Load position
	bool saveLoadPosition = true;

	bool drawFPS = false;
	bool showPos = false;

	// Speedometer
	bool drawSpeedometer = false;
	bool speedometerShowTopSpeed = false;
	bool speedometerGroundSpeed = true; //show speed on jumps?
	bool speedometerColorSpeed = true;
	bool speedometerConsolePrint = false;
	int speedometerType = 0;
	//float speedometerX = -1.0f, speedometerY = -1.0f;
	float speedometerPos[2] = { -1.0f, -1.0f };
	float speedometerSize = -1.0f;

	// Tweakables..
	bool motionBlur = true; //original game has this on
	bool TemporalSSAA = false; //original game would have this enabled, but i hate it!!!
	bool disableLevitation = false;

protected:
	inline void ReadExtraConfig(std::fstream& file) {
		//readFromIni(file, "drawTriggers", drawTriggers);
		readFromIni(file, "drawTriggersHotkey", drawTriggersHotkey);
		readFromIni(file, "triggerOpacity", triggerOpacity);

		readFromIni(file, "saveLoadPosition", saveLoadPosition);

		readFromIni(file, "drawFPS", drawFPS);
		//readFromIni(file, "showPos", showPos);

		readFromIni(file, "drawSpeedometer", drawSpeedometer); //causing crashes when enabled on startup rn
		readFromIni(file, "speedometerShowTopSpeed", speedometerShowTopSpeed);
		readFromIni(file, "speedometerGroundSpeed", speedometerGroundSpeed);
		readFromIni(file, "speedometerColorSpeed", speedometerColorSpeed);
		//readFromIni(file, "speedometerConsolePrint", speedometerConsolePrint);
		//readFromIni(file, "speedometerType", speedometerType);
	}

	inline void WriteExtraConfig(std::fstream& file) {
		//writeToIni(file, "drawTriggers", drawTriggers);
		writeToIni(file, "drawTriggersHotkey", drawTriggersHotkey);
		writeToIni(file, "triggerOpacity", triggerOpacity);

		writeToIni(file, "saveLoadPosition", saveLoadPosition);

		writeToIni(file, "drawFPS", drawFPS);
		//writeToIni(file, "showPos", showPos);

		writeToIni(file, "drawSpeedometer", drawSpeedometer);
		writeToIni(file, "speedometerShowTopSpeed", speedometerShowTopSpeed);
		writeToIni(file, "speedometerGroundSpeed", speedometerGroundSpeed);
		writeToIni(file, "speedometerColorSpeed", speedometerColorSpeed);
		//writeToIni(file, "speedometerConsolePrint", speedometerConsolePrint);
		//writeToIni(file, "speedometerType", speedometerType);
	}

};