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

	if (!gameData->GetMapIsLoaded())
		return; //don't draw if map isn't loaded

	const float speed = gameData->getPlayerPhysxSpeed();
	//const float speed = gameData->getPlayerPosSpeed();
	static float topSpeed = 0.0f, previousSpeed = speed;
	float drawSpeed, drawTopSpeed;
	int len = 0;
	char text[MAX_SPEED_LEN];
	ImVec4 color;
	ImVec2 pos, textSize;
	ImFont *font;

	if (config->speedometerSize < 1.0f || config->speedometerSize > 10.0f) //shuoldn't need this anymore 
		config->speedometerSize = 1.0f;

#if SAMPLE_POSITION_UPDATES == 1 //measure the rate at which position is updated..
	//position is updated at 30hz apparently..?
	//if (1)
	{
		#define POSITION_FRAMES 240
		static int deltas[POSITION_FRAMES] = { 0 };
		static int deltaIndex = 0, lastPositionUpdate = 0;
		int now = clock();

		Vector3 currentPos = *gameData->GetPlayerPos();
		static Vector3 previousPos = currentPos;

		if (!(currentPos == previousPos)) {
			int i, total = 0;
			float average = 0;
			const int positionDelta = now - lastPositionUpdate;

			deltas[deltaIndex++ % POSITION_FRAMES] = positionDelta;

			for (i = 0; i < POSITION_FRAMES; i++) total += deltas[i];
			if (total < 1) total = 1;
			average = 1000 * POSITION_FRAMES / (float)total;

			printf("positionUpdate %02i avg rate %02f\n", positionDelta, average);
			lastPositionUpdate = now;
			previousPos = currentPos;
		}
		#undef POSITION_FRAMES
	}
#endif

