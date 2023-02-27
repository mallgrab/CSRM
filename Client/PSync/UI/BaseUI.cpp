#include "BaseUI.h"
#include "../Fonts/RodinProM.cpp"

#define NOTIF_PADDING 10
#define PI 3.141592

// Countdown
float countdownOpacityPerc = 1.0f;
float countdownGoTime = 0.0f;

uint64_t* currentRegionAddress = nullptr;

float countdownTme = 3.0f;
char chapterString[32];

char ipToConnect[64] = "";
char portToConnect[8] = "";
char nickname[16] = "";

bool drawPlayers = false;

float curTime = 0;


void BaseUI::KeyPress(WPARAM key) {
	if (key == config->menuKeybind) {
		data->uiToggle = !data->uiToggle;
	}
}

void BaseUI::Init() {
	ImGuiIO& io = ImGui::GetIO();

	io.MouseDrawCursor = (config->drawImGuiCursor && data->uiToggle);

	if (init) return;
	ImVector<ImWchar> ranges;
	ImFontGlyphRangesBuilder builder;
	builder.AddRanges(io.Fonts->GetGlyphRangesDefault());
	builder.BuildRanges(&ranges);

	io.Fonts->AddFontDefault();
	rodinProMFontBig = io.Fonts->AddFontFromMemoryCompressedTTF(RodinProM_compressed_data, RodinProM_compressed_size, 128.0f, NULL, ranges.Data);
	rodinProMFontSmall = io.Fonts->AddFontFromMemoryCompressedTTF(RodinProM_compressed_data, RodinProM_compressed_size, 18.0f, NULL, ranges.Data);
	io.Fonts->Build();
	SetupTheme();
	init = true;
}

/*
GUI
*/

void BaseUI::RenderGUI() {
	if (!init) return;

	gHue++;
	gHue = gHue % 360;

	if (!data->uiToggle) {
		return;
	}

	ImGuiIO& io = ImGui::GetIO();

		ImGui::SetNextWindowSizeConstraints(ImVec2(520.0f, 150.0f), io.DisplaySize);
		if (!ImGui::Begin("PooPMod", &data->uiToggle, 0)) { ImGui::End(); return; }
		
		ImGui::Text(
			" _|_|_|      _|_|      _|_|    _|_|_|    _|      _|    _|_|    _|_|_|    \n"
			" _|    _|  _|    _|  _|    _|  _|    _|  _|_|  _|_|  _|    _|  _|    _|  \n"
			" _|_|_|    _|    _|  _|    _|  _|_|_|    _|  _|  _|  _|    _|  _|    _|  \n"
			" _|        _|    _|  _|    _|  _|        _|      _|  _|    _|  _|    _|  \n"
			" _|          _|_|      _|_|    _|        _|      _|    _|_|    _|_|_|    \n"
		);

		ImGui::Text(" - The stinkiest thing since your feet...");

	ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;
	if (ImGui::BeginTabBar("Tabs", tab_bar_flags))
	{
		ConnectionTab();
		CustomizationTab();
		ConfigTab();
		DebugTab();
		CreditsTab();
		ImGui::EndTabBar();
	}

	ImGui::End();
}

