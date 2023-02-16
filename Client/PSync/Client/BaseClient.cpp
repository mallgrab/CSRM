#include "BaseClient.h"

#define TICK_RATE 40

auto lastUpdateTime = std::chrono::high_resolution_clock::now();
Vector3 prevPos = { 0.0f, 0.0f, 0.0f };

void BaseClient::StopClient() {
	printf("[PSync] Shutting down PSync client...\n");
	ui->CreateNotification(const_cast<char*>("[PSync] Shutting down PSync client..."));
	stopClient = true;
}

void BaseClient::ResetClient() {
	stopClient = true;
	clientRunning = false;
	localID = -1;
	status = "Disconnected";

	for (const auto& client_it : client_map) {
		delete client_it.second;
	}

	client_map.clear();
}

void BaseClient::StartClient(char* serverIP, int port, char* nickname, unsigned int playerColour, unsigned int playerTrailColour) {
	if (clientRunning) {
		printf("[PSync] Client already running!\n");
		ui->CreateNotification(const_cast<char*>("[PSync] Client already running!"), 4.0f, 0.1f, 0.5f, ImColor(0.2f, 0.2f, 0.2f), ImColor(1.0f, 1.0f, 0.0f));
		return;
	}

	clientRunning = true;
	status = "Starting Client";
	clientMainThread = new std::thread([this, serverIP, port, nickname, playerColour, playerTrailColour]() {
		if (enet_initialize() != 0) {
			fprintf(stderr, "[PSync] An error occurred while initializing ENet.\n");
			ResetClient();
			throw stderr;
		}
		atexit(enet_deinitialize);

		client = enet_host_create(NULL, 1, 1, 0, 0);
		if (client == NULL) {
			printf("[PSync] An error occurred while trying to create an ENet client host.\n");
			ResetClient();
			throw 1;
		}

		printf("[PSync] PSync client started...\n");

		enet_address_set_host(&address, serverIP);
		address.port = port;

		peer = enet_host_connect(client, &address, 1, 0);
		if (peer == NULL) {
			fprintf(stderr, "[PSync] No available peers for initiating an ENet connection!\n");
			ResetClient();
			return;
		}

		ENetEvent event;
		printf("[PSync] Attempting connection to server...\n");
		ui->CreateNotification(const_cast<char*>("[PSync] Attempting connection to server..."));
		status = "Connecting...";
		if (enet_host_service(client, &event, 5000) > 0 && event.type == ENET_EVENT_TYPE_CONNECT) // Wait 5 seconds for connect event to confirm we are connected.
		{
			char buf[128];
			sprintf_s(buf, "[PSync] Connected to %s:%i, awaiting registration.\n", serverIP, port);
			printf(buf);
			ui->CreateNotification(buf);

			enet_packet_destroy(event.packet);
		}
		else {
			char buf[128];
			sprintf_s(buf, "[PSync] Could not connect to %s:%i.\n", serverIP, port);
			printf(buf);
			ui->CreateNotification(buf, 4.0f, 0.1f, 0.5f, ImColor(0.2f, 0.2f, 0.2f), ImColor(1.0f, 0.0f, 0.0f));

			enet_peer_reset(peer);
			ResetClient();
			return;
		}
		enet_host_flush(client); // Flush the buffer to make sure the connection packet is sent.

		// Wait for ID and Nickname confirmation
		status = "Registering...";
		while (enet_host_service(client, &event, 5000) > 0) { // Wait 5 seconds for our registration to be setup and confirmed.
			if (event.type != ENET_EVENT_TYPE_RECEIVE) {
				enet_packet_destroy(event.packet);
				continue;
			}

			char* data = (char*)event.packet->data;
			char packetType = data[0];

			if (packetType == '0') {	// We have received our ID.
				memcpy(&localID, data + 1, sizeof(localID));
				printf("[PSync] ID %i assigned by server.\n", localID);

				// Send our nickname and colours
				unsigned int offset = 0;
				char packetData[128];
				packetData[0] = '1';														// Set packet type
				offset++;
				memcpy(packetData + offset, &localID, sizeof(localID));							// Add our ID
				offset += sizeof(localID);
				memcpy(packetData + offset, nickname, strlen(nickname) + 1);						// Add our name
				offset += strlen(nickname) + 1;
				memcpy(packetData + offset, &playerColour, sizeof(playerColour));				// Add our player colour
				offset += sizeof(playerColour);
				memcpy(packetData + offset, &playerTrailColour, sizeof(playerTrailColour));		// Add our trail colour
				offset += sizeof(playerTrailColour);

				ENetPacket* packet = enet_packet_create(packetData, sizeof(packetData), ENET_PACKET_FLAG_RELIABLE);
				enet_peer_send(peer, 0, packet);
			}
			else if (packetType == '2') {
				unsigned int receivedID;
				memcpy(&receivedID, data + 1, sizeof(receivedID));

				if (receivedID != localID) continue;		// If this isn't our connection announcement, check the next packet.
				stopClient = false;
				printf("[PSync] Nickname successfully registered.\n");
				ui->CreateNotification(const_cast<char*>("[PSync] Registration successful. Connection complete."), ImColor(0.2f, 0.2f, 0.2f), ImColor(0.0f, 1.0f, 0.0f));
				break;
			}

			enet_packet_destroy(event.packet);
		}


		if (stopClient) {
			if (localID == -1) {
				printf("[PSync] Registration failed due to server not assigning ID.\n");
				ui->CreateNotification(const_cast<char*>("[PSync] Registration failed due to server not assigning ID."), 4.0f, 0.1f, 0.5f, ImColor(0.2f, 0.2f, 0.2f), ImColor(1.0f, 0.0f, 0.0f));
			}
			else {
				printf("[PSync] Registration failed due to nickname being rejected.\n");
				ui->CreateNotification(const_cast<char*>("[PSync] Registration failed due to nickname being rejected."), 4.0f, 0.1f, 0.5f, ImColor(0.2f, 0.2f, 0.2f), ImColor(1.0f, 0.0f, 0.0f));
			}
		}

		status = "Connected";

		// Start send thread
		/*
		clientSendThread = new std::thread([]() {
			Vector3 prevPos = { 0.0f, 0.0f, 0.0f };
			while (!stopClient) {
				std::this_thread::sleep_for(std::chrono::milliseconds(TICK_RATE));
				// Send position
				Vector3* playerPos = Globals::g_PSyncGamedata->GetPlayerPos();
				if (playerPos == nullptr) continue;
				if (*playerPos == prevPos) continue;
				prevPos = *playerPos;

				char packetData[64];
				size_t offset = 0;
				packetData[0] = '4';												// Set packet type
				offset++;
				memcpy(packetData + offset, &localID, sizeof(localID));				// Add our ID
				offset += sizeof(localID);
				memcpy(packetData + offset, &playerPos->v, sizeof(playerPos->v));	// Add X position
				offset += sizeof(playerPos->v);

				ENetPacket* packet = enet_packet_create(packetData, sizeof(packetData), 0);
				enet_peer_send(peer, 0, packet);
				std::cout << std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count() << " SEND: ";
				for (int i = 0; i < offset; i++) printf("%02x ", packetData[i]);
				printf("\n");
			}
		});
		*/
		lastUpdateTime = std::chrono::high_resolution_clock::now();

		// Start listening
		while (!stopClient) {
			// Send
			SendUpdate();
			// Receive
			ENetEvent event;
			while (enet_host_service(client, &event, TICK_RATE) > 0) {
				switch (event.type) {
				case ENET_EVENT_TYPE_RECEIVE:
				{
					// Parse packets
					HandlePacket(event.packet);
					break;
				}
				case ENET_EVENT_TYPE_DISCONNECT:
				{
					localID = -1;
					stopClient = true;

					printf("[PSync] Connection closed by server.\n");
					ui->CreateNotification(const_cast<char*>("[PSync] Connection closed by server."));
					break;
				}
				}
				enet_packet_destroy(event.packet);
				if (stopClient) break;
			}
		}

		//clientSendThread->join();

		status = "Disconnecting...";
		printf("[PSync] Disconnecting from server...\n");
		enet_peer_disconnect(peer, 0);
		if (enet_host_service(client, &event, 3000) > 0)
		{
			if (event.type == ENET_EVENT_TYPE_RECEIVE) {
				enet_packet_destroy(event.packet);
			}
			else if (event.type == ENET_EVENT_TYPE_DISCONNECT) {
				printf("[PSync] Disconnection confirmed by server.\n");
			}
		}

		enet_peer_reset(peer);
		printf("[PSync] Disconnected.\n");
		ui->CreateNotification(const_cast<char*>("[PSync] Disconnected."));

		ResetClient();

		clientRunning = false;
		}); // End of thread
}

