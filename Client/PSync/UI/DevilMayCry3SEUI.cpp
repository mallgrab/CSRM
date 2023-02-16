#include "DevilMayCry3SEUI.h"

void DevilMayCry3SEUI::RenderGUI() {
    if (!init) return;

    gHue++;
    gHue = gHue % 360;

    if (!data->uiToggle) {
        return;
    }

    ImGuiIO& io = ImGui::GetIO();

        ImGui::SetNextWindowSizeConstraints(ImVec2(520.0f, 150.0f), io.DisplaySize);
        if (!ImGui::Begin("DMC3 SE SE", &data->uiToggle, 0)) { ImGui::End(); return; }
        ImGui::Text(
            
"d8888b. .88b  d88.  .o88b. d8888b. .d8888. d88888b      .d8888. d88888b \n"
"88  `8D 88'YbdP`88 d8P  Y8 VP  `8D 88'  YP 88'          88'  YP 88'     \n"
"88   88 88  88  88 8P        oooY' `8bo.   88ooooo      `8bo.   88ooooo \n"
"88   88 88  88  88 8b        ~~~b.   `Y8b. 88~~~~~        `Y8b. 88~~~~~ \n"
"88  .8D 88  88  88 Y8b  d8 db   8D db   8D 88.          db   8D 88.     \n"
"Y8888D' YP  YP  YP  `Y88P' Y8888P' `8888Y' Y88888P      `8888Y' Y88888P "

        );

        ImGui::Text(" - Special Edition Speedrun Edition...");

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

void DevilMayCry3SEUI::DrawPlayerObjects() {
    ImDrawList* drawList = ImGui::GetForegroundDrawList();
    Matrix4* myViewMatrix = data->GetViewMatrix();
    if (myViewMatrix != nullptr) {
        Vector3 worldPos;
        ImVec2 nicknamePos;

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
            worldPos.y += 100;

            // Colours
            ImColor playerTrailColour = ImColor(Float3AToImColor(client_it.second->trailColour, config->playerTrailOpacity));
            ImColor playerObjColour = ImColor(Float3AToImColor(client_it.second->colour, config->playerObjectOpacity));
            ImColor playerNameColour = ImColor(Float3AToImColor(client_it.second->colour, 1.0f));

            // Special trail colours
            if (client_it.second->nickname == "Wolf") {
                playerTrailColour = ImColor::HSV(gHue / 360.0f, 1.0f, 1.0f, config->playerTrailOpacity);
            }

            // Trails
            if (config->playerTrailEnabled) {
                if (client_it.second->prevPos.size() > 3) {
                    for (size_t i = 0; i < client_it.second->prevPos.size() - 3; i++) {
                        if (client_it.second->prevPos[i] == client_it.second->prevPos[i + 1]) continue;

                        Vector3 pos1;
                        pos1.x = std::get<0>(client_it.second->prevPos[i]);
                        pos1.y = std::get<1>(client_it.second->prevPos[i]) + 100.0f;
                        pos1.z = std::get<2>(client_it.second->prevPos[i]);
                        Vector3 pos2;
                        pos2.x = std::get<0>(client_it.second->prevPos[i + 1]);
                        pos2.y = std::get<1>(client_it.second->prevPos[i + 1]) + 100.0f;
                        pos2.z = std::get<2>(client_it.second->prevPos[i + 1]);

                        //float fastDistance = (pow(pos1.x - pos2.x, 2) + pow(pos1.y - pos2.y, 2) + pow(pos1.z - pos2.z, 2)) * 100000;
                        //if (fastDistance < 0.5f) continue;

                        Vector2 point1;
                        Vector2 point2;
                        if (!GLWorldToScreen(myViewMatrix, ((DevilMayCry3SEGameData*)data)->GetFOV(),  pos1, screenWidth, screenHeight, point1)) continue;
                        if (!GLWorldToScreen(myViewMatrix, ((DevilMayCry3SEGameData*)data)->GetFOV(), pos2, screenWidth, screenHeight, point2)) continue;

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
                    distance = ((Vector3*)client_it.second->pos)->DistanceTo(*myPlayerPos)/100.0f;
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

ImVec2 DevilMayCry3SEUI::DrawOctahedron(ImDrawList* drawList, ImVec2 screenSize, Matrix4* viewMatrix, Vector3* position, float scale, ImColor color, float thickness) {
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
        GLWorldToScreen(viewMatrix, ((DevilMayCry3SEGameData*)data)->GetFOV(), verts[i], screenSize.x, screenSize.y, tempPoint);
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

void DevilMayCry3SEUI::ConfigTab() {
    if (ImGui::BeginTabItem("Configuration"))
    {
        if (ImGui::CollapsingHeader("Menu")) {
            if (ImGui::ColorEdit3("Accent Colour", config->menuAccentColour)) SetupTheme();
            ImGui::Checkbox("Draw ImGui Cursor", &config->drawImGuiCursor);
        }

        if (ImGui::CollapsingHeader("System")) {
            ImGui::Checkbox("Position Smoothing", &config->positionSmoothing);
            ImGui::Checkbox("Load StyleSwitcher on Startup (Requires Config Save and Restart)", &((DevilMayCry3SEConfig*)config)->loadStyleSwitcher);
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