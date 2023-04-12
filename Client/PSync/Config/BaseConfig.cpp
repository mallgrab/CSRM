#include "BaseConfig.h"

void BaseConfig::ReadConfig() {
	std::fstream file;
	file.open(getIniFilePath(), std::fstream::in);
	if (!file.is_open()) {
		char buf[128];
		sprintf_s(buf, "[PSync] Could not load config file: %s\n", getIniFilePath().c_str());
		printf(buf);
		ui->CreateNotification(buf, ImColor(0.2f, 0.2f, 0.2f), ImColor(1.0f, 1.0f, 0.0f));
		file.close();
		return;
	}
	std::string tempStr;

	readFromIni(file, "MenuKeybind", menuKeybind);

	readFromIni(file, "MenuAccentColourR", menuAccentColour[0]);
	readFromIni(file, "MenuAccentColourG", menuAccentColour[1]);
	readFromIni(file, "MenuAccentColourB", menuAccentColour[2]);

	readFromIni(file, "DrawImGuiCursor", drawImGuiCursor);

	readFromIni(file, "PositionSmoothing", positionSmoothing);

	readFromIni(file, "ServerIP", tempStr);
	strcpy_s(serverIP, tempStr.c_str());
	readFromIni(file, "ServerPort", tempStr);
	strcpy_s(serverPort, tempStr.c_str());
	readFromIni(file, "Nickname", tempStr);
	strcpy_s(nickname, tempStr.c_str());
	readFromIni(file, "ConnectOnStart", connectOnStart);
	readFromIni(file, "AutoRetryOnStart", autoRetryOnStart);

	readFromIni(file, "MyColourR", myColour[0]);
	readFromIni(file, "MyColourG", myColour[1]);
	readFromIni(file, "MyColourB", myColour[2]);

	readFromIni(file, "MyTrailColourR", myTrailColour[0]);
	readFromIni(file, "MyTrailColourG", myTrailColour[1]);
	readFromIni(file, "MyTrailColourB", myTrailColour[2]);

	readFromIni(file, "PlayerNicknamePlateColourR", playerNicknamePlateColour[0]);
	readFromIni(file, "PlayerNicknamePlateColourG", playerNicknamePlateColour[1]);
	readFromIni(file, "PlayerNicknamePlateColourB", playerNicknamePlateColour[2]);
	readFromIni(file, "PlayerNicknamePlateColourA", playerNicknamePlateColour[3]);

	readFromIni(file, "PlayerObjectOpacity", playerObjectOpacity);
	readFromIni(file, "PlayerObjectScale", playerObjectScale);

	readFromIni(file, "PlayerTrailEnabled", playerTrailEnabled);
	readFromIni(file, "PlayerTrailOpacity", playerTrailOpacity);
	readFromIni(file, "PlayerTrailThickness", playerTrailThickness);
	readFromIni(file, "PlayerTrailLength", playerTrailLength);

	readFromIni(file, "PlayerListEnabled", playerListEnabled);
	readFromIni(file, "PlayerListWidth", playerListWidth);

	readFromIni(file, "DrawSelf", drawSelf);

	readFromIni(file, "OverrideResolution", overrideResolution);
	readFromIni(file, "CustomWidth", customWidth);
	readFromIni(file, "CustomHeight", customHeight);

	ReadExtraConfig(file);

	file.close();
	char buf[128];
	sprintf_s(buf, "[PSync] Config Loaded: %s\n", getIniFilePath().c_str());
	printf(buf);
	ui->CreateNotification(buf);
}