void BaseUI::DebugTab() {
	if (ImGui::BeginTabItem("Debug"))
	{
		ImGuiIO& io = ImGui::GetIO();

		Matrix4* myViewMatrix = data->GetViewMatrix();
		Vector3* myPlayerPos = data->GetPlayerPos();

		/*
		ImGui::InputFloat("Countdown Time", &countdownTme);
		if (ImGui::Button("Start Countdown")) {
			drawCountdown = true;
			countdownRemainingTime = countdownTme;
			countdownGoTime = 2.0f;
			countdownOpacityPerc = 1.0f;
		}
		*/

		ImGui::Text("\nScreen Size:");
		ImGui::Text("%f, %f", io.DisplaySize.x, io.DisplaySize.y);

		/*
		ImGui::Checkbox("Override Resolution", &config->overrideResolution);
		ImGui::InputFloat("Custom Width", &config->customWidth);
		ImGui::InputFloat("Custom Height", &config->customHeight);
		*/

		if (myPlayerPos != nullptr) {
			ImGui::Text("\nPosX: %f", myPlayerPos->x);
			ImGui::Text("PosY: %f", myPlayerPos->y);
			ImGui::Text("PosZ: %f", myPlayerPos->z);
		}

		if (myViewMatrix != nullptr) {
			ImGui::Text("\nViewMatrix:");
			ImGui::Text("%f, %f, %f, %f", myViewMatrix->_00, myViewMatrix->_01, myViewMatrix->_02, myViewMatrix->_03);
			ImGui::Text("%f, %f, %f, %f", myViewMatrix->_10, myViewMatrix->_11, myViewMatrix->_12, myViewMatrix->_13);
			ImGui::Text("%f, %f, %f, %f", myViewMatrix->_20, myViewMatrix->_21, myViewMatrix->_22, myViewMatrix->_23);
			ImGui::Text("%f, %f, %f, %f", myViewMatrix->_30, myViewMatrix->_31, myViewMatrix->_32, myViewMatrix->_33);

			Matrix4 transposed = myViewMatrix->Transpose();
			ImGui::Text("\nTransposed ViewMatrix:");
			ImGui::Text("%f, %f, %f, %f", transposed._00, transposed._01, transposed._02, transposed._03);
			ImGui::Text("%f, %f, %f, %f", transposed._10, transposed._11, transposed._12, transposed._13);
			ImGui::Text("%f, %f, %f, %f", transposed._20, transposed._21, transposed._22, transposed._23);
			ImGui::Text("%f, %f, %f, %f", transposed._30, transposed._31, transposed._32, transposed._33);

			Vector3 worldPos;
			worldPos.x = 0;
			worldPos.y = 0;
			worldPos.z = 0;

			Vector2 scrnOutPos;

			if (DXWorldToScreen(myViewMatrix, worldPos, io.DisplaySize.x, io.DisplaySize.y, scrnOutPos)) {
				ImGui::Text("\nScreenPos:");
				ImGui::Text("%f, %f", scrnOutPos.x, scrnOutPos.y);
			}
		}

		if (currentRegionAddress != nullptr) {
			ImGui::Text((char*)currentRegionAddress);
		}

		ImGui::Checkbox("Draw Own User", &config->drawSelf);

		/*
		ImGui::InputText("Chapter String", chapterString, sizeof(chapterString));
		if (ImGui::Button("Set Chapter")) {
			client->SendChapter(chapterString);
		}

		ImGui::Checkbox("DO NOT ENABLE UNLESS YOU SERIOUSLY KNOW WHAT YOU ARE DOING", &config->unknownBoolean);
		*/
		
		ImGui::EndTabItem();
	}
}

void BaseUI::ConnectionTab() {
	if (ImGui::BeginTabItem("Connection"))
	{
		ImGui::PushItemWidth(ImGui::GetWindowSize().x);
		ImGui::Text("Server IP");
		ImGui::InputText("##serverip", config->serverIP, sizeof(config->serverIP));
		ImGui::Text("Server Port");
		ImGui::InputText("##serverport", config->serverPort, sizeof(config->serverPort));
		ImGui::Text("Nickname");
		ImGui::InputText("##nickname", config->nickname, sizeof(config->nickname));
		ImGui::PopItemWidth();
		ImGui::Checkbox("Connect On Startup", &config->connectOnStart);

		if (ImGui::CollapsingHeader("Colours")) {
			ImGui::ColorEdit3("Colour", config->myColour);
			ImGui::ColorEdit3("Trail Colour", config->myTrailColour);
		}
		ImGui::Separator();

		ImGui::Indent(ImGui::GetWindowSize().x / 3);
		if (!client->IsRunning()) {
			if (ImGui::Button("Connect", ImVec2(ImGui::GetWindowSize().x / 3, 24.0f))) {
				unsigned int playerColour = ImGui::ColorConvertFloat4ToU32(Float3AToImColor(config->myColour, 1.0f));
				unsigned int playerTrailColour = ImGui::ColorConvertFloat4ToU32(Float3AToImColor(config->myTrailColour, 1.0f));

				try {
					client->StartClient(config->serverIP, atoi(config->serverPort), config->nickname, playerColour, playerTrailColour);
				}
				catch (const std::exception& exc) {
					std::cerr << exc.what();
					client->ResetClient();
				}
			}
		}
		else {
			std::string status;
			if (client->status != "Connected") {
				status = client->status;
			}
			else {
				status = "Disconnect";
			}

			if (ImGui::Button(status.c_str(), ImVec2(ImGui::GetWindowSize().x / 3, 24.0f))) {
				client->StopClient();
			}
		}
		ImGui::Unindent(ImGui::GetWindowSize().x / 3);


		if (client->GetUserCount() > 0) {
			ImGui::Separator();
			ImGui::Text("Ping: %u", client->GetPing());
			ImGui::Text("Users connected: %i", client->GetUserCount());

			if (ImGui::BeginTable("userTable", 4))
			{
				ImGui::TableSetupColumn("ID");
				ImGui::TableSetupColumn("Nickname");
				ImGui::TableSetupColumn("Chapter");
				ImGui::TableSetupColumn("Position");
				ImGui::TableHeadersRow();

				for (const auto& client_it : client->GetUserList()) {
					BaseUser* user = client_it.second;
					if (user == nullptr) continue;
					if (user->nickname == "") continue;

					ImGui::TableNextRow();

					ImGui::TableNextColumn();
					ImGui::Text("%i", user->ID);
					ImGui::TableNextColumn();
					ImGui::Text("%s", user->nickname.c_str());
					ImGui::TableNextColumn();
					ImGui::Text("%s", user->chapter.c_str());
					ImGui::TableNextColumn();
					ImGui::Text("%.2f, %.2f, %.2f", user->pos[0], user->pos[1], user->pos[2]);
				}
				ImGui::EndTable();
			}
			ImGui::Separator();
			ImGui::Indent(ImGui::GetWindowSize().x / 3);
			if (ImGui::Button("Vote for Countdown", ImVec2(ImGui::GetWindowSize().x / 3, 24.0f))) client->VoteCountdown();
			ImGui::Unindent(ImGui::GetWindowSize().x / 3);
		}
		ImGui::EndTabItem();
	}
}

