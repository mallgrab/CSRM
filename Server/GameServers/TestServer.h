#pragma once
#include "BaseServer.h"

class TestServer : public virtual BaseServer {
public:
	void SayMessage(std::string message);
};