#include "ControlUI.h"

void ControlUI::RenderGUI() {
	if (!init) return;

	gHue++;
	gHue = gHue % 360;

	if (!data->uiToggle) {
		return;
	}

	ImGuiIO& io = ImGui::GetIO();

	ImGui::SetNextWindowSizeConstraints(ImVec2(520.0f, 150.0f), io.DisplaySize);
	if (!ImGui::Begin("Prac", &data->uiToggle, 0)) { ImGui::End(); return; }
	
	ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;
	if (ImGui::BeginTabBar("Tabs", tab_bar_flags))
	{
		DebugTab();
		ConnectionTab();
		CustomizationTab();
		ConfigTab();
		CreditsTab();
		ImGui::EndTabBar();
	}

	ImGui::End();
}

#if 0
void draw_speed(ImVec2 position, float speed) {
	char health_text[25];
	sprintf_s(health_text, 25, "%.2f", speed);
	ImGui::GetForegroundDrawList()->AddText({ (position.x - ImGui::CalcTextSize(health_text).x / 2.0f),
		(position.y - ImGui::CalcTextSize(health_text).y / 2.0f) }, ImGui::ColorConvertFloat4ToU32({ 1,1,1,1.0f }), health_text);
}
#else
static void ImGui_DrawStringWithDropShadow(const ImVec2 &pos, const ImVec4 &color, const char *text)
{ //utility function that handles dropshadow
	ImGui::GetForegroundDrawList()->AddText({ pos.x + 2.0f, pos.y + 2.0f }, ImGui::ColorConvertFloat4ToU32({ 0.0f, 0.0f, 0.0f, color.w}), text);
	ImGui::GetForegroundDrawList()->AddText(pos, ImGui::ColorConvertFloat4ToU32(color), text);
}

#define GROUND_SPEED 6.00f //4.50f
ImVec4 CSRM_SpeedColor(float speed, bool colorSpeed) { //color speed based on how fast > groundspeed
	ImVec4 color = { 1.0f, 1.0f, 1.0f, 1.0f };

	if (colorSpeed && speed > 0.0f)
	{
		const float speedFactor = speed/GROUND_SPEED;
		const float colorValue = 1.0f / (speedFactor*speedFactor);
		color = { 1.0f, colorValue, colorValue, 1.0f };
	}

	return color;
}

//todo.. groundspeed, colored u: based on acceleration/deceleration
#define MAX_SPEED_LEN 32
//control speedrun mod?
float CSRM_SpeedometerSpeed(float speed, int speedometerType)
{
	float retSpeed = speed;
	switch (speedometerType)
	{
		default: speedometerType = 1;
		case 1: //raw speed value from the game
			break;
		case 2: //regular speed * 50, seems close to quake (totally arbitrary & not scientific at all)
			retSpeed *= 0.5f;
		case 0: //regular speed * 100 making it more readable
			retSpeed *= 100.0f;
			break;
	}
	return retSpeed;
}