void BaseUI::CustomizationTab() {
	if (ImGui::BeginTabItem("Customization"))
	{
		if (ImGui::CollapsingHeader("Nicknames")) {
			ImGui::ColorEdit4("Nickname Plate Colour", config->playerNicknamePlateColour);
		}

		if (ImGui::CollapsingHeader("Objects")) {
			if (ImGui::InputFloat("Player Object Opacity", &config->playerObjectOpacity, 0.1f)) {
				if (config->playerObjectOpacity < 0.0f) config->playerObjectOpacity = 0.0f;  // Min
				if (config->playerObjectOpacity > 1.0f) config->playerObjectOpacity = 1.0f;  // Max
			}
			if (ImGui::InputFloat("Player Object Scale", &config->playerObjectScale, 0.1f) && config->playerObjectScale <= 0) config->playerObjectScale = 0.1f;
		}

		if (ImGui::CollapsingHeader("Trails")) {
			ImGui::Checkbox("Draw Trails", &config->playerTrailEnabled);
			if (ImGui::InputFloat("Player Trail Opacity", &config->playerTrailOpacity, 0.1f)) {
				if (config->playerTrailOpacity < 0.0f) config->playerTrailOpacity = 0.0f;  // Min
				if (config->playerTrailOpacity > 1.0f) config->playerTrailOpacity = 1.0f;  // Max
			}
			if (ImGui::InputFloat("Player Trail Thickness", &config->playerTrailThickness, 1.0f) && config->playerTrailThickness <= 0) config->playerTrailThickness = 1.0f;
			if (ImGui::InputInt("Player Trail Length", &config->playerTrailLength, 1) && config->playerTrailLength <= 4) config->playerTrailLength = 5;
		}

		if (ImGui::CollapsingHeader("Player List")) {
			ImGui::Checkbox("Draw Player List", &config->playerListEnabled);
			if (ImGui::InputInt("Player List Width", &config->playerListWidth, 5) && config->playerListWidth <= 0) config->playerListWidth = 1;
		}

		ImGui::EndTabItem();
	}
}

void BaseUI::ConfigTab() {
	if (ImGui::BeginTabItem("Configuration"))
	{
		if (ImGui::CollapsingHeader("Menu")) {
			if (ImGui::ColorEdit3("Accent Colour", config->menuAccentColour)) SetupTheme();
			ImGui::Checkbox("Draw ImGui Cursor", &config->drawImGuiCursor);
		}

		if (ImGui::CollapsingHeader("System")) {
			ImGui::Checkbox("Position Smoothing", &config->positionSmoothing);
		}

		if (ImGui::CollapsingHeader("Keybinds")) {
			ImGuiIO io = ImGui::GetIO();

			char keyName[64];
			UINT scanCode = MapVirtualKey(config->menuKeybind, MAPVK_VK_TO_VSC);
			switch (config->menuKeybind)
			{
			case VK_LEFT: case VK_UP: case VK_RIGHT: case VK_DOWN:
			case VK_RCONTROL: case VK_RMENU:
			case VK_LWIN: case VK_RWIN: case VK_APPS:
			case VK_PRIOR: case VK_NEXT:
			case VK_END: case VK_HOME:
			case VK_INSERT: case VK_DELETE:
			case VK_DIVIDE:
			case VK_NUMLOCK:
				scanCode |= KF_EXTENDED;
			default:
				GetKeyNameTextA(scanCode << 16, keyName, sizeof(keyName));
			}

			ImGui::Text("Menu Keybind:");
			ImGui::Indent(ImGui::GetWindowSize().x / 4);
			if (!rebindingKey) {
				if (ImGui::Button(keyName, ImVec2(ImGui::GetWindowSize().x / 2, 24.0f))) {
					rebindingKey = true;
				}
			}
			else {
				ImGui::Indent(ImGui::GetWindowSize().x / 2);
				ImGui::Button("Press Any Key... (ESC To Cancel)", ImVec2(ImGui::GetWindowSize().x / 2, 24.0f));
				for (int i = 0; i < IM_ARRAYSIZE(io.KeysDown); i++) {
					if (io.KeysDownDuration[i] > 0.0f) {
						if (i == VK_ESCAPE) {                           // Canceled
							rebindingKey = false;
							break;
						}
						else if (i == config->menuKeybind) {       // Pressed current menuKeybind
							rebindingKey = false;
							break;
						}
						else {                                          // Pressed new key
							config->menuKeybind = i;
							rebindingKey = false;
							break;
						}
					}
				}
			}
			ImGui::Unindent(ImGui::GetWindowSize().x / 4);
		}
		ImGui::Separator();
		ImGui::Separator();
		ImGui::Separator();
		if (ImGui::Button("Save Config", ImVec2(ImGui::GetWindowSize().x / 2, 24.0f))) {
			config->WriteConfig();
		}
		ImGui::SameLine();
		if (ImGui::Button("Load Config", ImVec2(ImGui::GetWindowSize().x / 2, 24.0f))) {
			config->ReadConfig();
		}

		ImGui::EndTabItem();
	}
}

