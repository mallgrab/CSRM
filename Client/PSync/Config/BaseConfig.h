#pragma once
#include <iostream>
#include <string>
#include <fstream>
#define _WINSOCKAPI_
#include <Windows.h>
#include "../../ocular/imgui/imgui.h"

class BaseUI;

#include "../UI/BaseUI.h"

// I don't really like the way this config system is setup, but I'm lazy rn and I'll just throw this on the backburner for a overhaul.
// Probably could do with some macros.
class BaseConfig {
public:
	BaseConfig() {

	}

	BaseUI* ui;

	void ReadConfig();
	void WriteConfig();

	// Speedometer
	bool drawSpeedometer = false;
	bool speedometerShowTopSpeed = false;
	bool speedometerGroundSpeed = false; //show speed on jumps?
	bool speedometerColorSpeed = false;
	bool speedometerConsolePrint = false;
	int speedometerType = 0;
	float speedometerX = -1.0f;
	float speedometerY = -1.0f;
	float speedometerSize = -1.0f;

	// Trigger
	bool drawTriggers = false;
	float triggerOpacity = 0.3f;

	// Vars
	int menuKeybind = 33;
	float menuAccentColour[3] = { 55 / 255.0f, 55 / 255.0f, 55 / 255.0f };
	bool drawImGuiCursor = false;
	bool positionSmoothing = true;

	char serverIP[64] = "";
	char serverPort[6] = "";
	char nickname[32] = "";
	float myColour[3] = { 1.0f, 1.0f, 1.0f };
	float myTrailColour[3] = { 0.26f, 0.53f, 0.96f };
	bool connectOnStart = false;

	float playerNicknamePlateColour[4] = { 0.2f, 0.2f, 0.2f, 0.8f };
	float playerObjectOpacity = 0.9f;
	float playerObjectScale = 0.26f;

	bool playerTrailEnabled = true;
	float playerTrailOpacity = 0.6f;
	float playerTrailThickness = 3.0f;
	int playerTrailLength = 18;

	bool playerListEnabled = true;
	int playerListWidth = 160;

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