//static void CSRM_Speedometer(float speed, BaseConfig *config, ImGuiIO *io)
static void CSRM_Speedometer(ControlGameData *gameData, BaseConfig *config, ImGuiIO *io)
{
	if (!config || !io || !gameData)
		return;

	const float speed = gameData->getPlayerPhysxSpeed();
	//const float speed = gameData->getPlayerPosSpeed();
	static float topSpeed = 0.0f, previousSpeed = speed;
	float drawSpeed, drawTopSpeed;
	int len = 0;
	char text[MAX_SPEED_LEN];
	ImVec4 color;
	ImVec2 pos, textSize;
	ImFont *font;

	if (!gameData->GetMapIsLoaded())
		return; //don't draw if map isn't loaded

	if (config->speedometerSize < 1.0f || config->speedometerSize > 10.0f) //shuoldn't need this anymore 
		config->speedometerSize = 1.0f;

#if 0 //measure the rate at which position is updated..
	//position is updated at 30hz apparently..?
	if (1)
	{
		#define POSITION_FRAMES 240
		static int deltas[POSITION_FRAMES] = { 0 };
		static int deltaIndex = 0, lastUpdateTimee = 0;
		int now = clock();

		Vector3 currentPos = *gameData->GetPlayerPos();
		static Vector3 previousPos = currentPos;

		if (!(currentPos == previousPos)) {
			int i, total = 0;
			float average = 0;
			const int positionDelta = now - lastUpdateTimee;

			deltas[deltaIndex++ % POSITION_FRAMES] = positionDelta;

			for (i = 0; i < POSITION_FRAMES; i++) total += deltas[i];
			if (total < 1) total = 1;
			average = 1000 * POSITION_FRAMES / (float)total;

			printf("positionUpdate %02i avg rate %02f\n", positionDelta, average);
			lastUpdateTimee = now;
			previousPos = currentPos;
		}
		#undef POSITION_FRAMES 
	}
#endif

	//track top speed
	if (speed <= 0) //reset when not moving
		topSpeed = 0;
	else if (speed > topSpeed)
		topSpeed = speed;
	else if (topSpeed > 7.0f && speed <= 7.0f) //reset if below sprinting speed
		topSpeed = speed;

	//sprintf_s(text, sizeof(text), "%.2f", speed);
	drawSpeed = CSRM_SpeedometerSpeed(speed, config->speedometerType);
	drawTopSpeed = CSRM_SpeedometerSpeed(topSpeed, config->speedometerType);

	//sry about all the ternaries..
	if (config->speedometerShowTopSpeed && topSpeed > 0) {//append topspeed
		len += sprintf_s(text+len, sizeof(text)-len,
						 config->speedometerType != 1 ? "%.0f > " : "%.02f > ", drawTopSpeed);
	}

	len += sprintf_s(text+len, sizeof(text)-len,
					config->speedometerType != 1 ? "%.0f" : "%.02f", drawSpeed);

	if (config->speedometerGroundSpeed)
	{ //this is shitty and temporary but we'll use the Y coordinate to guess when we jump
		static int lastUpdateTime = 0;
		static int drawTime = 0;
		const int curTime = clock();
		const int delta = curTime - lastUpdateTime; //this shit HAS to be throttled I guess?

		static bool onGround = true;
		static float jumpSpeed = 0;
		static float jumpY = 0;

		if (delta >= 30) //30hz..
		{
			Vector3 pos = *gameData->GetPlayerPos();
			//static Vector3 previousPos = pos;
			const float yPos = pos.y;
			static float previousYPos = yPos;
			const float heightDiff = yPos - previousYPos;
			static float previousHeightDiff = heightDiff;

			if (yPos == previousYPos && pos.y <= jumpY) //not moving up or down and we're below where we were when we jumped
				onGround = true;

			if (heightDiff > 0 && previousHeightDiff < 0) //moving up after previously moving downward (hopped)
				onGround = true;

			if (onGround) {
				if (heightDiff > 0)
				{ //we're moving up for the first time so probably jump (stairs and shit are obviously gonna trigger this lol)
					onGround = false;
					drawTime = curTime + 1500;
				}
				/*else {
					drawTime = 0; //don't draw
				}*/

				jumpSpeed = speed;
				jumpY = yPos;
			}

			/*
			if (heightDiff)
				printf("%i heightDiff %f - jumpSpeed %f - onGround %s\n",
					   curTime, heightDiff, jumpSpeed, onGround ? "true" : "false");
			*/

			previousYPos = yPos;
			previousHeightDiff = heightDiff;
			lastUpdateTime = curTime;

		}

		if (curTime <= drawTime) {
			len += sprintf_s(text+len, sizeof(text)-len,
							 config->speedometerType != 1 ? "  %.0f" : "  %.02f", CSRM_SpeedometerSpeed(jumpSpeed, config->speedometerType));
		}
	}

	//ImGui::SetWindowFontScale(2.0f);
	font = ImGui::GetFont();
	font->Scale = config->speedometerSize;
	ImGui::PushFont(font);
	ImGui::PopFont();

	textSize = ImGui::CalcTextSize(text);
	pos = { (config->speedometerX - (textSize.x * 0.5f)), (config->speedometerY - (textSize.y * 0.5f)) };
	color = CSRM_SpeedColor(speed, config->speedometerColorSpeed);
	ImGui_DrawStringWithDropShadow(pos, color, text);

	//reset font scale
	font->Scale = 1.0f;
	ImGui::PushFont(font);
	ImGui::PopFont();
	//ImGui::SetWindowFontScale(1.0f);

	//print to console window, throttle at 60hz?
	if (config->speedometerConsolePrint)
	{
		static float prevSpeed = speed;
		//static const int baseTime = timeGetTime();
		static int baseTime = clock();
		static int lastUpdateTime = 0;
		int curTime = clock() - baseTime;

		if (curTime - lastUpdateTime >= 16) //62hz
		{
			float seconds = 0.0f;

			lastUpdateTime = curTime;
			if (prevSpeed == speed && speed < 0.01f) {
				baseTime = clock(); //reset timer when coming to a complete stop
				lastUpdateTime = 0;
				prevSpeed = speed;
				return;
			}

			prevSpeed = speed;

			if (curTime > 0) seconds = (float)curTime / 1000.0f;

			//actually print the shit
			if (config->speedometerType != 1)
				printf("XYVel - %.03f: %.0f\n", seconds, drawSpeed);
			else
				printf("XYVel - %.03f: %.02f\n", seconds, drawSpeed);
		}
	}
}
#undef MAX_SPEED_LEN
#undef GROUND_SPEED
#endif