void BaseUI::CreditsTab() {
	if (ImGui::BeginTabItem("Credits"))
	{
		ImGui::Text("~ The Successor of NARoIP ~");
		ImGui::Separator();
		ImGui::Text("\tCreated by Woeful_Wolf");
		ImGui::Separator();
		ImGui::Text("\tkiero - Rebzzel");
		ImGui::Text("\tMinHook - TsudaKageyu");
		ImGui::Text("\tDear ImGui - ocornut");
		ImGui::Text("\tENet - lsalzman");
		ImGui::Text("\tDMC3SE Help - deepdarkkapustka");
		ImGui::Text("\tMarketing - Siyan");
		ImGui::Text("\tDMC3SE Contributor - hies");
		ImGui::Text("\tTesting - Aloyark, Hellbufl, Isum1644, Jaynoo, Tsugoii, Valeria-CHAN");
		ImGui::Separator();
		ImGui::Text("Original NARoIP Credits:");
		ImGui::Text("\tVC3Mod - icefire");
		ImGui::Text("\t2BHook - Martino");
		ImGui::Text("\tTesting - Amei, Mytherium, gbchan, Kekoulis, rodg");
		ImGui::EndTabItem();
	}
}

/*
OSD
*/

void BaseUI::RenderOSD() {
	if (!init) return;

	ImGuiIO io = ImGui::GetIO();
	ImDrawList* drawList = ImGui::GetForegroundDrawList();

	drawPlayers = (client->GetUserCount() > 0);

	curTime = curTime + io.DeltaTime;
	curTime = std::fmod(curTime, 6000.0f);

	screenWidth = io.DisplaySize.x;
	screenHeight = io.DisplaySize.y;

	if (config->overrideResolution) {
		screenWidth = config->customWidth;
		screenHeight = config->customHeight;
	}

	DrawNotifications(io, drawList);
	DrawCountdown(io, drawList, screenWidth, screenHeight);

	//if (!drawPlayers) return;
	// Draw player list
	//if (config->playerListEnabled) DrawPlayerList();

	// Draw viewport elements
	DrawPlayerObjects();
	
}

void BaseUI::DrawPlayerList() {
	int playerListWidth = config->playerListWidth;

	ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.1f, 0.1f, 0.1f, 0.3f));
	ImGui::SetNextWindowPos(ImVec2(screenWidth - playerListWidth, 0));
	ImGui::SetNextWindowSize(ImVec2(playerListWidth, 0));

	ImGui::PopStyleColor();
}

