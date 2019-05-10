#include "B2D_pch.h"
#include "GameSystemProfilerView.h"
#include "Core/Profiler.h"

GameSystemProfilerView::GameSystemProfilerView()
{
}


GameSystemProfilerView::~GameSystemProfilerView()
{
}

void GameSystemProfilerView::Tick(float deltaTime)
{
    if (ImGui::Begin("Game System Profiler"))
    {
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));

        ProfilerData const& statistics = Profiler::GetLastFrame();

        static std::unordered_map<std::thread::id, uint32> threadSet;
        uint32 threadCount = threadSet.size();

        ImVec2 regionMin = ImGui::GetWindowContentRegionMin();

        TimeStamp startTimeStamp = TimeStamp::max();
        TimeStamp endTimeStamp = TimeStamp::min();
        for (StatisticData const& data : statistics)
        {
            if (data.startTimestamp < startTimeStamp)
            {
                startTimeStamp = data.startTimestamp;
            }

            if ((data.startTimestamp + data.duration) > endTimeStamp)
            {
                endTimeStamp = data.startTimestamp + data.duration;
            }
        }
        Duration const duration = endTimeStamp - startTimeStamp;
        std::chrono::duration<float, std::milli> const md = endTimeStamp - startTimeStamp;

        float maxWidth = ImGui::GetContentRegionAvailWidth();

        ImGui::BeginGroup();
        for (StatisticData const& data : statistics)
        {
            uint32 threadIndex;

            auto const it = threadSet.find(data.thread);
            if (it != threadSet.end())
            {
                threadIndex = it->second;
            }
            else
            {
                threadSet.insert(std::make_pair(data.thread, threadCount));
                threadIndex = threadCount;
                threadCount++;
            }

            auto b = (data.startTimestamp - startTimeStamp);
            std::chrono::duration<float, std::milli> m = b;

            float startPos = (m.count() / md.count());
            float width = std::chrono::duration<float, std::milli>(data.duration).count() / md.count();

            ImGui::SetCursorPos(ImVec2(startPos * maxWidth, regionMin.y + threadIndex * ImGui::GetItemsLineHeightWithSpacing() + 2));
            ImGui::Button(data.tag, ImVec2(width * maxWidth, 0));
            if (ImGui::IsItemHovered())
            {
                ImGui::BeginTooltip();
                ImGui::SetTooltip("%s\n%.3fms", data.tag, std::chrono::duration<float, std::milli>(data.duration).count());
                ImGui::EndTooltip();
            }
        }
        ImGui::EndGroup();

        ImGui::PopStyleVar();
    }

    ImGui::End();
}
