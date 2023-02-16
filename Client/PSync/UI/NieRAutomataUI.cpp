#include "NieRAutomataUI.h"

void NieRAutomataUI::RenderGUI() {
    if (!init) return;

    gHue++;
    gHue = gHue % 360;

    if (!data->uiToggle) {
        return;
    }

    ImGuiIO& io = ImGui::GetIO();

    ImGui::SetNextWindowSizeConstraints(ImVec2(520.0f, 150.0f), io.DisplaySize);
    if (!ImGui::Begin("NARoIP", &data->uiToggle, 0)) { ImGui::End(); return; }

    ImGui::Text(
        "`7MN.   `7MF'     db      `7MM\"\"\"Mq.         `7MMF'`7MM\"\"\"Mq. \n"
        "  MMN.    M      ;MM:       MM   `MM.          MM    MM   `MM.\n"
        "  M YMb   M     ,V^MM.      MM   ,M9  ,pW\"Wq.  MM    MM   ,M9 \n"
        "  M  `MN. M    ,M  `MM      MMmmdM9  6W'   `Wb MM    MMmmdM9  \n"
        "  M   `MM.M    AbmmmqMA     MM  YM.  8M     M8 MM    MM       \n"
        "  M     YMM   A'     VML    MM   `Mb.YA.   ,A9 MM    MM       \n"
        ".JML.    YM .AMA.   .AMMA..JMML. .JMM.`Ybmd9'.JMML..JMML.     "
    );

    ImGui::Text(" - The Original OG Gangster...");

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

void NieRAutomataUI::ConfigTab() {
    if (ImGui::BeginTabItem("Configuration"))
    {
        if (ImGui::CollapsingHeader("Menu")) {
            if (ImGui::ColorEdit3("Accent Colour", config->menuAccentColour)) SetupTheme();
            ImGui::Checkbox("Draw ImGui Cursor", &config->drawImGuiCursor);
        }

        if (ImGui::CollapsingHeader("System")) {
            ImGui::Checkbox("Position Smoothing", &config->positionSmoothing);
            if (ImGui::Checkbox("Old Patch (v1.01) Compatibility Mode", &((NieRAutomataConfig*)config)->oldPatch)) {
                data->InitGameData();
            }
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