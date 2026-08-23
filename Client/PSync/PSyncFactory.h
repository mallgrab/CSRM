#pragma once
#include <Windows.h>
#include <iostream>
#include <cstdint>
#include <functional>

#include "../ocular/ocular.h"

#if _WIN32 || _WIN64
#if _WIN64
typedef uint64_t ptr;
#else
typedef uint32_t ptr;
#define ENVIRONMENT32
#endif
#endif

// NieRAutomata
#include "GameData/NieRAutomataGameData.h"
#include "Config/NieRAutomataConfig.h"
#include "UI/NieRAutomataUI.h"
#include "Client/NieRAutomataClient.h"

// Dark Souls 3
#include "GameData/DarkSouls3GameData.h"

// NieR Replicant ver.1.22474487139
#include "GameData/NieRReplicantGameData.h"

// Devil May Cry 3 SE
#include "GameData/DarkSouls3GameData.h"
#include "UI/DevilMayCry3SEUI.h"
#include "Config/DevilMayCry3SEConfig.h"

// Control
#include "GameData/Control/GameData.h"
#include "UI/ControlUI.h"
#include "Client/ControlClient.h"
#include "Config/ControlConfig.h"

class PSyncFactory {
private:
    OcularHook oHook;

public:
	inline void PSyncMod(OcularHook ocular, wchar_t *path, wchar_t *filename) {
        oHook = ocular;
#if 0 //moved to main.cpp
        wchar_t path[FILENAME_MAX];
        wchar_t filename[FILENAME_MAX];

        GetModuleFileName(NULL, path, MAX_PATH);
        _wsplitpath_s(path, NULL, NULL, NULL, NULL, filename, FILENAME_MAX, NULL, NULL);
#else
        //assert(filename);
#endif

    if (wcscmp(filename, L"Control_DX11") == 0) {
            std::wcout << "control injected" << std::endl;
            static ControlGameData data;
            static ControlConfig config;
            static ControlUI ui;
            static ControlClient client;

            config.ui = &ui;

            ui.data = &data;
            ui.config = &config;
            ui.client = &client;

            client.data = &data;
            client.ui = &ui;
            client.config = &config;

            config.ReadConfig();
            cfg = &config;
         
            data.InitGameData();

            oHook.SetImGuiWndProcHandlerToggle(&data.uiToggle);

            oHook.BindPreframeFunction(std::bind(&ControlUI::Init, &ui));
            oHook.BindKeypressFunction(std::function<void(WPARAM)>(std::bind(&ControlUI::KeyPress, &ui, std::placeholders::_1)));
            oHook.BindRenderFunction(std::function<void()>(std::bind(&ControlUI::RenderOSD, &ui)));

            oHook.BindRenderFunction(std::function<void()>(std::bind(&ControlUI::RenderGUI, &ui)));

#ifndef _DEBUG
            if (config.connectOnStart) {
                unsigned int playerColour = ImGui::ColorConvertFloat4ToU32(Float3AToImColor(config.myColour, 1.0f));
                unsigned int playerTrailColour = ImGui::ColorConvertFloat4ToU32(Float3AToImColor(config.myTrailColour, 1.0f));
                client.StartClient(config.serverIP, atoi(config.serverPort), config.nickname, playerColour, playerTrailColour);
            }
#endif
        }
        else {
            std::wcout << "Executable filename not recognized: " << filename << std::endl;
            return;
        }
	}
};