void BaseUI::DrawPlayerObjects() {
	ImDrawList* drawList = ImGui::GetForegroundDrawList();
	Matrix4* myViewMatrix = data->GetViewMatrix();
	if (myViewMatrix != nullptr) {
		Vector3 worldPos;
		ImVec2 nicknamePos;
		ImColor defaultTriggerColor = ImColor(1.0f, 1.0f, 1.0f, 0.5f);
		Vector2 tmp;

		for (const auto& client_it : client->GetUserList()) {
			if (client_it.second == nullptr) continue;
			if (client_it.second->nickname == "") continue;

			if (!config->drawSelf && client_it.first == client->localID) continue;

			// Position Smoothing
			if (client_it.second->prevPos.size() > 3 && config->positionSmoothing) {
				long long timeSincePrevPos = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - client_it.second->latestPosTime).count();
				long long timeBetweenPositions = std::chrono::duration_cast<std::chrono::milliseconds>(client_it.second->latestPosTime - client_it.second->prevPosTime).count();
				float perc = (float)timeSincePrevPos / timeBetweenPositions;

				if (perc < 1.0f) {
					Vector3 prevPos = client_it.second->prevPos.at(1);
					worldPos = prevPos.LerpTo(Vector3(client_it.second->pos), perc);
				}
				else {
					worldPos = client_it.second->pos;
				}
			}
			else {
				worldPos = client_it.second->pos;
			}

			worldPos.y += 1;

			Vector2 screenPos;
			if (!DXWorldToScreen(myViewMatrix, worldPos, screenWidth, screenHeight, screenPos)) continue;

			// Colours
			ImColor playerTrailColour = ImColor(Float3AToImColor(client_it.second->trailColour, config->playerTrailOpacity));
			ImColor playerObjColour = ImColor(Float3AToImColor(client_it.second->colour, config->playerObjectOpacity));
			ImColor playerNameColour = ImColor(Float3AToImColor(client_it.second->colour, 1.0f));

			// Trails
			if (config->playerTrailEnabled) {
				if (client_it.second->prevPos.size() > 3) {
					for (size_t i = 0; i < client_it.second->prevPos.size() - 3; i++) {
						if (client_it.second->prevPos[i] == client_it.second->prevPos[i + 1]) continue;

						Vector3 pos1;
						pos1.x = std::get<0>(client_it.second->prevPos[i]);
						pos1.y = std::get<1>(client_it.second->prevPos[i]) + 1.0f;
						pos1.z = std::get<2>(client_it.second->prevPos[i]);
						Vector3 pos2;
						pos2.x = std::get<0>(client_it.second->prevPos[i + 1]);
						pos2.y = std::get<1>(client_it.second->prevPos[i + 1]) + 1.0f;
						pos2.z = std::get<2>(client_it.second->prevPos[i + 1]);

						//float fastDistance = (pow(pos1.x - pos2.x, 2) + pow(pos1.y - pos2.y, 2) + pow(pos1.z - pos2.z, 2)) * 100000;
						//if (fastDistance < 0.5f) continue;

						Vector2 point1;
						Vector2 point2;
						if (!DXWorldToScreen(myViewMatrix, pos1, screenWidth, screenHeight, point1)) continue;
						if (!DXWorldToScreen(myViewMatrix, pos2, screenWidth, screenHeight, point2)) continue;

						drawList->AddLine(ImVec2(point1.x, point1.y), ImVec2(point2.x, point2.y), playerTrailColour, config->playerTrailThickness);
					}
				}
			}

			// Object
			nicknamePos = DrawOctahedron(drawList, ImVec2(screenWidth, screenHeight), myViewMatrix, &worldPos, config->playerObjectScale, playerObjColour, 2.0f);

			// Check if centre of screen
			if ((nicknamePos.x > screenWidth + 100 || nicknamePos.x < -100) && (nicknamePos.y > screenHeight + 100 || nicknamePos.y < -100)) continue;

			// Distance (if looking at the name)
			if (sqrt(pow(nicknamePos.x - screenWidth / 2, 2) + pow(nicknamePos.y - screenHeight / 2, 2)) < screenHeight / 4.5f) {
				Vector3* myPlayerPos = data->GetPlayerPos();
				float distance = -1.0f;

				if (myPlayerPos != nullptr) {
					distance = ((Vector3*)client_it.second->pos)->DistanceTo(*myPlayerPos);
				}

				char distStr[32] = "0.0m";
				sprintf_s(distStr, sizeof(distStr), "%.2fm", distance);

				ImVec2 distRectStart = ImVec2(nicknamePos.x - ImGui::CalcTextSize(distStr).x / 2.0f - 2.0f, nicknamePos.y + 6 - 2.0f);
				ImVec2 distRectEnd = ImVec2(nicknamePos.x + ImGui::CalcTextSize(distStr).x / 2.0f + 2.0f, nicknamePos.y + 6 + ImGui::CalcTextSize(distStr).y + 2.0f);

				if (distRectEnd.x - distRectStart.x > 8 && distRectEnd.y - distRectStart.y > 8)
					drawList->AddRectFilled(distRectStart, distRectEnd, ImColor(Float4ToImColor(config->playerNicknamePlateColour)), 4.0f);

				drawList->AddText(ImVec2(nicknamePos.x - ImGui::CalcTextSize(distStr).x / 2.0f, nicknamePos.y + 6), playerNameColour, distStr);
			}

			// Nickname
			ImVec2 nameRectStart = ImVec2(nicknamePos.x - ImGui::CalcTextSize(client_it.second->nickname.c_str()).x / 2.0f - 2.0f, nicknamePos.y - 15 - 2.0f);
			ImVec2 nameRectEnd = ImVec2(nicknamePos.x + ImGui::CalcTextSize(client_it.second->nickname.c_str()).x / 2.0f + 2.0f, nicknamePos.y - 15 + ImGui::CalcTextSize(client_it.second->nickname.c_str()).y + 2.0f);

			if (nameRectEnd.x - nameRectStart.x > 8 && nameRectEnd.y - nameRectStart.y > 8)
				drawList->AddRectFilled(nameRectStart, nameRectEnd, ImColor(Float4ToImColor(config->playerNicknamePlateColour)), 4.0f);

			nicknamePos.x -= ImGui::CalcTextSize(client_it.second->nickname.c_str()).x / 2.0f;
			nicknamePos.y -= 15;

			drawList->AddText(nicknamePos, playerNameColour, client_it.second->nickname.c_str());
		}
	}
}