unsigned int BaseClient::GetUserCount() {
	return (unsigned int)client_map.size();
}

std::map<int, BaseUser*> BaseClient::GetUserList() {
	return client_map;
}

void BaseClient::SendUpdate() {
	long long timeSinceUpdate = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - lastUpdateTime).count();
	if (timeSinceUpdate >= TICK_RATE) {
		// Send position
		Vector3* playerPos = data->GetPlayerPos();
		if (playerPos == nullptr) return;
		if (*playerPos == prevPos) return;
		prevPos = *playerPos;

		char packetData[64];
		size_t offset = 0;
		packetData[0] = '4';												// Set packet type
		offset++;
		memcpy(packetData + offset, &localID, sizeof(localID));				// Add our ID
		offset += sizeof(localID);
		memcpy(packetData + offset, &playerPos->v, sizeof(playerPos->v));	// Add X position
		offset += sizeof(playerPos->v);

		ENetPacket* packet = enet_packet_create(packetData, sizeof(packetData), 0);
		enet_peer_send(peer, 0, packet);
		//std::cout << std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count() << " SEND: ";
		//for (int i = 0; i < offset; i++) printf("%02x ", packetData[i]);
		//printf("\n");
		lastUpdateTime = std::chrono::high_resolution_clock::now();
	}
}