#if SAMPLE_POSITION_UPDATES == 2 //do same thing but for speed?
	//position is updated at 30hz apparently..?
	//if (1)
	{
		#define SPEED_FRAMES 60
		static int deltas[SPEED_FRAMES] = { 0 };
		static int deltaIndex = 0, lastSpeedUpdate = 0;
		int now = clock();

		static float previousSpeed = speed;

		if (!(speed == previousSpeed)) {
			int i, total = 0;
			float average = 0;
			const int speedDelta = now - lastSpeedUpdate;

			deltas[deltaIndex++ % SPEED_FRAMES] = speedDelta;

			for (i = 0; i < SPEED_FRAMES; i++) total += deltas[i];
			if (total < 1) total = 1;
			average = 1000 * SPEED_FRAMES / (float)total;

			printf("speedUpdate %02i avg rate %02f\n", speedDelta, average);
			lastSpeedUpdate = now;
			previousSpeed = speed;
		}
		#undef SPEED_FRAMES
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

		if (delta >= 33) //30hz..
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

		if (curTime - lastUpdateTime >= 33) //~30hz
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

#define MAX_POSITION_SLOTS 10
static Vector3 savedPositions[MAX_POSITION_SLOTS];
static void CSRM_SavePosition(int slot, ControlGameData *gameData, ControlUI *ui)
{
	char buf[128];
	Vector3 curPos;

	if (!gameData || !ui)
		return;

	if (!gameData->GetMapIsLoaded())
		return;

	if (slot < 0 || slot >= MAX_POSITION_SLOTS)
		return;

	curPos = *gameData->GetPlayerPos_Real();
#ifndef _DEBUG
	sprintf_s(buf, "Saving position to slot %i..\n", slot);
#else
	sprintf_s(buf, "Saving position to slot %i (%f %f %f)..\n", slot, curPos.x, curPos.y, curPos.z);
	printf(buf);
#endif
	ui->CreateNotification(buf); //, ImColor(0.2f, 0.2f, 0.2f), ImColor(1.0f, 1.0f, 0.0f));

	savedPositions[slot] = curPos;
}

static void CSRM_LoadPosition(int slot, ControlGameData *gameData, ControlUI *ui)
{
	char buf[128];

	if (!gameData || !ui)
		return;

	if (!gameData->GetMapIsLoaded())
		return;

	if (slot < 0 || slot >= MAX_POSITION_SLOTS)
		return;

	if (!savedPositions[slot].x && !savedPositions[slot].y && !savedPositions[slot].z)
		return; //empty position slot i guess

	//if (gameData->getPlayerPhysxSpeed() > 0) //for some reason, the new position doesn't stay if we set our position while moving
	//	return;

	if (savedPositions[slot] == *gameData->GetPlayerPos_Real()) //don't set it if we're already there
		return; //(this is just so the print message doesn't pop up)

#ifndef _DEBUG
	sprintf_s(buf, "Loading position from slot %i..\n", slot);
#else
	sprintf_s(buf, "Loading position %i (%f %f %f)..\n", slot, savedPositions[slot].x, savedPositions[slot].x, savedPositions[slot].z);
	printf(buf);
#endif
	ui->CreateNotification(buf); //, ImColor(0.2f, 0.2f, 0.2f), ImColor(1.0f, 1.0f, 0.0f));

	gameData->SetPlayerPos(savedPositions[slot]);
}

static int CSRM_SlotFromKey(WPARAM key)
{ //maps keycode to position slot
	switch (key)
	{
		case VK_F1:
		case 0x31: //1
			return 1;
		case VK_F2:
		case 0x32: //2
			return 2;
		case VK_F3:
		case 0x33: //3
			return 3;
		case VK_F4:
		case 0x34: //4
			return 4;
		case VK_F5:
		case 0x35: //5
			return 5;
		case VK_F6:
		case 0x36: //6
			return 6;
		case VK_F7:
		case 0x37: //7
			return 7;
		case VK_F8:
		case 0x38: //8
			return 8;
		case VK_F9:
		case 0x39: //9
			return 9;
		case VK_F10:
		case 0x30: //0
			return 0;
		default: break;
	}

	return 0;
}
#undef MAX_POSITION_SLOTS

void ControlUI::KeyPress(WPARAM key) {
	//printf("ControlUI::KeyPress %i\n", (int)key);
	if (key == config->menuKeybind) {
		data->uiToggle = !data->uiToggle;
		return;
	}

	if (config->drawTriggersHotkey) {
		if (key == 0x2D) {
			config->drawTriggers = !config->drawTriggers;
			return;
		}
	}

	if (config->saveLoadPosition)
	{
		if (key >= VK_F1 && key <= VK_F9) {
			CSRM_SavePosition(CSRM_SlotFromKey(key), (ControlGameData *)data, this);
			return;
		}
		if (key >= 0x30 && key <= 0x39) {
			CSRM_LoadPosition(CSRM_SlotFromKey(key), (ControlGameData *)data, this);
			return;
		}
	}
}

void ControlUI::DebugTab() {
	if (ImGui::BeginTabItem("Debug"))
	{
		ImGuiIO& io = ImGui::GetIO();
		ImDrawList* drawList = ImGui::GetForegroundDrawList();

		Matrix4* myViewMatrix = data->GetViewMatrix();
		Vector3* myPlayerPos = data->GetPlayerPos();

		ControlGameData *controlData = ((ControlGameData*)data);
		if (!controlData) return; //don't rlly need this but...

		/*
		ImGui::InputFloat("Countdown Time", &countdownTme);
		if (ImGui::Button("Start Countdown")) {
			drawCountdown = true;
			countdownRemainingTime = countdownTme;
			countdownGoTime = 2.0f;
			countdownOpacityPerc = 1.0f;
		}
		*/

		ImGui::Text("%.01f FPS\n", io.Framerate);
		ImGui::Text("Screen Size:");
		ImGui::SameLine();
		ImGui::Text("%.0f, %.0f", io.DisplaySize.x, io.DisplaySize.y);

		/*
		ImGui::Checkbox("Override Resolution", &config->overrideResolution);
		ImGui::InputFloat("Custom Width", &config->customWidth);
		ImGui::InputFloat("Custom Height", &config->customHeight);
		*/

		//ImGui::Text("\n");
		if (ImGui::Checkbox("Motion blur", &config->motionBlur))
			controlData->Tweakable_SetMotionBlur(config->motionBlur);
		ImGui::SameLine();
		if (ImGui::Checkbox("Temporal SSAA", &config->TemporalSSAA))
			controlData->Tweakable_SetTemporalSSAA(config->TemporalSSAA);

		ImGui::Text("\n");
		ImGui::Text("Game settings:");
		ImGui::Checkbox("Save/Load position", &config->saveLoadPosition);
		if (ImGui::Checkbox("Disable levitation", &config->disableLevitation))
			controlData->Tweakable_SetLevitateDisable(config->disableLevitation);

		ImGui::Text("\n");
		ImGui::Text("Triggers (%i):\n", controlData->getTriggerCount());
		ImGui::Checkbox("Enable Triggers", &config->drawTriggers);
		ImGui::SameLine();
		ImGui::Checkbox("Ins toggle", &config->drawTriggersHotkey);
		if (config->drawTriggers) {
			ImGui::Text("Trigger Opacity:");
			ImGui::SliderFloat("", &config->triggerOpacity, 0.0f, 1.0f, "%.3f");
		}

		ImGui::Text("\n");
		ImGui::Text("Speedometer:");
		ImGui::Checkbox("Draw Speedometer", &config->drawSpeedometer);
		if (config->drawSpeedometer) {
			ImGui::SameLine();
			ImGui::Checkbox("Speed Color", &config->speedometerColorSpeed);
			ImGui::Checkbox("Show Top Speed", &config->speedometerShowTopSpeed);
			ImGui::SameLine();
			ImGui::Checkbox("Show Ground Speed", &config->speedometerGroundSpeed);

			ImGui::Combo("Unit type:", &config->speedometerType, "Normal\0Raw\0Quake Units\0\0");

			//ImGui::InputFloat("X", &config->speedometerX, 1.0f, 2.0f); //change this to use InputFloat2
			//ImGui::InputFloat("Y", &config->speedometerY, 1.0f, 2.0f);
			ImGui::Text("X Pos:");
			ImGui::SameLine();
			ImGui::InputFloat("", &config->speedometerX, 1.0f, 2.0f); //change this to use InputFloat2
			ImGui::Text("Y Pos:");
			ImGui::SameLine();
			ImGui::InputFloat("", &config->speedometerY, 1.0f, 2.0f);
			ImGui::Text("Size:");
			ImGui::SameLine();
			ImGui::InputFloat("", &config->speedometerSize, 0.125f, 0.25f);

			ImGui::Checkbox("Print speed to console", &config->speedometerConsolePrint);
		}

		//ImGui::Text("\n");
		ImGui::Text("\nDebug:");
		if (myPlayerPos != nullptr) {
			ImGui::Text("PosX: %f", myPlayerPos->x);
			ImGui::Text("PosY: %f", myPlayerPos->y);
			ImGui::Text("PosZ: %f", myPlayerPos->z);
		}

		float *playerSpeedCapsule = controlData->getPlayerCapsuleSpeed();
		if (playerSpeedCapsule != nullptr) {
			ImGui::Text("speed capsule: %f %f %f %f",
						*playerSpeedCapsule,
						*(playerSpeedCapsule + 1),
						*(playerSpeedCapsule + 2),
						*(playerSpeedCapsule + 3));
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

static inline void CSRM_DrawTriggers(ControlGameData *gameData, BaseConfig *config, ImDrawList *drawList, int screenWidth, int screenHeight)
{
	Matrix4* myViewMatrix = gameData->GetViewMatrix();
	Vector3 worldPos;
	ImVec2 nicknamePos;

	if (!myViewMatrix)
		return;

	if (!gameData->GetMapIsLoaded() || !gameData->GetTriggers().size())
		return;

	//Vector3 playerPos = *gameData->GetPlayerPos();
	auto triggers = gameData->GetTriggers();
	for (int i = 0; i < triggers.size(); i++)
	{
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

		if (!DXWorldToScreen(myViewMatrix, minA, screenWidth, screenHeight, vec2_minA)) continue;
		if (!DXWorldToScreen(myViewMatrix, maxB, screenWidth, screenHeight, vec2_maxB)) continue;

		if (!DXWorldToScreen(myViewMatrix, minA_tmpA, screenWidth, screenHeight, vec2_minA_tmpA)) continue;
		if (!DXWorldToScreen(myViewMatrix, minA_tmpB, screenWidth, screenHeight, vec2_minA_tmpB)) continue;
		if (!DXWorldToScreen(myViewMatrix, minA_tmpC, screenWidth, screenHeight, vec2_minA_tmpC)) continue;

		if (!DXWorldToScreen(myViewMatrix, maxB_tmpA, screenWidth, screenHeight, vec2_maxB_tmpA)) continue;
		if (!DXWorldToScreen(myViewMatrix, maxB_tmpB, screenWidth, screenHeight, vec2_maxB_tmpB)) continue;
		if (!DXWorldToScreen(myViewMatrix, maxB_tmpC, screenWidth, screenHeight, vec2_maxB_tmpC)) continue;

		imvec_minA.x = vec2_minA.x; imvec_minA.y = vec2_minA.y;
		imvec_maxB.x = vec2_maxB.x; imvec_maxB.y = vec2_maxB.y;

		imvec_minA_tmpA.x = vec2_minA_tmpA.x; imvec_minA_tmpA.y = vec2_minA_tmpA.y;
		imvec_minA_tmpB.x = vec2_minA_tmpB.x; imvec_minA_tmpB.y = vec2_minA_tmpB.y;
		imvec_minA_tmpC.x = vec2_minA_tmpC.x; imvec_minA_tmpC.y = vec2_minA_tmpC.y;

		imvec_maxB_tmpA.x = vec2_maxB_tmpA.x; imvec_maxB_tmpA.y = vec2_maxB_tmpA.y;
		imvec_maxB_tmpB.x = vec2_maxB_tmpB.x; imvec_maxB_tmpB.y = vec2_maxB_tmpB.y;
		imvec_maxB_tmpC.x = vec2_maxB_tmpC.x; imvec_maxB_tmpC.y = vec2_maxB_tmpC.y;

		drawList->AddLine(imvec_minA, imvec_minA_tmpA, triggerColor, 1.0f);
		drawList->AddLine(imvec_minA_tmpB, imvec_minA_tmpC, triggerColor, 1.0f);
		drawList->AddLine(imvec_minA, imvec_minA_tmpB, triggerColor, 1.0f);
		drawList->AddLine(imvec_minA_tmpC, imvec_minA_tmpA, triggerColor, 1.0f);

		drawList->AddLine(imvec_maxB, imvec_maxB_tmpA, triggerColor, 1.0f);
		drawList->AddLine(imvec_maxB_tmpB, imvec_maxB_tmpC, triggerColor, 1.0f);
		drawList->AddLine(imvec_maxB, imvec_maxB_tmpB, triggerColor, 1.0f);
		drawList->AddLine(imvec_maxB_tmpC, imvec_maxB_tmpA, triggerColor, 1.0f);

		drawList->AddLine(imvec_maxB, imvec_minA_tmpC, triggerColor, 1.0f);
		drawList->AddLine(imvec_maxB_tmpA, imvec_minA_tmpB, triggerColor, 1.0f);

		drawList->AddLine(imvec_minA, imvec_maxB_tmpC, triggerColor, 1.0f);
		drawList->AddLine(imvec_minA_tmpA, imvec_maxB_tmpB, triggerColor, 1.0f);
	}
}

void ControlUI::Init()
{
	// despite being called "Init" this function is deceivingly called every frame
	// and returned out of early with the init check, so we'll check for resolution changes and such here
	ImGuiIO io = ImGui::GetIO();
	static ImVec2 lastDisplaySize = io.DisplaySize;

	//printf("ControlUI::Init()\n");

	if (config->speedometerSize == -1.0f || //config uninitialized
		config->speedometerX < 0.0f || config->speedometerX > io.DisplaySize.x || //config values gone wrong
		config->speedometerY < 0.0f || config->speedometerY > io.DisplaySize.y ||
		lastDisplaySize.y != io.DisplaySize.y || lastDisplaySize.x != io.DisplaySize.x) //window size changed
	{
		float yscale = io.DisplaySize.y * (1.0f/720.0f); //scale from 720p, should probably factor in DisplayFramebufferScale as well

		//printf("resolution changed or something\n");

		if (yscale < 1.0f || yscale > 10.0f)
			yscale = 1.0f;

		config->speedometerX = io.DisplaySize.x * 0.5f;
		config->speedometerY = 75 * yscale;

		config->speedometerSize = yscale;

		lastDisplaySize = io.DisplaySize;
	}

	if (init) return; //really don't need the ImGUI mousecursor at all with how input works
	BaseUI::Init();
	//config->ReadConfig();
}

void ControlUI::DrawPlayerObjects() {
	ImDrawList *drawList = ImGui::GetForegroundDrawList();
	ImGuiIO io = ImGui::GetIO();

	ControlGameData *controlData = (ControlGameData *)data;
	if (!controlData) return; //don't rlly need this but...

	if (config->drawSpeedometer) {
		CSRM_Speedometer(controlData, config, &io);
		//controlData->getPlayerPosSpeed();
	}

	if (config->drawTriggers) {
		CSRM_DrawTriggers(controlData, config, drawList, (int)screenWidth, (int)screenHeight);
	}

	if (client->status == "Connected") {
		BaseUI::DrawPlayerObjects();
	}
}