void ControlUI::DebugTab() {
	if (ImGui::BeginTabItem("Debug"))
	{
		ImGuiIO& io = ImGui::GetIO();
		ImDrawList* drawList = ImGui::GetForegroundDrawList();

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

		if (((ControlGameData*)data)->getPlayerCapsuleSpeed() != nullptr)
			ImGui::Text("speed capsule: %f %f %f %f", 
				*((ControlGameData*)data)->getPlayerCapsuleSpeed(),
				*(((ControlGameData*)data)->getPlayerCapsuleSpeed() + 1),
				*(((ControlGameData*)data)->getPlayerCapsuleSpeed() + 2),
				*(((ControlGameData*)data)->getPlayerCapsuleSpeed() + 3));

		/*
		ImGui::Checkbox("Override Resolution", &config->overrideResolution);
		ImGui::InputFloat("Custom Width", &config->customWidth);
		ImGui::InputFloat("Custom Height", &config->customHeight);
		*/

		ImGui::Checkbox("Enable Triggers", &config->drawTriggers);
		ImGui::Text("\nTrigger Opacity:");
		ImGui::SliderFloat("", &config->triggerOpacity, 0.0f, 1.0f, "%.3f");

		ImGui::Text("\nSpeedometer:");
		ImGui::Checkbox("Enable Speedometer", &config->drawSpeedometer);
		ImGui::Checkbox("Show Ground Speed", &config->speedometerGroundSpeed);
		ImGui::Checkbox("Show Top Speed", &config->speedometerShowTopSpeed);
		ImGui::Checkbox("Speed Color", &config->speedometerColorSpeed);
		ImGui::Checkbox("Print speed to console", &config->speedometerConsolePrint);

		ImGui::Combo("Unit type:", &config->speedometerType, "Normal\0Raw\0Quake Units\0\0");
		ImGui::InputFloat("X", &config->speedometerX, 1.0f, 2.0f); //change this to use InputFloat2
		ImGui::InputFloat("Y", &config->speedometerY, 1.0f, 2.0f);
		ImGui::InputFloat("Size", &config->speedometerSize, 0.125f, 0.25f);

		//ImGui::Text("\n");
		ImGui::Text("\nDebug:");
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

			if (DXWorldToScreen(myViewMatrix, worldPos, (int)io.DisplaySize.x, (int)io.DisplaySize.y, scrnOutPos)) {
				ImGui::Text("\nScreenPos:");
				ImGui::Text("%f, %f", scrnOutPos.x, scrnOutPos.y);
			}
		}

		/*
		uint64_t* currentRegionAddress = nullptr;
		
		if (currentRegionAddress != nullptr) {
			ImGui::Text((char*)currentRegionAddress);
		}
		*/

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

void ControlUI::ConfigTab() {
	if (ImGui::BeginTabItem("Configuration"))
	{
		if (ImGui::CollapsingHeader("Menu")) {
			if (ImGui::ColorEdit3("Accent Colour", config->menuAccentColour)) SetupTheme();
			ImGui::Checkbox("Draw ImGui Cursor", &config->drawImGuiCursor);
		}

		if (ImGui::CollapsingHeader("System"))
			ImGui::Checkbox("Position Smoothing", &config->positionSmoothing);

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

static bool firstTimeInitalization = true;
void ControlUI::DrawPlayerObjects() {
	ImDrawList* drawList = ImGui::GetForegroundDrawList();
	ImGuiIO& io = ImGui::GetIO();

	if (config->drawSpeedometer)
	{
		//should move a lot of this stuff out of here tbh lol
		static ImVec2 lastDisplaySize = io.DisplaySize; //check if resolution/window size changed
		if (lastDisplaySize.y != io.DisplaySize.y || lastDisplaySize.x != io.DisplaySize.x)
			firstTimeInitalization = true;

		if (config->speedometerSize == -1.0f ||
			config->speedometerX < 0.0f || config->speedometerX > io.DisplaySize.x ||
			config->speedometerY < 0.0f || config->speedometerY > io.DisplaySize.y)
			firstTimeInitalization = true;

		if (firstTimeInitalization)			
		{
			float yscale = io.DisplaySize.y * (1.0f/720.0f); //scale from 720p, should probably factor in DisplayFramebufferScale as well
			if (yscale < 1.0f || yscale > 10.0f)
				yscale = 1.0f;

			config->speedometerX = io.DisplaySize.x * 0.5f;
			config->speedometerY = 75 * yscale;

			//if (config->speedometerSize == -1.0f)
				config->speedometerSize = yscale;

			lastDisplaySize = io.DisplaySize;

			firstTimeInitalization = false;
			//printf("resolution changed or something\n");
		}

		//draw_speed(((ControlGameData*)data)->getPlayerPhysxSpeed(), config, &io);
		CSRM_Speedometer((ControlGameData*)data, config, &io);
		//((ControlGameData*)data)->getPlayerPosSpeed();
	}
	 
	if (!config->drawTriggers)
		return;

	Matrix4* myViewMatrix = data->GetViewMatrix();
	if (myViewMatrix != nullptr) {
		Vector3 worldPos;
		ImVec2 nicknamePos;

		if (!((ControlGameData*)data)->GetMapIsLoaded())
			return;

		if (((ControlGameData*)data)->GetTriggers().size() != 0) {
			Vector3 playerPos = *data->GetPlayerPos();
			
			auto triggers = ((ControlGameData*)data)->GetTriggers();
			for (int i = 0; i < triggers.size(); i++) {
				ImColor triggerColor;

				if (triggers.at(i).trigger == NULL || 
					*triggers.at(i).genericentity == NULL ||
					triggers.at(i).triggerobject.trigger == nullptr)
					continue;

				if (*triggers.at(i).triggerobject.onEnter == 1)
					triggerColor = ImColor(1.0f, 1.0f, 1.0f, config->triggerOpacity * 0.9f);
				else if (*triggers.at(i).triggerobject.onEnter != 0)
					triggerColor = ImColor(0.7f, 1.0f, 0.7f, config->triggerOpacity * 0.5f);
				else
					triggerColor = ImColor(1.0f, 1.0f, 1.0f, config->triggerOpacity * 0.2f);

				Vector3 minA = triggers.at(i).triggerobject.minmax.a;
				Vector3 maxB = triggers.at(i).triggerobject.minmax.b;

				Vector3 minA_tmpA = minA; minA_tmpA.x = maxB.x;
				Vector3 minA_tmpB = minA; minA_tmpB.y = maxB.y;
				Vector3 minA_tmpC = minA; minA_tmpC.x = maxB.x; minA_tmpC.y = maxB.y;

				Vector3 maxB_tmpA = maxB; maxB_tmpA.x = minA.x;
				Vector3 maxB_tmpB = maxB; maxB_tmpB.y = minA.y;
				Vector3 maxB_tmpC = maxB; maxB_tmpC.x = minA.x; maxB_tmpC.y = minA.y;

				Vector2 vec2_minA;
				Vector2 vec2_maxB;

				Vector2 vec2_minA_tmpA;
				Vector2 vec2_minA_tmpB;
				Vector2 vec2_minA_tmpC;

				Vector2 vec2_maxB_tmpA;
				Vector2 vec2_maxB_tmpB;
				Vector2 vec2_maxB_tmpC;

				ImVec2 imvec_minA;
				ImVec2 imvec_maxB;

				ImVec2 imvec_minA_tmpA;
				ImVec2 imvec_minA_tmpB;
				ImVec2 imvec_minA_tmpC;

				ImVec2 imvec_maxB_tmpA;
				ImVec2 imvec_maxB_tmpB;
				ImVec2 imvec_maxB_tmpC;

				if (!DXWorldToScreen(myViewMatrix, minA, (int)screenWidth, (int)screenHeight, vec2_minA)) continue;
				if (!DXWorldToScreen(myViewMatrix, maxB, (int)screenWidth, (int)screenHeight, vec2_maxB)) continue;

				if (!DXWorldToScreen(myViewMatrix, minA_tmpA, (int)screenWidth, (int)screenHeight, vec2_minA_tmpA)) continue;
				if (!DXWorldToScreen(myViewMatrix, minA_tmpB, (int)screenWidth, (int)screenHeight, vec2_minA_tmpB)) continue;
				if (!DXWorldToScreen(myViewMatrix, minA_tmpC, (int)screenWidth, (int)screenHeight, vec2_minA_tmpC)) continue;

				if (!DXWorldToScreen(myViewMatrix, maxB_tmpA, (int)screenWidth, (int)screenHeight, vec2_maxB_tmpA)) continue;
				if (!DXWorldToScreen(myViewMatrix, maxB_tmpB, (int)screenWidth, (int)screenHeight, vec2_maxB_tmpB)) continue;
				if (!DXWorldToScreen(myViewMatrix, maxB_tmpC, (int)screenWidth, (int)screenHeight, vec2_maxB_tmpC)) continue;

				imvec_minA.x = vec2_minA.x; imvec_minA.y = vec2_minA.y;
				imvec_maxB.x = vec2_maxB.x; imvec_maxB.y = vec2_maxB.y;

				imvec_minA_tmpA.x = vec2_minA_tmpA.x; imvec_minA_tmpA.y = vec2_minA_tmpA.y;
				imvec_minA_tmpB.x = vec2_minA_tmpB.x; imvec_minA_tmpB.y = vec2_minA_tmpB.y;
				imvec_minA_tmpC.x = vec2_minA_tmpC.x; imvec_minA_tmpC.y = vec2_minA_tmpC.y;

				imvec_maxB_tmpA.x = vec2_maxB_tmpA.x; imvec_maxB_tmpA.y = vec2_maxB_tmpA.y;
				imvec_maxB_tmpB.x = vec2_maxB_tmpB.x; imvec_maxB_tmpB.y = vec2_maxB_tmpB.y;
				imvec_maxB_tmpC.x = vec2_maxB_tmpC.x; imvec_maxB_tmpC.y = vec2_maxB_tmpC.y;
				
				drawList->AddLine(imvec_minA,		imvec_minA_tmpA, triggerColor, 1.0f);
				drawList->AddLine(imvec_minA_tmpB,	imvec_minA_tmpC, triggerColor, 1.0f);
				drawList->AddLine(imvec_minA,		imvec_minA_tmpB, triggerColor, 1.0f);
				drawList->AddLine(imvec_minA_tmpC,	imvec_minA_tmpA, triggerColor, 1.0f);

				drawList->AddLine(imvec_maxB,		imvec_maxB_tmpA, triggerColor, 1.0f);
				drawList->AddLine(imvec_maxB_tmpB,	imvec_maxB_tmpC, triggerColor, 1.0f);
				drawList->AddLine(imvec_maxB,		imvec_maxB_tmpB, triggerColor, 1.0f);
				drawList->AddLine(imvec_maxB_tmpC,	imvec_maxB_tmpA, triggerColor, 1.0f);

				drawList->AddLine(imvec_maxB,		imvec_minA_tmpC, triggerColor, 1.0f);
				drawList->AddLine(imvec_maxB_tmpA,	imvec_minA_tmpB, triggerColor, 1.0f);

				drawList->AddLine(imvec_minA,		imvec_maxB_tmpC, triggerColor, 1.0f);
				drawList->AddLine(imvec_minA_tmpA,	imvec_maxB_tmpB, triggerColor, 1.0f);

			}
		}

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
			if (!DXWorldToScreen(myViewMatrix, worldPos, (int)screenWidth, (int)screenHeight, screenPos)) continue;

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

						Vector2 point1;
						Vector2 point2;
						if (!DXWorldToScreen(myViewMatrix, pos1, (int)screenWidth, (int)screenHeight, point1)) continue;
						if (!DXWorldToScreen(myViewMatrix, pos2, (int)screenWidth, (int)screenHeight, point2)) continue;

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