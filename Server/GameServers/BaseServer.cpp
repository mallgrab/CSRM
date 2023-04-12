#include "BaseServer.h"

#define TICK_RATE 40

unsigned int BaseServer::GetClientCount() {
	return (unsigned int)client_map.size();
}

std::map<int, Client*> BaseServer::GetClientList() {
	return client_map;
}


void BaseServer::StopServer() {
	printf("Shutting down PSync server...\n");
	stopServer = true;
}

void BaseServer::DisconnectClient(Client* disconnectedClient) {
	// Get necessary data.
	unsigned int disconnectedID = disconnectedClient->ID;
	std::string disconnectedUsername = disconnectedClient->nickname;

	// Destroy the client.
	delete client_map[disconnectedID];

	// Remove them from the map.
	std::map<int, Client*>::iterator map_it;
	map_it = client_map.find(disconnectedID);
	client_map.erase(map_it);

	printf("%u %s has disconnected.\n", disconnectedID, disconnectedUsername.c_str());

	// Broadcast that a client has disconnected
	char packetData[16];
	packetData[0] = '3';																						// Set packet type
	memcpy(packetData + 1, &disconnectedID, sizeof(disconnectedID));											// Add the client's ID
	ENetPacket* packet = enet_packet_create(packetData, sizeof(packetData), ENET_PACKET_FLAG_RELIABLE);
	enet_host_broadcast(server, 0, packet);

	for (int i = 0; i < server->connectedPeers; i++) {
		if (static_cast<Client*>(server->peers[i].data) == disconnectedClient) {
			enet_peer_disconnect_now(&server->peers[i], 0);
			break;
		}
	}
}

void BaseServer::SendUpdates() {
	for (const auto& client_it : client_map) {
		Client* client = client_it.second;
		if (client == nullptr) continue;
		//if (client->pos[0] == client->prevPos[0] && client->pos[1] == client->prevPos[1] && client->pos[2] == client->prevPos[2]) continue;

		// Send position
		char packetData[64];
		size_t offset = 0;
		packetData[0] = '4';												// Set packet type
		offset++;
		memcpy(packetData + offset, &client->ID, sizeof(client->ID));		// Add our ID
		offset += sizeof(client->ID);
		memcpy(packetData + offset, &client->pos, sizeof(client->pos));		// Add position
		offset += sizeof(client->pos);

		ENetPacket* packet = enet_packet_create(packetData, sizeof(packetData), 0);

		enet_host_broadcast(server, 0, packet);
	}
}

