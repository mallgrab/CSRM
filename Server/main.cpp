#define DEFAULT_SERVER_PORT 11939
#define DEFAULT_MAX_CLIENTS 32

#include <cstdio>
#include <string>
#include <thread>
#include <vector>

#include "./GameServers/BaseServer.h"
#include "./GameServers/TestServer.h"

bool doRestart = false;
static BaseServer baseServer;
BaseServer* server = &baseServer;

int main(int argc, const char* argv[]) {
	std::vector<std::string> args(argv, argv + argc);
	

	// Get game argument and set server type, otherwise leave as base server;
	auto gameArg = std::find(args.begin(), args.end(), "-game");
	if (gameArg != args.cend()) {
		int index = std::distance(args.begin(), gameArg);
		std::string game = args[index+1];

		if (game == "test") {
			printf("Creating TEST server...\n");
			static TestServer testServer;
			server = &testServer;
		}
	}

	// Listen for user input and handle inputs
	std::thread* userInputThread = nullptr;
	userInputThread = new std::thread([](){
		std::string synchronousInput;
		char nextCharacter;

		while (true) {
			while (std::cin.peek() == EOF) {
				std::this_thread::yield();
			}
			
			nextCharacter = std::cin.get();
			if (nextCharacter != '\n') {
				synchronousInput += nextCharacter;
				continue;
			}

			// Check commands (I'm too lazy to add a proper command handler for so little commands right now, guess I'll go yanderedev on it)
			std::string::size_type strSplit = synchronousInput.find(' ');
			std::string command;
			std::string args;
			if (strSplit == std::string::npos) {
				command = synchronousInput;
			}
			else {
				command = synchronousInput.substr(0, strSplit);
				args = synchronousInput.substr(strSplit + 1);
			}

			if (command == "exit" || command == "quit") {
				server->StopServer();
				return;
			} else if (command == "help") {
				printf("I'm lazy, here's just the list of commands, the names should be self-explanatory:\n");
				printf("help\nclientcount|playercount\nclientlist|playerlist\nkickid <id>\nkick <username>\nsay <message>\nrestart\nexit|quit\n");
			}
			else if (command == "clientcount" || command == "playercount") {
				printf("%u clients currently connected.\n", server->GetClientCount());
			}
			else if (command == "clientlist" || command == "playerlist") {
				printf("%-8s%-18s%-12s%-24s  %s\n", "ID:", "Username:", "Chapter:", "Connected On:", "Position:");
				for (const auto& client_it : server->GetClientList()) {
					printf("%-8u%-18s%-12s%.24s  %f, %f, %f\n", client_it.first, client_it.second->nickname.c_str(), client_it.second->chapter.c_str(), client_it.second->connectTime.c_str(), client_it.second->pos[0], client_it.second->pos[1], client_it.second->pos[2]);
				}
			}
			else if (command == "kickid") {
				unsigned int idToKick = std::stoi(args);
				server->KickClient(idToKick, "");
			}
			else if (command == "kick") {
				std::string nickToKick = args;
				server->KickClient(-1, nickToKick);
			}
			else if (command == "say") {
				std::string message = args;
				server->SayMessage(message);
			}
			else if (command == "restart") {
				doRestart = true;
				server->StopServer();
			}
			else {
				printf("Unknown command '%s'.\n", synchronousInput.c_str());
			}

			// Reset input
			synchronousInput.clear();
		}
	});

	int port = DEFAULT_SERVER_PORT;
	int maxClients = DEFAULT_MAX_CLIENTS;

	// Get port argument
	auto portArg = std::find(args.begin(), args.end(), "-port");
	if (portArg != args.cend()) {
		int index = std::distance(args.begin(), portArg);
		port = std::stoi(args[index + 1]);
	}

	// Get max clients argument
	auto maxArg = std::find(args.begin(), args.end(), "-max");
	if (maxArg != args.cend()) {
		int index = std::distance(args.begin(), maxArg);
		maxClients = std::stoi(args[index + 1]);
	}

	do {
		doRestart = false;
		server->StartServer(port, maxClients);
	} while (doRestart);
	
	userInputThread->join();
}