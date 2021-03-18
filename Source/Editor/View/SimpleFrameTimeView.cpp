#include "B2D_pch.h"
#include "SimpleFrameTimeView.h"

#include "GameEngine.h"

SimpleFrameTimeView::SimpleFrameTimeView()
{

}

SimpleFrameTimeView::~SimpleFrameTimeView()
{
    
}

void SimpleFrameTimeView::Tick(float deltaTime)
{
    uint32 const fps = GameEngine::Instance()->GetFps();

    ImGui::SetNextWindowBgAlpha(0.35f);
    if (ImGui::Begin("Simple Frame Time View", &m_open, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav))
    {
        ImGui::Text("Fps:   %d", fps);
        ImGui::Text("Delta: %.2fms", 1000.0f / fps);
    }
    ImGui::End();

    if (!m_open)
    {
        Close();
    }
}