void BaseServer::StartServer(int port, int numClients) {
	if (enet_initialize() != 0) {
		fprintf(stderr, "An error occurred while initializing ENet.\n");
		throw stderr;
	}
	atexit(enet_deinitialize);

	address.host = ENET_HOST_ANY;
	address.port = port;

	server = enet_host_create(&address, // address of this server
		numClients, // number of clients
		1, // number of channels
		0, //incoming bandwidth limit (0 for no limit)
		0); //outgoing bandwidth limit (0 for no limit)

	if (server == NULL) {
		printf("An error occurred while trying to create an ENet server host.");
		throw 1;
	}

	printf("PSync server started on port %i with %i slots, waiting for clients...\n", port, numClients);

	// Init
	stopServer = false;
	unsigned int nextClientID = 0;
	auto lastUpdateTime = std::chrono::high_resolution_clock::now();

	/*
	client_map[9996] = new Client(9996);
	client_map[9996]->nickname = "Bob";
	
	client_map[9997] = new Client(9997);
	client_map[9997]->nickname = "Clarence";
	
	client_map[9998] = new Client(9998);
	client_map[9998]->nickname = "Steven";

	client_map[9999] = new Client(9999);
	client_map[9999]->nickname = "JonTron";
	*/
	
	// Receive
	ENetEvent event;
	while (!stopServer) {
		while (enet_host_service(server, &event, 5000) > 0) {
			// Send timer
			long long timeSinceUpdate = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - lastUpdateTime).count();
			if (timeSinceUpdate >= TICK_RATE) {
				SendUpdates();
				lastUpdateTime = std::chrono::high_resolution_clock::now();
			}

			switch (event.type) {
			case ENET_EVENT_TYPE_CONNECT:
			{
				printf("New client connected, assigning ID: %u\n", nextClientID);
				client_map[nextClientID] = new Client(nextClientID);
#if CLIENT_TIMEOUT_TIME
				client_map[nextClientID]->lastPacketTime = clock();
#endif
				event.peer->data = client_map[nextClientID];

				// Once a client connects, send them their ID.
				char packetData[16];
				packetData[0] = '0';																					// Set packet type
				memcpy(packetData + 1, &client_map[nextClientID]->ID, sizeof(client_map[nextClientID]->ID));			// Add the client's ID
				ENetPacket* packet = enet_packet_create(packetData, sizeof(packetData), ENET_PACKET_FLAG_RELIABLE);
				enet_peer_send(event.peer, 0, packet);

				nextClientID++;
				break;
			}
			case ENET_EVENT_TYPE_RECEIVE:
			{
				// Parse packets
				HandlePacket(event.packet, event.peer);
				break;
			}
			case ENET_EVENT_TYPE_DISCONNECT:
			{
				// Get our disconnected client.
				Client* disconnectedClient = static_cast<Client*>(event.peer->data);
				if (disconnectedClient == NULL) break;

				DisconnectClient(disconnectedClient);
				event.peer->data = NULL;

				// Reset nextClientID to 0 when no-one is connected.
				if (client_map.size() == 0) nextClientID = 0;
				break;
			}
			}
			enet_packet_destroy(event.packet);
			if (stopServer) break;
		}
	}

	// Disconnect all the peers
	printf("Disconnecting clients...\n");
	for (int i = 0; i < server->connectedPeers; i++) {
		enet_peer_disconnect_now(&server->peers[i], 0);
	}

	// Clear our client_map
	for (const auto& client_it : client_map) {
		delete client_it.second;
	}
	client_map.clear();

	// Destroy the server
	enet_host_destroy(server);

	printf("PSync server successfully shutdown.\n");
	return;
}

/*
PACKET TYPES:
0 - Deliver ID [Reliable] (Client-bound)
1 - Register Nickname [Reliable] (Server-bound)
2 - Announce Client(s) [Reliable] (Client-bound)
3 - Client Disconnected [Reliable] (Bi-directional)
4 - Position Update [Unreliable] (Bi-directional)
5 - Chapter Update [Reliable] (Bi-directional)
6 - Server Message [Reliable] (Client-bound)
7 - Vote Countdown [Reliable] (Server-bound) / Start Countdown [Reliable] (Client-bound)
*/

