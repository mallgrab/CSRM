#pragma once
#include <chrono>
#include <iostream>
#include <enet/enet.h>
#include <map>
#include <thread>
#include <cstring>

class Client {
public:
	inline Client(int clientID) {
		ID = clientID;

		std::time_t ct = std::time(0);
		char* cc = ctime(&ct);
		connectTime = cc;
	}

	std::string nickname;
	std::string connectTime;

	unsigned int ID;
	unsigned int colour = 16777215;
	unsigned int trailColour;
	float pos[3] = { 0.0f, 0.0f, 0.0f };
	float prevPos[3] = { 0.0f, 0.0f, 0.0f };

	std::string chapter;

	bool votedCountdown = false;
private:

};

class BaseServer {
public:
	void StartServer(int port, int numClients);
	void StopServer();
	unsigned int GetClientCount();
	std::map<int, Client*> GetClientList();

	void KickClient(int ID, std::string nickname);
	virtual void SayMessage(std::string message);
private:
	bool stopServer = false;
	ENetAddress address;
	ENetHost* server;
	std::map<int, Client*> client_map;

	void SendUpdates();
	void DisconnectClient(Client* disconnectedClient);
	void HandlePacket(ENetPacket* packet, ENetPeer* peer);

	void SendCountdown();

	unsigned int GetCountdownVoteCount();
	void ResetCountdownVotes();
protected:
	void SendCustomPacket(ENetPeer* peer, char packetType, char data[64], bool reliable);
	void BroadcastCustomPacket(char packetType, char data[64], bool reliable);

	// Add custom packet parsing
	virtual inline void HandleCustomPacket(ENetPeer* peer, char packetType, char* data) { return; };

	// Receive Events
	virtual inline void OnClientRegistered(Client* client) { return; }
	virtual inline void OnClientPositionUpdate(Client* client) { return; }
	virtual inline void OnClientChapterUpdate(Client* client) { return; }
	virtual inline void OnClientCountdownVote(Client* client, bool successful) { return; }
};