/*
UTILS
*/

// Returns the screen position of the top vertex
ImVec2 BaseUI::DrawOctahedron(ImDrawList* drawList, ImVec2 screenSize, Matrix4* viewMatrix, Vector3* position, float scale, ImColor color, float thickness) {
	Vector3 verts[6];
	ImVec2 points[6];

	verts[0] = *position; verts[0].y += 2.0f * scale;
	verts[1] = *position; verts[1].x += 1.0f * scale;
	verts[2] = *position; verts[2].z += 1.0f * scale;
	verts[3] = *position; verts[3].x -= 1.0f * scale;
	verts[4] = *position; verts[4].z -= 1.0f * scale;
	verts[5] = *position; verts[5].y -= 2.0f * scale;

	for (int i = 0; i < sizeof(verts) / sizeof(Vector3); i++) {
		Vector2 tempPoint;
		DXWorldToScreen(viewMatrix, verts[i], screenSize.x, screenSize.y, tempPoint);
		points[i].x = tempPoint.x;
		points[i].y = tempPoint.y;
	}

	drawList->AddLine(points[0], points[1], color, thickness);
	drawList->AddLine(points[0], points[2], color, thickness);
	drawList->AddLine(points[0], points[3], color, thickness);
	drawList->AddLine(points[0], points[4], color, thickness);

	drawList->AddLine(points[1], points[2], color, thickness);
	drawList->AddLine(points[2], points[3], color, thickness);
	drawList->AddLine(points[3], points[4], color, thickness);
	drawList->AddLine(points[4], points[1], color, thickness);

	drawList->AddLine(points[5], points[1], color, thickness);
	drawList->AddLine(points[5], points[2], color, thickness);
	drawList->AddLine(points[5], points[3], color, thickness);
	drawList->AddLine(points[5], points[4], color, thickness);

	return points[0];
}

ImVec2 BaseUI::DrawBox(ImDrawList* drawList, ImVec2 screenSize, Matrix4* viewMatrix, Vector3* position, float scale, ImColor color, float thickness) {
	Vector3 verts[2];
	ImVec2 points[2];

	verts[0] = *position; 
	verts[0].y += 2.0f * scale;
	verts[1] = *position; 
	verts[1].x += 1.0f * scale;

	Vector2 tempPoint;
	DXWorldToScreen(viewMatrix, verts[0], screenSize.x, screenSize.y, tempPoint);
	points[0].x = tempPoint.x;
	points[0].y = tempPoint.y;

	DXWorldToScreen(viewMatrix, verts[1], screenSize.x, screenSize.y, tempPoint);
	points[1].x = tempPoint.x;
	points[1].y = tempPoint.y;

	drawList->AddLine(points[0], points[1], color, thickness);

	return points[0];
}

/*
Notifications
*/

void BaseUI::DrawNotifications(ImGuiIO io, ImDrawList* drawList) {
	if (!drawNotif) return;
	if (notifRemainingTime > 0) {
		if (notifRemainingTime < notifFadeOut) {
			notifOpacityPerc -= io.DeltaTime / notifFadeOut;
		}
		else if (notifOpacityPerc < 1.0f) {
			notifOpacityPerc += io.DeltaTime / notifFadeIn;
		}

		ImVec2 textSize = ImGui::CalcTextSize(notifMessage);
		drawList->AddRectFilled(ImVec2(20, 20), ImVec2(20 + textSize.x + NOTIF_PADDING * 2, 20 + textSize.y + NOTIF_PADDING * 2), ImColor(notifBgCol.Value.x, notifBgCol.Value.y, notifBgCol.Value.z, notifBgCol.Value.w * notifOpacityPerc), NOTIF_PADDING);
		drawList->AddText(ImVec2(20 + NOTIF_PADDING, 20 + NOTIF_PADDING), ImColor(notifTextCol.Value.x, notifTextCol.Value.y, notifTextCol.Value.z, notifTextCol.Value.w * notifOpacityPerc), notifMessage);
		notifRemainingTime -= io.DeltaTime;
	}
	else {
		drawNotif = false;
	}
}

