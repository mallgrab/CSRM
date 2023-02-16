#pragma once
#define _WINSOCKAPI_
#include <iostream>
#include <ctime>
#include <map>
#include <deque>
#include <thread>
#include <enet/enet.h>

#include "../../Utils.h"
#include "BaseUser.h"

class BaseGameData;
class BaseConfig;
class BaseUI;

#include "../Config/BaseConfig.h"
#include "../GameData/BaseGameData.h"

class BaseClient {
public:
	BaseGameData* data;
	BaseConfig* config;
	BaseUI* ui;

	int localID = -1;
	std::string status = "Disconnected";

	void StartClient(char* serverIP, int port, char* nickname, unsigned int playerColour, unsigned int playerTrailColour);
	void StopClient();
	void ResetClient();
	unsigned int GetUserCount();
	std::map<int, BaseUser*> GetUserList();
	bool IsRunning() { return clientRunning; };
	unsigned int GetPing();

	void VoteCountdown();
	virtual void SendChapter(const char* chapter);

private:
	std::thread* clientMainThread = nullptr;
	std::thread* clientSendThread = nullptr;

	bool stopClient = true;
	bool clientRunning = false;
	ENetAddress address;
	ENetHost* client;
	ENetPeer* peer;
	std::map<int, BaseUser*> client_map;

protected:
	virtual void HandlePacket(ENetPacket* packet);
	virtual void SendUpdate();

	// Add custom packet parsing
	virtual inline void HandleCustomPacket(char packetType, char* data) { return; };

	// Receive Events
	virtual inline void OnUserConnect(BaseUser* newUser) { return; }
	virtual inline void OnUserDisconnect(std::string disconnectedNick, unsigned int disconnectedID) { return; }
	virtual inline void OnPositionUpdate(BaseUser* user) { return; }
	virtual inline void OnChapterUpdate(BaseUser* user) { return; }
	virtual inline void OnServerMessage(std::string message) { return; }
	virtual inline void OnCountdownStarted(unsigned int countdownStartTime) { return; }
};