#pragma once
#include <iostream>
#include <ctime>
#include <map>
#include <deque>
#include <chrono>

#include "../../ocular/imgui/imgui.h"

class BaseUser {
public:
	inline BaseUser(int clientID, std::string username, unsigned int playerColour, unsigned int playerTrailColour) {
		ID = clientID;
		nickname = username;

		ImColor col = ImGui::ColorConvertU32ToFloat4(playerColour);
		colour[0] = col.Value.x;
		colour[1] = col.Value.y;
		colour[2] = col.Value.z;

		col = ImGui::ColorConvertU32ToFloat4(playerTrailColour);
		trailColour[0] = col.Value.x;
		trailColour[1] = col.Value.y;
		trailColour[2] = col.Value.z;

		char buf[26];
		time_t now = time(0);
		ctime_s(buf, 26, &now);
		connectTime = buf;
	}

	std::string nickname;
	std::string connectTime;

	unsigned int ID;
	float colour[3];
	float trailColour[3];
	float pos[3] = { 0.0f, 0.0f, 0.0f };
	std::string chapter;

	std::deque<std::tuple<float, float, float>> prevPos;
	std::chrono::steady_clock::time_point latestPosTime;
	std::chrono::steady_clock::time_point prevPosTime;

	inline ~BaseUser() {
		nickname.clear();
		connectTime.clear();
		prevPos.clear();
	}
};