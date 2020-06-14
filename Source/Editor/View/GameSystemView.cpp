#include "B2D_pch.h"
#include "GameSystemView.h"
#include "GameEngine.h"
#include "Game/GameInstance.h"
#include "Game/Core/World.h"
#include "Game/Core/System.h"
#include "Game/Core/SystemAdmin.h"

void GameSystemView::Tick(float deltaTime)
{
    SystemAdmin& systemAdmin = GameEngine::Instance()->GetGameInstance()->GetWorld()->GetSystemAdmin();

    ImGui::SetNextWindowSizeConstraints(ImVec2(200, 200), ImVec2(9999, 9999));
    if (ImGui::Begin("Systems"))
    {
        // System Admin Settings
        ImGui::BeginGroup();

        ImGui::Checkbox("Enable Multithreading", &systemAdmin.m_multithreaded);

        ImGui::Checkbox("Use Optimized Order", &systemAdmin.m_useOptimizedSystems);

        ImGui::NewLine();

        ImGui::EndGroup();

        // System Overview
        ImGui::BeginGroup();

        ImGui::Columns(4);
        ImGui::Text("System");
        ImGui::NextColumn();
        ImGui::Text("Read");
        ImGui::NextColumn();
        ImGui::Text("Write");
        ImGui::NextColumn();
        //ImGui::Text("Multithreaded");
        char const* const multithreadText = "Multithreaded";
        if (systemAdmin.m_multithreaded)
        {
            ImGui::Text(multithreadText);
        }
        else
        {
            ImGui::TextDisabled(multithreadText);
        }
        ImGui::Separator();
        ImGui::NextColumn();

        std::vector<System*> const& systems = systemAdmin.m_useOptimizedSystems ? systemAdmin.m_systemsOptimized : systemAdmin.m_systemsRaw;
        for (System* s : systems)
        {
            ImGui::Text(s->GetName());

            ImGui::NextColumn();
            PrintMask(s->GetReadMask());

            ImGui::NextColumn();
            PrintMask(s->GetWriteMask());

            ImGui::NextColumn();
            char const* const multithreadText = s->IsMultithreaded() ? "Yes" : "No";
            if (systemAdmin.m_multithreaded)
            {
                ImGui::Text(multithreadText);
            }
            else
            {
                ImGui::TextDisabled(multithreadText);
            }

            ImGui::NextColumn();
        }
        ImGui::EndGroup();
    }
    ImGui::End();
}

static uint16 selectedComponent = -1;
void GameSystemView::PrintMask(uint16 mask)
{
    for (uint32 i = 0; i < 16; ++i)
    {
        bool selected = i == selectedComponent;
        bool binary = (mask >> i) & 1;

        ImVec4 color = binary ? ImVec4(1, 1, 1, 1) : ImVec4(0.5f, 0.5f, 0.5f, 1);
//         if (selected)
//         {
//             color = binary ? ImVec4(0, 1, 1, 1) : ImVec4(0, 0.5f, 0.5f, 1);
//         }

        ImGui::TextColored(color, binary ? "1" : "0");

        if (!selected && ImGui::IsItemHovered())
        {
            selectedComponent = i;
        }

        ImGui::SameLine(0.0f, 3.0f);
    }
}