void BaseUI::CreateNotification(char* message) {
	if (strlen(message) > sizeof(notifMessage)) return;
	strcpy_s(notifMessage, message);

	drawNotif = true;
	notifFadeIn = 0.1f;
	notifFadeOut = 0.5f;
	notifBgCol = ImColor(0.2f, 0.2f, 0.2f, 0.9f);
	notifTextCol = ImColor(1.0f, 1.0f, 1.0f, 1.0f);
	notifRemainingTime = 4.0f;
}

void BaseUI::CreateNotification(char* message, float duration) {
	if (strlen(message) > sizeof(notifMessage)) return;
	strcpy_s(notifMessage, message);

	drawNotif = true;
	notifFadeIn = 0.1f;
	notifFadeOut = 0.5f;
	notifBgCol = ImColor(0.2f, 0.2f, 0.2f, 0.9f);
	notifTextCol = ImColor(1.0f, 1.0f, 1.0f, 1.0f);
	notifRemainingTime = duration;
}

void BaseUI::CreateNotification(char* message, ImColor bgCol, ImColor textCol) {
	if (strlen(message) > sizeof(notifMessage)) return;
	strcpy_s(notifMessage, message);

	drawNotif = true;
	notifBgCol = bgCol;
	notifTextCol = textCol;
	notifFadeIn = 0.1f;
	notifFadeOut = 0.5f;
	notifRemainingTime = 4.0f;
}

void BaseUI::CreateNotification(char* message, float duration, float fadeIn, float fadeOut) {
	if (strlen(message) > sizeof(notifMessage)) return;
	strcpy_s(notifMessage, message);

	drawNotif = true;
	notifBgCol = ImColor(0.2f, 0.2f, 0.2f, 0.9f);
	notifTextCol = ImColor(1.0f, 1.0f, 1.0f, 1.0f);
	notifRemainingTime = duration;
}

void BaseUI::CreateNotification(char* message, float duration, float fadeIn, float fadeOut, ImColor bgCol, ImColor textCol) {
	if (strlen(message) > sizeof(notifMessage)) return;
	strcpy_s(notifMessage, message);

	drawNotif = true;
	notifBgCol = bgCol;
	notifTextCol = textCol;
	notifFadeIn = fadeIn;
	notifFadeOut = fadeOut;
	notifRemainingTime = duration;
}

/*
Countdown
*/
void ImguiCircleSegmented(ImDrawList* drawList, float x, float y, float radius, int segments, float perc) {
	return;
}

void BaseUI::DrawCountdown(ImGuiIO io, ImDrawList* drawList, float width, float height) {
	if (!drawCountdown) return;

	if (waitingCountdown) {
		auto tp = std::chrono::system_clock::now();
		auto dur = tp.time_since_epoch();
		unsigned int seconds = std::chrono::duration_cast<std::chrono::seconds>(dur).count();

		if (seconds != countdownStartTime) return;
		waitingCountdown = false;
	}

	float ceilRemaining = ceil(countdownRemainingTime);
	if (ceilRemaining > 0) {
		char buf[32] = "";
		sprintf_s(buf, sizeof(buf), "%.0f", ceilRemaining);

		ImGui::PushFont(rodinProMFontBig);
		ImVec2 textSize = ImGui::CalcTextSize(buf);
		drawList->AddCircleFilled(ImVec2(width / 2, height / 5), 100, ImColor(0.1f, 0.1f, 0.1f, 0.3f), 50);
		drawList->AddText(ImVec2(width / 2 - textSize.x / 2, height / 5 - textSize.y / 2), ImColor(1.0f, 1.0f, 1.0f, 1.0f), buf);
		drawList->PathArcTo(ImVec2(width / 2, height / 5), 100, -1.570796, 6.283185 * (countdownRemainingTime / countdownTme) - 1.570796, 50);
		drawList->PathStroke(ImColor(1.0f, 1.0f, 1.0f, 1.0f), 0, 5.0f);
		ImGui::PopFont();

		countdownRemainingTime -= io.DeltaTime;
		if (countdownRemainingTime <= 0) {
			PlaySound(TEXT("SystemAsterisk"), NULL, SND_ALIAS | SND_ASYNC);
		}
	}
	else if (countdownGoTime > 0) {

		ImGui::PushFont(rodinProMFontBig);
		ImVec2 textSize = ImGui::CalcTextSize("GO");
		drawList->AddCircleFilled(ImVec2(width / 2, height / 5), 100, ImColor(0.1f, 0.1f, 0.1f, 0.3f * countdownOpacityPerc), 50);
		drawList->AddText(ImVec2(width / 2 - textSize.x / 2, height / 5 - textSize.y / 2), ImColor(0.0f, 1.0f, 0.0f, countdownOpacityPerc), "GO");
		ImGui::PopFont();

		countdownGoTime -= io.DeltaTime;
		countdownOpacityPerc -= io.DeltaTime * 2;
	}
	else {
		drawCountdown = false;
	}
}

