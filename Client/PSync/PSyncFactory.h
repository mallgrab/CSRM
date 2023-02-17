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
#include "GameData/ControlGameData.h"
#include "UI/ControlUI.h"
#include "Client/ControlClient.h"

class PSyncFactory {
private:
    OcularHook oHook;

public:
	inline void PSyncMod(OcularHook ocular) {
        oHook = ocular;
        wchar_t path[FILENAME_MAX];
        wchar_t filename[FILENAME_MAX];

        GetModuleFileName(NULL, path, MAX_PATH);
        _wsplitpath_s(path, NULL, NULL, NULL, NULL, filename, FILENAME_MAX, NULL, NULL);

#if 1
        std::wcout << "control injected" << std::endl;
        static ControlGameData data;
        static BaseConfig config;
        static ControlUI ui;
        static ControlClient client;

        config.ui = &ui;

        ui.data = &data;
        ui.config = &config;
        ui.client = &client;

        client.data = &data;
        client.ui = &ui;
        client.config = &config;

        data.InitGameData();
        // config.ReadConfig(); for later, not important atm

        oHook.SetImGuiWndProcHandlerToggle(&data.uiToggle);

        oHook.BindPreframeFunction(std::bind(&ControlUI::Init, &ui));
        oHook.BindKeypressFunction(std::function<void(WPARAM)>(std::bind(&ControlUI::KeyPress, &ui, std::placeholders::_1)));
        oHook.BindRenderFunction(std::function<void()>(std::bind(&ControlUI::RenderOSD, &ui)));
        oHook.BindRenderFunction(std::function<void()>(std::bind(&ControlUI::RenderGUI, &ui)));

        if (config.connectOnStart) {
            unsigned int playerColour = ImGui::ColorConvertFloat4ToU32(Float3AToImColor(config.myColour, 1.0f));
            unsigned int playerTrailColour = ImGui::ColorConvertFloat4ToU32(Float3AToImColor(config.myTrailColour, 1.0f));
            client.StartClient(config.serverIP, atoi(config.serverPort), config.nickname, playerColour, playerTrailColour);
        }
#else
        //std::wcout << filename << std::endl;
        if (wcscmp(filename, L"NieRAutomata") == 0) {
            static NieRAutomataGameData data;
            static NieRAutomataConfig config;
            static NieRAutomataUI ui;
            static NieRAutomataClient client;

            data.config = &config;

            config.ui = &ui;

            ui.data = &data;
            ui.config = &config;
            ui.client = &client;

            client.data = &data;
            client.ui = &ui;
            client.config = &config;

            config.ReadConfig();
            data.InitGameData();

            oHook.SetImGuiWndProcHandlerToggle(&data.uiToggle);

            oHook.BindPreframeFunction(std::bind(&NieRAutomataUI::Init, &ui));
            oHook.BindKeypressFunction(std::function<void(WPARAM)>(std::bind(&NieRAutomataUI::KeyPress, &ui, std::placeholders::_1)));
            oHook.BindRenderFunction(std::function<void()>(std::bind(&NieRAutomataUI::RenderOSD, &ui)));
            oHook.BindRenderFunction(std::function<void()>(std::bind(&NieRAutomataUI::RenderGUI, &ui)));

            if (config.connectOnStart) {
                unsigned int playerColour = ImGui::ColorConvertFloat4ToU32(Float3AToImColor(config.myColour, 1.0f));
                unsigned int playerTrailColour = ImGui::ColorConvertFloat4ToU32(Float3AToImColor(config.myTrailColour, 1.0f));
                client.StartClient(config.serverIP, atoi(config.serverPort), config.nickname, playerColour, playerTrailColour);
            }
        }
        else if (wcscmp(filename, L"NieR Replicant ver.1.22474487139") == 0) {
            static NieRReplicantGameData data;
            static BaseConfig config;
            static BaseUI ui;
            static BaseClient client;

            config.ui = &ui;

            ui.data = &data;
            ui.config = &config;
            ui.client = &client;

            client.data = &data;
            client.ui = &ui;
            client.config = &config;

            data.InitGameData();
            config.ReadConfig();

            oHook.SetImGuiWndProcHandlerToggle(&data.uiToggle);

            oHook.BindPreframeFunction(std::bind(&BaseUI::Init, &ui));
            oHook.BindKeypressFunction(std::function<void(WPARAM)>(std::bind(&BaseUI::KeyPress, &ui, std::placeholders::_1)));
            oHook.BindRenderFunction(std::function<void()>(std::bind(&BaseUI::RenderOSD, &ui)));
            oHook.BindRenderFunction(std::function<void()>(std::bind(&BaseUI::RenderGUI, &ui)));

            if (config.connectOnStart) {
                unsigned int playerColour = ImGui::ColorConvertFloat4ToU32(Float3AToImColor(config.myColour, 1.0f));
                unsigned int playerTrailColour = ImGui::ColorConvertFloat4ToU32(Float3AToImColor(config.myTrailColour, 1.0f));
                client.StartClient(config.serverIP, atoi(config.serverPort), config.nickname, playerColour, playerTrailColour);
            }
        }
        else if (wcscmp(filename, L"DarkSoulsIII") == 0) {
            static DarkSouls3GameData data;
            static BaseConfig config;
            static BaseUI ui;
            static BaseClient client;

            config.ui = &ui;

            ui.data = &data;
            ui.config = &config;
            ui.client = &client;

            client.data = &data;
            client.ui = &ui;
            client.config = &config;

            data.InitGameData();
            config.ReadConfig();

            oHook.SetImGuiWndProcHandlerToggle(&data.uiToggle);

            oHook.BindPreframeFunction(std::bind(&BaseUI::Init, &ui));
            oHook.BindKeypressFunction(std::function<void(WPARAM)>(std::bind(&BaseUI::KeyPress, &ui, std::placeholders::_1)));
            oHook.BindRenderFunction(std::function<void()>(std::bind(&BaseUI::RenderOSD, &ui)));
            oHook.BindRenderFunction(std::function<void()>(std::bind(&BaseUI::RenderGUI, &ui)));

            if (config.connectOnStart) {
                unsigned int playerColour = ImGui::ColorConvertFloat4ToU32(Float3AToImColor(config.myColour, 1.0f));
                unsigned int playerTrailColour = ImGui::ColorConvertFloat4ToU32(Float3AToImColor(config.myTrailColour, 1.0f));
                client.StartClient(config.serverIP, atoi(config.serverPort), config.nickname, playerColour, playerTrailColour);
            }
        }
        else if (wcscmp(filename, L"dmc3se") == 0 || wcscmp(filename, L"dmc3se_nosteam") == 0) {
            static DevilMayCry3SEGameData data;
            static DevilMayCry3SEConfig config;
            static DevilMayCry3SEUI ui;
            static BaseClient client;

            config.ui = &ui;

            ui.data = &data;
            ui.config = &config;
            ui.client = &client;

            client.data = &data;
            client.ui = &ui;
            client.config = &config;

            data.InitGameData();
            config.ReadConfig();

            if (config.loadStyleSwitcher) {
                HINSTANCE styleSwitcherDLL = LoadLibrary(L"StyleSwitcher.dll");
                if (styleSwitcherDLL != NULL) { std::cout << "[DMC3SE SE] StyleSwitcher loaded!" << std::endl; }
                else { std::cout << "[DMC3SE SE] Could not load StyleSwitcher!" << std::endl; }
            }

            oHook.SetImGuiWndProcHandlerToggle(&data.uiToggle);

            oHook.BindPreframeFunction(std::bind(&BaseUI::Init, &ui));
            oHook.BindKeypressFunction(std::function<void(WPARAM)>(std::bind(&BaseUI::KeyPress, &ui, std::placeholders::_1)));
            oHook.BindRenderFunction(std::function<void()>(std::bind(&BaseUI::RenderOSD, &ui)));
            oHook.BindRenderFunction(std::function<void()>(std::bind(&BaseUI::RenderGUI, &ui)));

            if (config.connectOnStart) {
                unsigned int playerColour = ImGui::ColorConvertFloat4ToU32(Float3AToImColor(config.myColour, 1.0f));
                unsigned int playerTrailColour = ImGui::ColorConvertFloat4ToU32(Float3AToImColor(config.myTrailColour, 1.0f));
                client.StartClient(config.serverIP, atoi(config.serverPort), config.nickname, playerColour, playerTrailColour);
            }
        }
        else if (wcscmp(filename, L"Control_DX11") == 0) {
            std::wcout << "control injected" << std::endl;
            static ControlGameData data;
            static BaseConfig config;
            static ControlUI ui;
            static ControlClient client;

            config.ui = &ui;

            ui.data = &data;
            ui.config = &config;
            ui.client = &client;

            client.data = &data;
            client.ui = &ui;
            client.config = &config;

            data.InitGameData();
            config.ReadConfig();

            oHook.SetImGuiWndProcHandlerToggle(&data.uiToggle);

            oHook.BindPreframeFunction(std::bind(&ControlUI::Init, &ui));
            oHook.BindKeypressFunction(std::function<void(WPARAM)>(std::bind(&ControlUI::KeyPress, &ui, std::placeholders::_1)));
            oHook.BindRenderFunction(std::function<void()>(std::bind(&ControlUI::RenderOSD, &ui)));
            oHook.BindRenderFunction(std::function<void()>(std::bind(&ControlUI::RenderGUI, &ui)));

            if (config.connectOnStart) {
                unsigned int playerColour = ImGui::ColorConvertFloat4ToU32(Float3AToImColor(config.myColour, 1.0f));
                unsigned int playerTrailColour = ImGui::ColorConvertFloat4ToU32(Float3AToImColor(config.myTrailColour, 1.0f));
                client.StartClient(config.serverIP, atoi(config.serverPort), config.nickname, playerColour, playerTrailColour);
            }
        }
        else {
            std::wcout << "Executable filename not recognized: " << filename << std::endl;
            return;
        }
#endif
	}
};