void BaseServer::HandlePacket(ENetPacket* packet, ENetPeer* peer) {
	Client* client = static_cast<Client*>(peer->data);
	char* data = (char*)packet->data;
	char packetType = data[0];

#if CLIENT_TIMEOUT_TIME
	if (client) client->lastPacketTime = clock();
#endif

	switch (packetType) {
	case '0':
	{
		break;
	}
	case '1':	// Register the nickname and announce the client to others.
	{
		// Check their ID
		unsigned int offset = 1;
		unsigned int verifyID;
		memcpy(&verifyID, data + offset, sizeof(verifyID));
		offset += sizeof(verifyID);
		if (verifyID != client->ID) break;

		// Get the requested nickname
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

		//for (int i = 0; i < offset; i++) printf("%02x ", data[i]);
		//printf("\n");

		std::string sNickname = std::string(nickname);

		// Run nickname checks here
		// Check that nickname is not empty
		if (sNickname == "") {
			printf("Client %u requested an empty nickname, rejecting.\n", client->ID);
			DisconnectClient(client);
			break;
		}

		// Check if the nickname is unique
		bool isUnique = true;
		for (const auto& client_it : client_map) {
#if CLIENT_TIMEOUT_TIME
			if (client_it.second->nickname == sNickname) {
				printf("Client %u requested nickname (%s) was not unique, ", client->ID, sNickname.c_str());
				clock_t timeout = clock() - client_it.second->lastPacketTime;
				if (client_it.second->lastPacketTime < client->lastPacketTime && timeout >= CLIENT_TIMEOUT_TIME)
				{
					printf("timing out old client %i.\n", timeout);
					DisconnectClient(client_it.second);
				}
				else {
					printf("rejecting.\n");
					isUnique = false;
				}
				break;
			}
#else
			if (client_it.second->nickname == sNickname) {
				printf("Client %u requested nickname (%s) was not unique, rejecting.\n", client->ID, sNickname.c_str());
				isUnique = false;
				break;
			}
#endif
		}
		if (!isUnique) {
			DisconnectClient(client);
			break;
		}

		client->nickname = sNickname;
		client->colour = playerColour;
		client->trailColour = playerTrailColour;
		printf("Client %u has registered their nickname as: %s\n", client->ID, client->nickname.c_str());

		// All nickname checks passed
		// Announce the newly registered client with their ID and nickname.
		offset = 0;
		char packetData[128];
		packetData[0] = '2';																						// Set packet type
		offset++;
		memcpy(packetData + offset, &client->ID, sizeof(client->ID));												// Add the client's ID
		offset += sizeof(client->ID);
		memcpy(packetData + offset, client->nickname.c_str(), strlen(client->nickname.c_str()) + 1);				// Add the client's nickname
		offset += strlen(client->nickname.c_str()) + 1;
		memcpy(packetData + offset, &client->colour, sizeof(client->colour));										// Add the client's colour
		offset += sizeof(client->colour);
		memcpy(packetData + offset, &client->trailColour, sizeof(client->trailColour));								// Add the client's trail colour
		offset += sizeof(client->trailColour);

		//for (int i = 0; i < offset; i++) printf("%02x ", packetData[i]);
		//printf("\n");

		ENetPacket* packet = enet_packet_create(packetData, sizeof(packetData), ENET_PACKET_FLAG_RELIABLE);
		enet_host_broadcast(server, 0, packet);

		// Update the newly registered client with the other clients' info.
		for (const auto& client_it : client_map) {
			Client* otherClient = client_it.second;

			char newPacketData[64];
			size_t offset = 0;
			newPacketData[0] = '2';																						// Set packet type
			offset++;
			memcpy(newPacketData + offset, &otherClient->ID, sizeof(otherClient->ID));									// Add the client's ID
			offset += sizeof(otherClient->ID);
			memcpy(newPacketData + offset, otherClient->nickname.c_str(), strlen(otherClient->nickname.c_str()) + 1);	// Add the client's nickname
			offset += strlen(otherClient->nickname.c_str()) + 1;
			memcpy(newPacketData + offset, &otherClient->colour, sizeof(otherClient->colour));							// Add the client's colour
			offset += sizeof(otherClient->colour);
			memcpy(newPacketData + offset, &otherClient->trailColour, sizeof(otherClient->trailColour));				// Add the client's trail colour
			offset += sizeof(otherClient->trailColour);

			ENetPacket* newPacket = enet_packet_create(newPacketData, sizeof(newPacketData), ENET_PACKET_FLAG_RELIABLE);
			enet_peer_send(peer, 0, newPacket);
		}
		OnClientRegistered(client);
		break;
	}
	case '2':
	{
		break;
	}
	case '3':
	{
		break;
	}
	case '4':	// Update client position
	{
		size_t offset = 1;
		// Check their ID
		unsigned int verifyID;
		memcpy(&verifyID, data + offset, sizeof(verifyID));
		if (verifyID != client->ID) break;

		client->prevPos[0] = client->pos[0];
		client->prevPos[1] = client->pos[1];
		client->prevPos[2] = client->pos[2];
		offset += sizeof(verifyID);
		memcpy(&client->pos, data + offset, sizeof(client->pos));	// Copy over the position
		offset += sizeof(client->pos);
		OnClientPositionUpdate(client);
		break;
	}
	case '5':	// Update client chapter
	{
		size_t offset = 1;
		// Check their ID
		unsigned int verifyID;
		memcpy(&verifyID, data + offset, sizeof(verifyID));
		if (verifyID != client->ID) break;
		offset += sizeof(verifyID);

		// Get the new chapter
		char* chapter = data + offset;
		offset += strlen(chapter) + 1;

		client->chapter = chapter;

		// Update other with client's new chapter
		offset = 0;
		char packetData[128];
		packetData[0] = '5';																						// Set packet type
		offset++;
		memcpy(packetData + offset, &client->ID, sizeof(client->ID));												// Add the client's ID
		offset += sizeof(client->ID);
		memcpy(packetData + offset, client->chapter.c_str(), strlen(client->chapter.c_str()) + 1);				// Add the client's chapter
		offset += strlen(client->chapter.c_str()) + 1;

		ENetPacket* packet = enet_packet_create(packetData, sizeof(packetData), ENET_PACKET_FLAG_RELIABLE);
		enet_host_broadcast(server, 0, packet);
		OnClientChapterUpdate(client);
		break;
	}
	case '6':
	{
		break;
	}
	case '7':
	{
		size_t offset = 1;
		// Check their ID
		unsigned int verifyID;
		memcpy(&verifyID, data + offset, sizeof(verifyID));
		if (verifyID != client->ID) break;

		client->votedCountdown = true;

		unsigned int clientsVoted = GetCountdownVoteCount();
		unsigned int clientCount = GetClientCount();

		float votePerc = (float)clientsVoted / clientCount;

		char buf[64];
		snprintf(buf, sizeof(buf), "%.1f%% of users have voted for a countdown!", votePerc * 100.0);
		SayMessage(buf);

		bool success = (votePerc >= 0.5);
		if (success) {
			ResetCountdownVotes();
			SendCountdown();
		}
		OnClientCountdownVote(client, success);
		break;
	}
	}
	HandleCustomPacket(peer, packetType, data);
}