void BaseClient::SendChapter(const char* chapter) {
	char packetData[64];
	size_t offset = 0;
	packetData[0] = '5';												// Set packet type
	offset++;
	memcpy(packetData + offset, &localID, sizeof(localID));				// Add our ID
	offset += sizeof(localID);

	memcpy(packetData + offset, chapter, strlen(chapter) + 1);		// Add our chapter
	offset += strlen(chapter) + 1;

	ENetPacket* packet = enet_packet_create(packetData, sizeof(packetData), ENET_PACKET_FLAG_RELIABLE);
	enet_peer_send(peer, 0, packet);
}

void BaseClient::VoteCountdown() {
	char packetData[64];
	size_t offset = 0;
	packetData[0] = '7';												// Set packet type
	offset++;
	memcpy(packetData + offset, &localID, sizeof(localID));				// Add our ID
	offset += sizeof(localID);

	ENetPacket* packet = enet_packet_create(packetData, sizeof(packetData), ENET_PACKET_FLAG_RELIABLE);
	enet_peer_send(peer, 0, packet);
}

unsigned int BaseClient::GetPing() {
	if (peer->state == ENetPeerState::ENET_PEER_STATE_CONNECTED) {
		return peer->roundTripTime;
	}
	else {
		return 0;
	}
}

/*
PACKET TYPES:
0 - Deliver ID [Reliable] (Client-bound)
1 - Register Nickname [Reliable] (Server-bound)
2 - Announce Client(s) [Reliable] (Client-bound)
3 - Client Disconnect [Reliable] (Bi-directional)
4 - Position Update [Unreliable] (Bi-directional)
5 - Chapter Update [Reliable] (Bi-directional)
6 - Server Message [Reliable] (Client-bound)
7 - Vote Countdown [Reliable] (Server-bound) / Start Countdown [Reliable] (Client-bound)
*/

