#pragma once
#include <iostream>
#include <string>
#include <fstream>
#define _WINSOCKAPI_
#include <Windows.h>
#include "../../ocular/imgui/imgui.h"

class BaseUI;

#include "../UI/BaseUI.h"

typedef enum listPosition_e {
	PLAYERLIST_TOPLEFT = 0,
	PLAYERLIST_TOPRIGHT,
	PLAYERLIST_BOTTOMLEFT,
	PLAYERLIST_BOTTOMRIGHT
} listPosition_t;

// I don't really like the way this config system is setup, but I'm lazy rn and I'll just throw this on the backburner for a overhaul.
// Probably could do with some macros.
class BaseConfig {
public:
	BaseConfig() {

	}

	BaseUI* ui;

	void ReadConfig();
	void WriteConfig();

	// Vars
	//int menuKeybind = VK_PRIOR; //0x21; //pageup
	int menuKeybind = VK_HOME; //0x24; //home
	//float menuAccentColour[3] = { 215 / 255.0f, 65 / 255.0f, 167 / 255.0f };
	float menuAccentColour[3] = { 55 / 255.0f, 55 / 255.0f, 55 / 255.0f };
	bool drawImGuiCursor = false;
	bool positionSmoothing = true;

	char serverIP[64] = "";
	char serverPort[6] = "";
	char nickname[32] = "";
	float myColour[3] = { 1.0f, 1.0f, 1.0f };
	float myTrailColour[3] = { 0.26f, 0.53f, 0.96f };

	bool connectOnStart = false;
	bool autoRetryOnStart = true; //retry with autoconnect enabled

	float playerNicknamePlateColour[4] = { 0.2f, 0.2f, 0.2f, 0.8f };
	float playerObjectOpacity = 0.9f;
	float playerObjectScale = 0.26f;

	bool playerTrailEnabled = true;
	float playerTrailOpacity = 0.6f;
	float playerTrailThickness = 3.0f;
	int playerTrailLength = 18;

	bool playerListEnabled = true;
	int playerListWidth = 0; //160;
	int playerListPos = PLAYERLIST_TOPRIGHT;

	bool drawSelf = false;

	bool overrideResolution = false;
	float customWidth = 0.0f;
	float customHeight = 0.0f;
protected:
	virtual void ReadExtraConfig(std::fstream& file);
	virtual void WriteExtraConfig(std::fstream& file);

	std::string getIniFilePath();

	// Read
	void readFromIni(std::fstream& file, std::string varName, int& variable);
	void readFromIni(std::fstream& file, std::string varName, float& variable);
	void readFromIni(std::fstream& file, std::string varName, std::string& variable);
	void readFromIni(std::fstream& file, std::string varName, bool& variable);

	// Write
	void writeToIni(std::fstream& file, std::string varName, int value);
	void writeToIni(std::fstream& file, std::string varName, float value);
	void writeToIni(std::fstream& file, std::string varName, std::string value);
	void writeToIni(std::fstream& file, std::string varName, bool value);

	// Utils
	void writeHeader(std::fstream& file, std::string header);
};