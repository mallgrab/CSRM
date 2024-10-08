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
	if (!ImGui::Begin("CSRM", &data->uiToggle, 0)) { ImGui::End(); return; }
	//Show build date, would've liked this in the title bar but doing so would bloat the imgui.ini file with every compile.
	ImGui::Text(__DATE__ " - " __TIME__);
	
	ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;
	if (ImGui::BeginTabBar("Tabs", tab_bar_flags))
	{
		DebugTab();
		LootDropTab();
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
//utility function that handles dropshadow
static inline void ImGui_DrawStringWithDropShadow(ImDrawList *drawList, const ImVec2 &pos, const ImVec4 &color, const char *text)
{
	drawList->AddText({ pos.x + 2.0f, pos.y + 2.0f }, ImGui::ColorConvertFloat4ToU32({ 0.0f, 0.0f, 0.0f, color.w }), text);
	drawList->AddText(pos, ImGui::ColorConvertFloat4ToU32(color), text);
}

static inline void ImGui_DrawStringWithDropShadow(const ImVec2 &pos, const ImVec4 &color, const char *text)
{
	ImGui_DrawStringWithDropShadow(ImGui::GetForegroundDrawList(), pos, color, text);
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

static void CSRM_Speedometer(ControlGameData *gameData, ControlConfig *config, ImGuiIO *io)
{
	static float topSpeed = 0;
	float drawSpeed, drawTopSpeed;
	int len = 0;
	char text[MAX_SPEED_LEN];
	ImVec4 color;
	ImVec2 pos, textSize;
	ImFont *font;

	if (!mapIsLoaded)
		return; //don't draw if map isn't loaded

	const float speed = gameData->getPlayerPhysxSpeed();
	//const float speed = gameData->getPlayerPosSpeed();

	if (isnan(speed))
		return;
	else if (speed > 100) //assume it's reading some whacky value
		return;

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
	else if (topSpeed > 6.0f && speed < 6.0f) //reset if well below sprinting speed
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
		static float groundSpeed = 0;

		if (delta >= 33) //30hz..
		{
			const bool onGround = gameData->playerIsOnGround();
			static bool lastOnGround = onGround;

			//printf("playerIsOnGround %s\n", onGround ? "yes" : "no");
			if (!onGround && lastOnGround) { //airborne this frame while we were grounded last frame
				drawTime = curTime + 1500;
				groundSpeed = speed;
			}
			/*else if (onGround && lastOnGround) {
				drawTime = 0; //don't draw
			}*/

			lastOnGround = onGround;
			lastUpdateTime = curTime;
		}

		if (curTime <= drawTime) {
			len += sprintf_s(text+len, sizeof(text)-len,
							 config->speedometerType != 1 ? "  %.0f" : "  %.02f", CSRM_SpeedometerSpeed(groundSpeed, config->speedometerType));
		}
	}

	//set font scale
	font = ImGui::GetFont();
	font->Scale = config->speedometerSize;
	ImGui::PushFont(font);
	ImGui::PopFont();

	textSize = ImGui::CalcTextSize(text);
	pos = { (config->speedometerPos[0] - (textSize.x * 0.5f)), (config->speedometerPos[1] - (textSize.y * 0.5f)) };
	color = CSRM_SpeedColor(speed, config->speedometerColorSpeed);
	ImDrawList *drawList = ImGui::GetForegroundDrawList();
	ImGui_DrawStringWithDropShadow(drawList, pos, color, text);

	//reset font scale
	font->Scale = 1.0f;
	ImGui::PushFont(font);
	ImGui::PopFont();

	if (!config->speedometerConsolePrint && !config->speedometerGraph) return; //prolly remove this in the future

#define SPEED_SAMPLES 512 //has to be divisible by 128?
#define SPEED_GRAPH_BASE_WIDTH 480 //should prolly scale this as well?
	//print to console window, throttle at 30hz?
	static int baseTime = clock();
	static int lastUpdateTime = 0;
	int curTime = clock() - baseTime;
	static float prevSpeed = speed;
	static float speedHistory[SPEED_SAMPLES];
	static int speedHistoryIndex = 0;
	bool update30Hz = (bool)(curTime - lastUpdateTime >= 33); //~30hz

	if (speed != prevSpeed)
		update30Hz = true; //reset if speed has changed since last update

	if (update30Hz)
	{
		lastUpdateTime = curTime;

		if (config->speedometerGraph) //update graph data
			speedHistory[speedHistoryIndex++ & (SPEED_SAMPLES - 1)] = speed;

		if (config->speedometerConsolePrint)
		{
			if (prevSpeed == speed && speed < 0.01f)
			{
				baseTime = clock(); //reset timer when coming to a complete stop
				lastUpdateTime = 0;
				prevSpeed = speed;
			}
			else //don't return early
			{
				float seconds = 0.0f;
				if (curTime > 0)
					seconds = (float)curTime / 1000.0f;

				//actually print the shit
				if (config->speedometerType != 1)
					printf("XYVel - %.03f: %.0f - %s\n", seconds, drawSpeed, gameData->playerIsOnGround() ? "grounded" : "airborne");
				else
					printf("XYVel - %.03f: %.02f - %s\n", seconds, drawSpeed, gameData->playerIsOnGround() ? "grounded" : "airborne");
			}
		}
		prevSpeed = speed;
	}

	if (config->speedometerGraph)
	{ //draw the graph
		int i;
		ImVec2 p1, p2;
		float scale = config->speedometerSize;
		float graphWidth = SPEED_GRAPH_BASE_WIDTH * config->speedometerSize;
		if (graphWidth > SPEED_SAMPLES) {
			graphWidth = SPEED_SAMPLES;
			scale = (float)(SPEED_SAMPLES/SPEED_GRAPH_BASE_WIDTH);
		}
		const float graphStartX = (io->DisplaySize.x * 0.5f) + (graphWidth * 0.5f);
		const static ImU32 colorGreen = ImGui::ColorConvertFloat4ToU32(ImVec4(0, 255, 0, 255));
		const static ImU32 colorYellow = ImGui::ColorConvertFloat4ToU32(ImVec4(255, 255, 0, 255));

		for (i = 0; i < graphWidth; i++)
		{ //loop through speedHistory, draw line by line right to left
			int index = (speedHistoryIndex - 1 - i) & (SPEED_SAMPLES - 1);
			float h = speedHistory[index];
			ImU32 graphColor = colorGreen;

			if (h > 50) { //cap this off
				h = 50;
				graphColor = colorYellow;
			}

			h *= 2.0f; //double it so it's a little easier to read - should this be a config setting?
			h *= config->speedometerSize; //scale it up kinda?

			p1.x = p2.x = graphStartX - (float)(i * scale);
			p1.y = p2.y = io->DisplaySize.y;
			p2.y -= h;
			if (h > 0)
				drawList->AddLine(p1, p2, graphColor, scale);
		}
	}
}
#undef MAX_SPEED_LEN
#undef GROUND_SPEED
#endif

#define DRAWFPS_SHAPEENGINE 1 //still a little fucked cuz we're drawing the FPS reported by ImGui, but ist fine i suppose
static void CSRM_DrawFPS(ControlGameData *gameData, ControlConfig *config, ControlUI *ui, ImGuiIO *io) {
	char text[16];
	int cornerPos = config->drawFPSPos;
#if DRAWFPS_SHAPEENGINE
	void *instance = ShapeEngine::getInstance();
	Vector2 pos, textSize;
	rend::ShapeEngine::Pivot pivot;
#else
	const ImVec4 color = { 1.0f, 1.0f, 1.0f, 1.0f };
	ImVec2 pos, textSize;
	ImFont *font;
#endif

	if (config->playerListEnabled && cornerPos == config->playerListPos && ui->client->IsRunning() && ui->client->GetUserCount() > 0)
	{ //move to another corner so we don't cover up the player list
		switch (config->playerListPos)
		{ //this is kinda bloat ngl
			default:
			case PLAYERLIST_TOPLEFT:
			case PLAYERLIST_BOTTOMLEFT:
			case PLAYERLIST_BOTTOMRIGHT:
				cornerPos = PLAYERLIST_TOPRIGHT; break;
			case PLAYERLIST_TOPRIGHT:
				cornerPos = PLAYERLIST_BOTTOMRIGHT; break;
		}
	}

#if 0
	int len = sprintf_s(text, sizeof(text), "%.0ffps", io->Framerate);
	len += sprintf_s(text+len, sizeof(text)-len, " %.2fms", io->DeltaTime*1000.0f);
#else
	sprintf_s(text, sizeof(text), "%.0ffps", io->Framerate);
#endif

#if DRAWFPS_SHAPEENGINE
	if (instance == nullptr || ShapeEngine::rendFontPtr == nullptr)
		return;

	ShapeEngine::SetFont(instance, ShapeEngine::rendFontPtr);
	//textSize = ShapeEngine::getTextSize(text); //crashes 4 som reason
	textSize = { 0.04f, ShapeEngine::getTextLineHeight() };

	switch (cornerPos) {
		case PLAYERLIST_TOPLEFT:
			pivot = rend::ShapeEngine::Pivot::UPPER_LEFT;
			pos = { 0, 0 };
			break;
		default:
		case PLAYERLIST_TOPRIGHT:
			pivot = rend::ShapeEngine::Pivot::UPPER_RIGHT;
			pos = { 1.0f, 0.0f };
			break;
		case PLAYERLIST_BOTTOMLEFT:
			pivot = rend::ShapeEngine::Pivot::LEFT_LOWER;
			pos = { 0, (1.0f - textSize.y - 0.04f) };
			pos = { 0, 1.0f - 0.005f };
			break;
		case PLAYERLIST_BOTTOMRIGHT:
			pivot = rend::ShapeEngine::Pivot::RIGHT_LOWER;
			pos = { 1.0f - 0.005f, 1.0f - 0.005f };
			break;
	}

	ShapeEngine::drawText(&pos, text, pivot);
#else
	font = ImGui::GetFont();
	font->Scale = 2.0f;
	ImGui::PushFont(font);
	ImGui::PopFont();

	textSize = ImGui::CalcTextSize(text);

	switch (cornerPos) {
		case PLAYERLIST_TOPLEFT:
			pos = ImVec2(0, 0);
			break;
		default:
		case PLAYERLIST_TOPRIGHT:
			pos = ImVec2(io->DisplaySize.x - textSize.x - 4.0f, 4.0f);
			break;
		case PLAYERLIST_BOTTOMLEFT:
			pos = ImVec2(0, io->DisplaySize.y - textSize.y - 4.0f);
			break;
		case PLAYERLIST_BOTTOMRIGHT:
			pos = ImVec2(io->DisplaySize.x - textSize.x - 4.0f, io->DisplaySize.y - textSize.y - 4.0f);
			break;
	}

	ImGui_DrawStringWithDropShadow(pos, color, text);

	//reset font scale
	font->Scale = 1.0f;
	ImGui::PushFont(font);
	ImGui::PopFont();
#endif
}

static void CSRM_DrawPos(ControlGameData *gameData, ControlConfig *config, ImGuiIO *io) {
#if 1 //fixme: add camera/player angle
	int len = 0;
	char text[512];
	const ImVec4 color = { 1.0f, 1.0f, 1.0f, 1.0f };
	ImVec2 pos, textSize;
	ImFont *font;
	Vector3 playerPos = *gameData->GetPlayerPos(); //this is crashing..???
	Matrix4* myViewMatrix = gameData->GetViewMatrix();
	float speed = gameData->getPlayerPhysxSpeed();

	if (!myViewMatrix)
		return;

	len += sprintf_s(text+len, sizeof(text)-len,
					 "pos:  %.02f %.02f %.02f\n",
					 playerPos.x, playerPos.y, playerPos.z);

	/*len += sprintf_s(text+len, sizeof(text)-len,
					 "ang:  %.02f %.02f %.02f\n",
					 myViewMatrix->_30, myViewMatrix->_31, myViewMatrix->_32);*/ //FIXME: this is not the real view angle lol

	len += sprintf_s(text+len, sizeof(text)-len,
					 "vel:  %.02f\n", speed);
					 //"vel:  \xb5: %.02f\n", speed);

#if 1
	if (ShapeEngine::getInstance() == nullptr || ShapeEngine::rendFontPtr == nullptr)
		return;

	Vector2 pos2 = { 0.75f, 0.05f };

	//ShapeEngine::setFont(ShapeEngine::getInstance(), ShapeEngine::rendFontPtr);
	ShapeEngine::drawText(&pos2, text, rend::ShapeEngine::Pivot::CENTER_TOP);
	//
	font = ImGui::GetFont();
	textSize = ImGui::CalcTextSize(text);
	pos = { ((io->DisplaySize.x * 0.75f) - (textSize.x*0.5f)), 0.0f };
	ImGui_DrawStringWithDropShadow(pos, color, text);
	//
#else
	font = ImGui::GetFont();
	//font->Scale = 1.0f;
	//ImGui::PushFont(font);
	//ImGui::PopFont();

	textSize = ImGui::CalcTextSize(text);
	pos = { ((io->DisplaySize.x * 0.75f) - (textSize.x*0.5f)), 0.0f };
	ImGui_DrawStringWithDropShadow(pos, color, text);

	//reset font scale
	//font->Scale = 1.0f;
	//ImGui::PushFont(font);
	//ImGui::PopFont();
#endif
#endif
}

static void CSRM_DrawHUD(ControlGameData *gameData, ControlConfig *config, ControlUI *ui, ImGuiIO *io) {
	if (config->drawFPS) {
		CSRM_DrawFPS(gameData, config, ui, io);
	}

	if (!mapIsLoaded) {
		return;
	}

	if (config->showPos) {
		CSRM_DrawPos(gameData, config, io);
	}

	if (config->drawSpeedometer) {
		CSRM_Speedometer(gameData, config, io);
		//controlData->getPlayerPosSpeed();
	}
}

#define MAX_POSITION_SLOTS 10
static Vector3 savedPositions[MAX_POSITION_SLOTS];
static void CSRM_SavePosition(int slot, ControlGameData *gameData, ControlUI *ui)
{
	char buf[128];
	Vector3 curPos;

	if (!gameData || !ui)
		return;

	if (!mapIsLoaded)
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

	if (!mapIsLoaded)
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
	ControlConfig *cfg = (ControlConfig *)config;
	if (cfg->drawTriggersHotkey) {
		if (key == VK_INSERT) { //0x2D
			cfg->drawTriggers = !cfg->drawTriggers;
			return;
		}
	}

	if (cfg->saveLoadPosition)
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

	BaseUI::KeyPress(key);
}

void ControlUI::LootDropTab() {
	if (ImGui::BeginTabItem("LootDrop")) 
	{
		ImGui::Text("Replace _Nothing globalID with loot\n");

		// we can use lootTableIndex as a way to pick items from the dropdown list to replace the current drop
		// no idea how to restore the drop table globalid changes if we do this
		// maybe the pointer is the same until the game reloads? could just keep it in memory then
		ControlConfig *cfg = ((ControlConfig*)config);

		ImGui::Checkbox("Dream Seed Generator", &modifyLootDropsCheat);

		if (cfg->lootTableItemNames.size() != 0)
			ImGui::Combo("##LootDropCombo", &cfg->lootTableIndex, &cfg->lootTableItemNames[0], (int)cfg->lootTableItemNames.size());
		else
			ImGui::Text("lootdroptable has not been constructed yet\n");
		
		// TODO: try modifying the globalid table instead of the droptable and see if that changes things
		// we can atleast restore changes from that table 

		ImGui::EndTabItem();
	}
}

extern float countdownOpacityPerc;
extern float countdownGoTime;
extern float countdownTme;
void ControlUI::DebugTab() {
	if (ImGui::BeginTabItem("Debug"))
	{
		ImGuiIO& io = ImGui::GetIO();
		ImDrawList* drawList = ImGui::GetForegroundDrawList();

		Matrix4* myViewMatrix = data->GetViewMatrix();
		Vector3* myPlayerPos = data->GetPlayerPos();

		ControlGameData *controlData = ((ControlGameData*)data);
		ControlConfig *cfg = ((ControlConfig*)config);

		ImGui::Text("%.01f FPS\n", io.Framerate);
		ImGui::TextUnformatted("Screen Size:");
		ImGui::SameLine();
		ImGui::Text("%.0f, %.0f", io.DisplaySize.x, io.DisplaySize.y);

		/*
		ImGui::Checkbox("Override Resolution", &config->overrideResolution);
		ImGui::InputFloat("Custom Width", &config->customWidth);
		ImGui::InputFloat("Custom Height", &config->customHeight);
		*/

		//ImGui::Text("\n");
		if (ImGui::Checkbox("Motion blur", &cfg->motionBlur))
			MotionBlurTweakable.SetTweakableStrValue(cfg->motionBlur ? "0.4f" : "0.0f");
		ImGui::SameLine();
		if (ImGui::Checkbox("Temporal SSAA", &cfg->TemporalSSAA))
			SSAATweakable.SetTweakableStrValue(cfg->TemporalSSAA ? "1.0f" : "0.0f");
		//ImGui::SameLine();
		//if (ImGui::Checkbox("Pause on focus lost", &cfg->pauseOnFocusLost))
		//	controlData->UpdateStartupStringValues(cfg);

		ImGui::TextUnformatted("\n");
		ImGui::TextUnformatted("Game settings:");
		ImGui::Checkbox("Save/Load position", &cfg->saveLoadPosition);
		if (ImGui::Checkbox("Disable levitation", &cfg->disableLevitation))
			AbilityLevitateTweakable.SetTweakableStrValue(cfg->disableLevitation ? "1" : "0");

		ImGui::TextUnformatted("\nHUD:");
		ImGui::Checkbox("Draw FPS", &cfg->drawFPS);
		ImGui::SameLine();
		if (cfg->drawFPS) {
			ImGui::Combo("##FPSPosFields", &cfg->drawFPSPos,
						 "Top left\0Top right\0Bottom left\0Bottom right\0\0");
		}
		ImGui::Checkbox("Show Pos", &cfg->showPos);

		ImGui::TextUnformatted("Speedometer:");
		ImGui::Checkbox("Draw Speedometer", &cfg->drawSpeedometer);
		if (cfg->drawSpeedometer) {
			ImGui::SameLine();
			ImGui::Checkbox("Speed graph", &cfg->speedometerGraph);
			ImGui::Checkbox("Speed Color", &cfg->speedometerColorSpeed);
			ImGui::SameLine();
			ImGui::Checkbox("Show Top Speed", &cfg->speedometerShowTopSpeed);
			ImGui::SameLine();
			ImGui::Checkbox("Show Ground Speed", &cfg->speedometerGroundSpeed);

			ImGui::TextUnformatted("Unit type:");
			ImGui::SameLine();
			ImGui::Combo("##UnitTypeCombo", &cfg->speedometerType, "Normal\0Raw\0Quake Units\0\0");

			//ImGui::InputFloat("X", &config->speedometerX, 1.0f, 2.0f); //change this to use InputFloat2
			//ImGui::InputFloat("Y", &config->speedometerY, 1.0f, 2.0f);
			ImGui::TextUnformatted("Position:");
			ImGui::SameLine();
			ImGui::InputFloat2("##SpeedometerPosFields", cfg->speedometerPos, "%.2f", ImGuiInputTextFlags_CharsScientific);
			ImGui::TextUnformatted("Size:");
			ImGui::SameLine();
			ImGui::InputFloat("", &cfg->speedometerSize, 0.125f, 0.25f);

			ImGui::Checkbox("Print speed to console", &cfg->speedometerConsolePrint);
		}

		ImGui::TextUnformatted("\n");
		ImGui::Text("Triggers (%i):", TriggerGetCount());
		ImGui::Checkbox("Enable Triggers", &cfg->drawTriggers);
		ImGui::SameLine();
		ImGui::Checkbox("Ins toggle", &cfg->drawTriggersHotkey);
		if (cfg->drawTriggers) {
			ImGui::TextUnformatted("Trigger Opacity:");
			ImGui::SliderFloat("##TriggerOpacitySlider", &cfg->triggerOpacity, 0.0f, 1.0f, "%.3f");
		}
		//ImGui::Text("\n");
		ImGui::TextUnformatted("Developer features:");
		if (ImGui::Button("Toggle Freecam")) controlData->ToggleFreeCam();
		ImGui::SameLine();
		if (ImGui::Button("Toggle developer menus")) controlData->ToggleDeveloperMenus();

		//ImGui::TextUnformatted("\n");
		ImGui::TextUnformatted("\nDebug:");
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

		ImGui::InputFloat("Countdown Time", &countdownTme);
		if (ImGui::Button("Start Countdown test")) {
			drawCountdown = true;
			countdownRemainingTime = countdownTme;
			countdownGoTime = 2.0f;
			countdownOpacityPerc = 1.0f;
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

static inline void CSRM_DrawTriggers(ControlGameData *gameData, ControlConfig *config, ImDrawList *drawList, int screenWidth, int screenHeight)
{
	Matrix4* myViewMatrix = gameData->GetViewMatrix();
	Vector3 worldPos;
	ImVec2 nicknamePos;

	if (!myViewMatrix)
		return;

	if (!mapIsLoaded || !TriggerGetVector().size())
		return;

	//Vector3 playerPos = *gameData->GetPlayerPos();
	auto triggers = TriggerGetVector();
	for (int i = 0; i < triggers.size(); i++)
	{
		ImColor triggerColor;

		if ((triggers.at(i) == nullptr) || (triggers.at(i)->ComponentBaseState() == nullptr))
			continue;

		if (triggers.at(i)->ComponentBaseState()->onEnter == 1)
			triggerColor = ImColor(1.0f, 1.0f, 1.0f, config->triggerOpacity * 0.9f);
		else if (triggers.at(i)->ComponentBaseState()->onEnter != 0)
			triggerColor = ImColor(0.7f, 1.0f, 0.7f, config->triggerOpacity * 0.5f);
		else
			triggerColor = ImColor(1.0f, 1.0f, 1.0f, config->triggerOpacity * 0.2f);

		Vector3 minA = { triggers.at(i)->boundbox.points[0], triggers.at(i)->boundbox.points[1], triggers.at(i)->boundbox.points[2] };
		Vector3 maxB = { triggers.at(i)->boundbox.points[3], triggers.at(i)->boundbox.points[4], triggers.at(i)->boundbox.points[5] };

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
	ImGuiIO &io = ImGui::GetIO();
	static ImVec2 lastDisplaySize = io.DisplaySize;

	ControlConfig *cfg = (ControlConfig *)config;
	//printf("ControlUI::Init()\n");

	if (cfg->speedometerSize == -1.0f || //config uninitialized
		cfg->speedometerPos[0] < 0.0f || cfg->speedometerPos[0] > io.DisplaySize.x || //config values gone wrong
		cfg->speedometerPos[1] < 0.0f || cfg->speedometerPos[1] > io.DisplaySize.y ||
		lastDisplaySize.y != io.DisplaySize.y || lastDisplaySize.x != io.DisplaySize.x) //window size changed
	{
		float yscale = io.DisplaySize.y * (1.0f/720.0f); //scale from 720p, should probably factor in DisplayFramebufferScale as well

		//printf("resolution changed or something\n");

		if (yscale < 0.1f)
			yscale = 0.1f;
		else if (yscale > 10.0f)
			yscale = 10.0f;

		cfg->speedometerPos[0] = io.DisplaySize.x * 0.5f;
		cfg->speedometerPos[1] = 75 * yscale;

		cfg->speedometerSize = yscale;

		lastDisplaySize = io.DisplaySize;
	}

	if (init) return; //really don't need the ImGUI mousecursor at all with how input works rn

	MotionBlurTweakable.SetTweakableStrValue(cfg->motionBlur ? "0.4f" : "0.0f");
	SSAATweakable.SetTweakableStrValue(cfg->TemporalSSAA ? "1.0f" : "0.0f");

	ControlGameData *controlData = (ControlGameData *)data;
	controlData->UpdateStartupStringValues(cfg);

	BaseUI::Init();
}

void ControlUI::RenderOSD() {
	if (!init) return;
	ImDrawList *drawList = ImGui::GetForegroundDrawList();
	ImGuiIO &io = ImGui::GetIO();
	ControlGameData *controlData = (ControlGameData *)data;
	ControlConfig *cfg = (ControlConfig *)config;

	BaseUI::RenderOSD();

	if (cfg->drawTriggers) {
		CSRM_DrawTriggers(controlData, cfg, drawList, (int)screenWidth, (int)screenHeight);
	}
	CSRM_DrawHUD(controlData, cfg, this, &io);
}