void BaseConfig::WriteConfig() {
	std::fstream file;
	file.open(getIniFilePath(), std::ofstream::out | std::ofstream::trunc);

	writeHeader(file, "[Menu]");
	writeToIni(file, "MenuKeybind", menuKeybind);

	writeToIni(file, "MenuAccentColourR", menuAccentColour[0]);
	writeToIni(file, "MenuAccentColourG", menuAccentColour[1]);
	writeToIni(file, "MenuAccentColourB", menuAccentColour[2]);

	writeToIni(file, "DrawImGuiCursor", drawImGuiCursor);

	writeToIni(file, "PositionSmoothing", positionSmoothing);

	writeHeader(file, "[Connection]");
	writeToIni(file, "ServerIP", std::string(serverIP));
	writeToIni(file, "ServerPort", std::string(serverPort));
	writeToIni(file, "Nickname", std::string(nickname));

	writeToIni(file, "MyColourR", myColour[0]);
	writeToIni(file, "MyColourG", myColour[1]);
	writeToIni(file, "MyColourB", myColour[2]);

	writeToIni(file, "MyTrailColourR", myTrailColour[0]);
	writeToIni(file, "MyTrailColourG", myTrailColour[1]);
	writeToIni(file, "MyTrailColourB", myTrailColour[2]);

	writeToIni(file, "ConnectOnStart", connectOnStart);
	writeToIni(file, "AutoRetryOnStart", autoRetryOnStart);

	writeHeader(file, "[Nicknames]");
	writeToIni(file, "PlayerNicknamePlateColourR", playerNicknamePlateColour[0]);
	writeToIni(file, "PlayerNicknamePlateColourG", playerNicknamePlateColour[1]);
	writeToIni(file, "PlayerNicknamePlateColourB", playerNicknamePlateColour[2]);
	writeToIni(file, "PlayerNicknamePlateColourA", playerNicknamePlateColour[3]);

	writeHeader(file, "[Objects]");
	writeToIni(file, "PlayerObjectOpacity", playerObjectOpacity);
	writeToIni(file, "PlayerObjectScale", playerObjectScale);

	writeHeader(file, "[Trails]");
	writeToIni(file, "PlayerTrailEnabled", playerTrailEnabled);
	writeToIni(file, "PlayerTrailOpacity", playerTrailOpacity);
	writeToIni(file, "PlayerTrailThickness", playerTrailThickness);
	writeToIni(file, "PlayerTrailLength", playerTrailLength);

	writeHeader(file, "[Player List]");
	writeToIni(file, "PlayerListEnabled", playerListEnabled);
	writeToIni(file, "PlayerListWidth", playerListWidth);

	writeHeader(file, "[Other]");
	writeToIni(file, "DrawSelf", drawSelf);

	writeToIni(file, "OverrideResolution", overrideResolution);
	writeToIni(file, "CustomWidth", customWidth);
	writeToIni(file, "CustomHeight", customHeight);

	WriteExtraConfig(file);

	file.close();
	char buf[128];
	sprintf_s(buf, "[PSync] Config Saved: %s\n", getIniFilePath().c_str());
	printf(buf);
	ui->CreateNotification(buf);
}

void BaseConfig::ReadExtraConfig(std::fstream& file) {

}

void BaseConfig::WriteExtraConfig(std::fstream& file) {

}


std::string BaseConfig::getIniFilePath()
{
	char modPath[MAX_PATH];
	DWORD result = GetModuleFileNameA(nullptr, modPath, MAX_PATH);

	std::string iniString = std::string(modPath);
	size_t i = iniString.find_last_of("/\\");
	std::string iniDir = iniString.substr(0, i + 1);
	std::string iniPathString = iniDir + "PSync.ini";

	std::fstream fs;
	fs.open(iniPathString, std::fstream::in | std::fstream::out);
	fs.close();

	return iniPathString;
}

void BaseConfig::writeToIni(std::fstream& file, std::string varName, int value)
{
	file << varName << " " << std::to_string(value) << std::endl;
}

void BaseConfig::readFromIni(std::fstream& file, std::string varName, int& variable)
{
	std::string var;
	std::string val;

	file.clear();
	file.seekg(0);
	while (file >> var >> val) {
		if (var == varName) {
			variable = std::stoi(val);
			return;
		}
	}
}

// Float
void BaseConfig::writeToIni(std::fstream& file, std::string varName, float value)
{
	file << varName << " " << std::to_string(value) << std::endl;
}

void BaseConfig::readFromIni(std::fstream& file, std::string varName, float& variable)
{
	std::string var;
	std::string val;

	file.clear();
	file.seekg(0);
	while (file >> var >> val) {
		if (var == varName) {
			variable = std::stof(val);
			return;
		}
	}
}


// String
void BaseConfig::writeToIni(std::fstream& file, std::string varName, std::string value)
{
	for (std::string::iterator it = value.begin(); it != value.end(); ++it) {
		if (*it == ' ') {
			*it = '_';
		}
	}
	if (value.length() == 0) value = "NULL";
	file << varName << " " << value << std::endl;
}

void BaseConfig::readFromIni(std::fstream& file, std::string varName, std::string& variable)
{
	std::string var;
	std::string val;

	file.clear();
	file.seekg(0);
	while (file >> var >> val) {
		if (var == varName) {
			if (val == "NULL") {
				variable = "";
			}
			else {
				variable = val;
			}
			return;
		}
	}
}

// Bool
void BaseConfig::writeToIni(std::fstream& file, std::string varName, bool value)
{
	file << varName << " " << value << std::endl;
}

void BaseConfig::readFromIni(std::fstream& file, std::string varName, bool& variable)
{
	std::string var;
	std::string val;

	file.clear();
	file.seekg(0);
	while (file >> var >> val) {
		if (var == varName) {
			if (val == "1") {
				variable = true;
			}
			else {
				variable = false;
			}
			return;
		}
	}
}

// Utils
void BaseConfig::writeHeader(std::fstream& file, std::string header) {
	file << ">> " << header << std::endl;
}