void BaseServer::KickClient(int ID, std::string nickname) {
	bool clientKicked = false;

	for (const auto& client_it : client_map) {
		if (client_it.second->ID == ID) {
			DisconnectClient(client_it.second);
			clientKicked = true;
			break;
		}

		if (client_it.second->nickname == nickname) {
			DisconnectClient(client_it.second);
			clientKicked = true;
			break;
		}
	}

	if (!clientKicked) {
		printf("Could not find client.\n");
	}
}

void BaseServer::SayMessage(std::string message) {
	unsigned int offset = 0;
	char packetData[128];
	packetData[0] = '6';																						// Set packet type
	offset++;
	memcpy(packetData + offset, message.c_str(), strlen(message.c_str()) + 1);									// Add the message
	offset += strlen(message.c_str()) + 1;

	//for (int i = 0; i < offset; i++) printf("%02x ", packetData[i]);
	//printf("\n");

	ENetPacket* packet = enet_packet_create(packetData, sizeof(packetData), ENET_PACKET_FLAG_RELIABLE);
	enet_host_broadcast(server, 0, packet);
}

unsigned int BaseServer::GetCountdownVoteCount() {
	unsigned int voteCount = 0;

	for (const auto& client_it : client_map) {
		if (client_it.second->votedCountdown) voteCount++;
	}

	return voteCount;
}

void BaseServer::ResetCountdownVotes() {
	for (const auto& client_it : client_map) {
		client_it.second->votedCountdown = false;
	}
}

void BaseServer::SendCountdown() {
	unsigned int offset = 0;
	char packetData[32];
	packetData[0] = '7';																						// Set packet type
	offset++;

	auto tp = std::chrono::system_clock::now();
	auto dur = tp.time_since_epoch();
	unsigned int seconds = std::chrono::duration_cast<std::chrono::seconds>(dur).count() + 2;

	memcpy(packetData + offset, &seconds, sizeof(seconds));									// Add the time
	offset += sizeof(seconds);

	ENetPacket* packet = enet_packet_create(packetData, sizeof(packetData), ENET_PACKET_FLAG_RELIABLE);
	enet_host_broadcast(server, 0, packet);
}

void BaseServer::SendCustomPacket(ENetPeer* peer, char packetType, char data[64], bool reliable) {
	char packetData[65];
	packetData[0] = packetType;
	packetData[1] = *data;
	ENetPacket* packet = enet_packet_create(packetData, sizeof(packetData), (reliable) ? ENET_PACKET_FLAG_RELIABLE : 0);
	enet_peer_send(peer, 0, packet);
}

void BaseServer::BroadcastCustomPacket(char packetType, char data[64], bool reliable) {
	char packetData[65];
	packetData[0] = packetType;
	packetData[1] = *data;
	ENetPacket* packet = enet_packet_create(packetData, sizeof(packetData), (reliable) ? ENET_PACKET_FLAG_RELIABLE : 0);
	enet_host_broadcast(server, 0, packet);
}