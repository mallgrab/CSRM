#pragma once
#define _WINSOCKAPI_
#include <Windows.h>
#include <chrono>
#include "../ocular/imgui/imgui.h"

#include "../Utils.h"
#include "../Client/BaseUser.h"

class BaseGameData;
class BaseConfig;
class BaseClient;

#include "../Client/BaseClient.h"

class BaseUI {
public:
	BaseUI() {

	};

	BaseGameData* data;
	BaseConfig* config;
	BaseClient* client;

	void KeyPress(WPARAM key);
	virtual void Init();
	virtual void RenderGUI();
	virtual void RenderOSD();
	void CreateNotification(const char *message);
	void CreateNotification(const char *message, float duration);
	void CreateNotification(const char *message, ImColor bgCol, ImColor textCol);
	void CreateNotification(const char *message, float duration, float fadeIn, float fadeOut);
	void CreateNotification(const char *message, float duration, float fadeIn, float fadeOut, ImColor bgCol, ImColor textCol);

	void QueueCountdown(unsigned int time);

	// Fonts
	ImFont* rodinProMFontBig;
	ImFont* rodinProMFontSmall;
protected:
	bool init = false;
	float screenWidth = 0.0f;
	float screenHeight = 0.0f;
	// Notification vars
	bool drawNotif = false;
	char notifMessage[256];
	ImColor notifBgCol = ImColor(0.2f, 0.2f, 0.2f, 0.9f);
	ImColor notifTextCol = ImColor(1.0f, 1.0f, 1.0f, 1.0f);
	float notifRemainingTime = 0.0f;
	float notifFadeIn = 0.1f;
	float notifFadeOut = 0.5f;;
	float notifOpacityPerc = 0.0f;
	// Countdown vars
	bool waitingCountdown = false;
	bool drawCountdown = false;
	unsigned int countdownStartTime = 0;
	float countdownRemainingTime = 0.0f;

	// Other vars
	unsigned int gHue = 0;

	bool rebindingKey = false;

	virtual ImVec2 DrawOctahedron(ImDrawList* drawList, ImVec2 screenSize, Matrix4* viewMatrix, Vector3* position, float scale, ImColor color, float thickness);
	ImVec2 DrawBox(ImDrawList* drawList, ImVec2 screenSize, Matrix4* viewMatrix, Vector3* position, float scale, ImColor color, float thickness);
	void G_DebugBoxLines(ImDrawList* drawList, ImVec2 screenSize, Matrix4* viewMatrix, Vector3* position, float scale, ImColor color, float thickness);
	void DrawNotifications(ImGuiIO io, ImDrawList* drawList);
	void DrawCountdown(ImGuiIO io, ImDrawList* drawList, float width, float height);

	virtual void ConnectionTab();
	virtual void CustomizationTab();
	virtual void ConfigTab();
	virtual void DebugTab();
	virtual void CreditsTab();

	virtual void DrawPlayerList();
	virtual void DrawPlayerObjects();

	virtual void SetupTheme();
};