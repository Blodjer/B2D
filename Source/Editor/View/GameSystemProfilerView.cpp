#include "B2D_pch.h"
#include "GameSystemProfilerView.h"
#include "Core/Profiler.h"

void GameSystemProfilerView::Tick(float deltaTime)
{
    if (ImGui::Begin("Game System Profiler"))
    {
        static uint64 lastFrameId = -1;
        static std::vector<float> frameDurations;
        static uint32 idx = 0;
        static size_t maxidx = 0;

        TimeStamp frameStartTimeStamp;
        TimeStamp frameEndTimeStamp;
        GameSystemStatisticDataList const* statistics = nullptr;
        uint64 frameId = Profiler::GetGameSystemStatistics(statistics, frameStartTimeStamp, frameEndTimeStamp);

        std::chrono::duration<float, std::milli> const chronoframeDuration = frameEndTimeStamp - frameStartTimeStamp;
        float frameDuration = chronoframeDuration.count();

        if (frameId != lastFrameId)
        {
            if (idx < frameDurations.size())
            {
                frameDurations[idx] = frameDuration;
            }
            else
            {
                frameDurations.emplace_back(frameDuration);
                maxidx++;
            }
            idx++;

            lastFrameId = frameId;
        }

        float accFrameDuration = 0;
        float minFrameDuration = FLT_MAX;
        float maxFrameDuration = FLT_MIN;

        size_t frameListSize = UMath::Min(maxidx, frameDurations.size());
        for (uint32 i = 0; i < frameListSize; ++i)
        {
            float d = frameDurations[i];
            accFrameDuration += d;

            if (d < minFrameDuration)
            {
                minFrameDuration = d;
            }

            if (d > maxFrameDuration)
            {
                maxFrameDuration = d;
            }
        }
        float avgFrameDuration = accFrameDuration / frameDurations.size();

        float accSinceLastReset = 0;
        for (uint32 i = 0; i < idx; ++i)
        {
            accSinceLastReset += frameDurations[i];
        }
        if (accSinceLastReset > 1500)
        {
            idx = 0;
            maxidx = frameDurations.size();
        }

        ImGui::Text("Delta:     %.3fms", frameDuration);
        ImGui::Text("Avg Delta: %.3fms", avgFrameDuration);
        ImGui::Text("Min Delta: %.3fms", minFrameDuration);
        ImGui::Text("Max Delta: %.3fms", maxFrameDuration);
        ImGui::NewLine();

        static std::unordered_map<std::thread::id, uint> threadSet;
        uint threadCount = threadSet.size();

        ImVec2 const cursorPos = ImGui::GetCursorPos();
        float const maxWidth = ImGui::GetContentRegionAvailWidth();

        ImGui::BeginGroup();
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
        for (GameSystemStatisticData const& data : *statistics)
        {
            float duration = std::chrono::duration<float, std::milli>(data.duration).count();

            uint threadIndex;

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

            std::chrono::duration<float, std::milli> const durationSinceFrameStart = data.startTimestamp - frameStartTimeStamp;
            float const startPosRatio = durationSinceFrameStart.count() / frameDuration;
            float const widthRatio = duration / frameDuration;

            ImGui::SetCursorPos(ImVec2(startPosRatio * maxWidth, cursorPos.y + threadIndex * ImGui::GetItemRectSize().y + 2));

            switch (data.type)
            {
                case GameSystemStatisticData::EType::ThreadOverhead:
                    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.5f, 0.2f, 0.2f, 0.6f));
                    break;
                case GameSystemStatisticData::EType::System:
                default:
                    ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyleColorVec4(ImGuiCol_Button));
                    break;
            }

            ImGui::Button(data.label, ImVec2(widthRatio * maxWidth, 0));
            ImGui::PopStyleColor();

            if (ImGui::IsItemHovered())
            {
                ImGui::BeginTooltip();
                ImGui::SetTooltip("%s\n%.3fms", data.label, duration);
                ImGui::EndTooltip();
            }
        }
        ImGui::PopStyleVar();
        ImGui::EndGroup();
    }

    ImGui::End();
}
