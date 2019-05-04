#include "B2D_pch.h"
#include "WorldEditorView.h"

#include "GameEngine.h"
#include "Graphics/GHI/GHIRenderTarget.h"
#include "Graphics/OpenGL/OpenGLTexture.h"
#include "Graphics/Renderer/WorldRenderer.h"
#include "Graphics/RenderManger.h"
#include "../EditorModule.h"

WorldEditorView::WorldEditorView()
{
    worldRenderer = GameEngine::Instance()->GetRenderManager()->CreateRenderer<WorldRenderer>();

    mViewportName = "Viewport";
    mViewportName += "##";
    mViewportName += std::to_string(reinterpret_cast<uintptr_t>(this));
}

WorldEditorView::~WorldEditorView()
{
    GameEngine::Instance()->GetRenderManager()->DeleteRenderer(worldRenderer);
}

void WorldEditorView::Tick(float deltaTime)
{
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));

    ImGui::SetNextWindowSizeConstraints(ImVec2(200, 120), ImVec2(9999, 9999));
    ImGui::SetNextWindowSize(ImVec2(800, 500), ImGuiCond_Once);

    if (ImGui::Begin(mViewportName.c_str(), &mOpen, ImGuiWindowFlags_NoCollapse))
    {
        ImVec2 contentSize = ImGui::GetContentRegionAvail();

        worldRenderer->mMutex.lock();
        OpenGLTexture const* tex = static_cast<OpenGLTexture const*>(worldRenderer->GetRenderOutput());
        ImGui::Image((void*)(tex->GetHandle()), ImVec2(contentSize.x, contentSize.y - 17), ImVec2(1, 1), ImVec2(0, 0));
        worldRenderer->mMutex.unlock();

        ImGui::Text(" Render: %.2fms", worldRenderer->GetRenderTime());
    }

    ImGui::End();

    ImGui::PopStyleVar();
    ImGui::PopStyleVar();

    if (!mOpen)
    {
        Close();
    }
}