void BaseClient::HandlePacket(ENetPacket* packet) {
	char* data = (char*)packet->data;
	char packetType = data[0];

	//std::cout << std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count() << " RECEIVE: ";
	//for (int i = 0; i < packet->dataLength; i++) printf("%02x ", data[i]);
	//printf("\n");

	switch (packetType) {
	case '0':
	{
		break;
	}
	case '1':
	{
		break;
	}
	case '2':	// Announce Client(s) [Reliable] (Client-bound)
	{
		unsigned int offset = 1;
		// Get player ID
		unsigned int newClientID;
		memcpy(&newClientID, data + offset, sizeof(newClientID));
		offset += sizeof(newClientID);
		// Get player nickname
		char* nickname = data + offset;
		offset += strlen(nickname) + 1;
		// Get player colour
		unsigned int playerColour;
		memcpy(&playerColour, data + offset, sizeof(playerColour));
		offset += sizeof(playerColour);
		// Get trail colour
		unsigned int playerTrailColour;
		memcpy(&playerTrailColour, data + offset, sizeof(playerTrailColour));
		offset += sizeof(playerTrailColour);

		std::string sNickname = std::string(nickname);

		if (client_map.find(newClientID) != client_map.end()) {					// Client already exists (let's update their nickname?)
			client_map[newClientID]->nickname = sNickname;
		}
		else {																	// New Client
			client_map[newClientID] = new BaseUser(newClientID, sNickname, playerColour, playerTrailColour);

			char buf[128];
			sprintf_s(buf, "[PSync] %s has joined.\n", client_map[newClientID]->nickname.c_str());
			printf(buf);
			ui->CreateNotification(buf);
		}

		OnUserConnect(client_map[newClientID]);
		break;
	}
	case '3':	// Client Disconnect [Reliable] (Bi-directional)
	{
		unsigned int disconnectedID;
		memcpy(&disconnectedID, data + 1, sizeof(disconnectedID));

		if (client_map.find(disconnectedID) == client_map.end()) break;

		BaseUser* disconnectedClient = client_map[disconnectedID];

		// Get necessary data->
		std::string disconnectedUsername = disconnectedClient->nickname;

		// Destroy the client.
		delete client_map[disconnectedID];

		// Remove them from the map.
		std::map<int, BaseUser*>::iterator map_it;
		map_it = client_map.find(disconnectedID);
		client_map.erase(map_it);

		char buf[128];
		sprintf_s(buf, "[PSync] %s has left.\n", disconnectedUsername.c_str());
		printf(buf);
		ui->CreateNotification(buf);

		OnUserDisconnect(disconnectedUsername, disconnectedID);
		break;
	}
	case '4':	// Position Update [Unreliable] (Bi-directional)
	{
		size_t offset = 1;
		unsigned int clientID;
		memcpy(&clientID, data + offset, sizeof(clientID));
		offset += sizeof(clientID);

		if (client_map.find(clientID) == client_map.end()) break;	// Check if we have this client in our client_map

		memcpy(&(client_map[clientID]->pos), data + offset, sizeof(client_map[clientID]->pos));	// Copy over the position
		offset += sizeof(client_map[clientID]->pos);

		client_map[clientID]->prevPos.push_front(std::make_tuple(client_map[clientID]->pos[0], client_map[clientID]->pos[1], client_map[clientID]->pos[2]));

		client_map[clientID]->prevPosTime = client_map[clientID]->latestPosTime;
		client_map[clientID]->latestPosTime = std::chrono::high_resolution_clock::now();

		while (client_map[clientID]->prevPos.size() > config->playerTrailLength) {
			client_map[clientID]->prevPos.pop_back();
		}

		OnPositionUpdate(client_map[clientID]);
		break;
	}
	case '5':	// Chapter Update [Reliable] (Bi-directional)
	{
		size_t offset = 1;
		unsigned int clientID;
		memcpy(&clientID, data + offset, sizeof(clientID));
		offset += sizeof(clientID);

		if (client_map.find(clientID) == client_map.end()) break;	// Check if we have this client in our client_map

		// Get chapter
		char* chapter = data + offset;
		offset += strlen(chapter) + 1;
		client_map[clientID]->chapter = chapter;

		OnChapterUpdate(client_map[clientID]);
		break;
	}
	case '6':	// Server Message [Reliable] (Client-bound)
	{
		unsigned int offset = 1;
		// Get message
		char* message = data + offset;
		offset += strlen(message) + 1;

		//std::string sMessage = std::string(message);

		char buf[128];
		sprintf_s(buf, "[PSync] %s\n", message);
		printf(buf);
		ui->CreateNotification(buf, 8.0f);

		OnServerMessage(message);
		break;
	}
	case '7':	// Vote Countdown [Reliable] (Server-bound) / Start Countdown [Reliable] (Client-bound)
	{
		size_t offset = 1;
		unsigned int countdownStartTime;
		memcpy(&countdownStartTime, data + offset, sizeof(countdownStartTime));
		offset += sizeof(countdownStartTime);

		ui->QueueCountdown(countdownStartTime);

		OnCountdownStarted(countdownStartTime);
		break;
	}
	}

	HandleCustomPacket(packetType, data);
}