void BaseUI::QueueCountdown(unsigned int time) {
	waitingCountdown = true;
	countdownStartTime = time;

	drawCountdown = true;
	countdownTme = 5.0;
	countdownRemainingTime = countdownTme;
	countdownGoTime = 2.0f;
	countdownOpacityPerc = 1.0f;
}

/*
Theme
*/

void BaseUI::SetupTheme() {
	ImGuiStyle* style = &ImGui::GetStyle();
	style->WindowRounding = 0.0f;
	style->FrameRounding = 0.0f;
	style->ScrollbarRounding = 0.0;

	ImVec4* colors = style->Colors;

	ImColor darkGrey = ImColor(18, 18, 18, 240);
	ImColor lighterGrey = ImColor(30, 30, 30, 250);

	float hue;
	float sat;
	float val;
	ImGui::ColorConvertRGBtoHSV(config->menuAccentColour[0], config->menuAccentColour[1], config->menuAccentColour[2], hue, sat, val);
	ImColor accent = ImColor(config->menuAccentColour[0], config->menuAccentColour[1], config->menuAccentColour[2], 250 / 255.0f);
	ImColor accentHover = ImColor::HSV(hue, sat, val + 0.1f, 245 / 255.0f);
	ImColor accentActive = ImColor::HSV(hue, sat, val - 0.18f, 245 / 255.0f);
	ImColor accentBright = ImColor::HSV(hue, sat, val + 0.22f, 245 / 255.0f);

	colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
	colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
	colors[ImGuiCol_WindowBg] = lighterGrey;
	colors[ImGuiCol_ChildBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	colors[ImGuiCol_PopupBg] = ImVec4(0.08f, 0.08f, 0.08f, 0.94f);
	colors[ImGuiCol_Border] = ImColor(0, 0, 0, 0);
	colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	colors[ImGuiCol_FrameBg] = darkGrey;
	colors[ImGuiCol_FrameBgHovered] = accentHover;
	colors[ImGuiCol_FrameBgActive] = accentActive;
	colors[ImGuiCol_TitleBg] = lighterGrey;
	colors[ImGuiCol_TitleBgActive] = lighterGrey;
	colors[ImGuiCol_TitleBgCollapsed] = ImColor(30, 30, 30, 200);
	colors[ImGuiCol_MenuBarBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
	colors[ImGuiCol_ScrollbarBg] = ImVec4(0.02f, 0.02f, 0.02f, 0.53f);
	colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
	colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
	colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.51f, 0.51f, 0.51f, 1.00f);
	colors[ImGuiCol_CheckMark] = accentBright;
	colors[ImGuiCol_SliderGrab] = accentHover;
	colors[ImGuiCol_SliderGrabActive] = accentActive;
	colors[ImGuiCol_Button] = accent;
	colors[ImGuiCol_ButtonHovered] = accentHover;
	colors[ImGuiCol_ButtonActive] = accentActive;
	colors[ImGuiCol_Header] = accent;
	colors[ImGuiCol_HeaderHovered] = accentHover;
	colors[ImGuiCol_HeaderActive] = accentActive;
	colors[ImGuiCol_Separator] = colors[ImGuiCol_Border];
	colors[ImGuiCol_SeparatorHovered] = accentHover;
	colors[ImGuiCol_SeparatorActive] = accentActive;
	colors[ImGuiCol_ResizeGrip] = accent;
	colors[ImGuiCol_ResizeGripHovered] = accentHover;
	colors[ImGuiCol_ResizeGripActive] = accentActive;
	colors[ImGuiCol_Tab] = darkGrey;
	colors[ImGuiCol_TabHovered] = accentHover;
	colors[ImGuiCol_TabActive] = accent;
	colors[ImGuiCol_TabUnfocused] = darkGrey;
	colors[ImGuiCol_TabUnfocusedActive] = accent;
	colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
	colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
	colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
	colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
	colors[ImGuiCol_TableHeaderBg] = ImVec4(0.19f, 0.19f, 0.20f, 1.00f);
	colors[ImGuiCol_TableBorderStrong] = ImVec4(0.31f, 0.31f, 0.35f, 1.00f);   // Prefer using Alpha=1.0 here
	colors[ImGuiCol_TableBorderLight] = ImVec4(0.23f, 0.23f, 0.25f, 1.00f);   // Prefer using Alpha=1.0 here
	colors[ImGuiCol_TableRowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.00f, 1.00f, 1.00f, 0.06f);
	colors[ImGuiCol_TextSelectedBg] = accentHover;
	colors[ImGuiCol_DragDropTarget] = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
	colors[ImGuiCol_NavHighlight] = accentHover;
	colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
	colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
	colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
}