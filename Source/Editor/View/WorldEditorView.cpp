#include "B2D_pch.h"
#include "WorldEditorView.h"

#include "GameEngine.h"
#include "Graphics/GHI/GHIRenderTarget.h"
#include "Graphics/OpenGL/OpenGLTexture.h"
#include "Graphics/Renderer/WorldRenderer.h"
#include "Graphics/RenderManger.h"

WorldEditorView::WorldEditorView()
    : mOpen(true)
{
    worldRenderer = CGameEngine::Instance()->GetRenderManager()->CreateRenderer<WorldRenderer>();
}

WorldEditorView::~WorldEditorView()
{
    CGameEngine::Instance()->GetRenderManager()->DeleteRenderer(worldRenderer);
}

void WorldEditorView::Tick(float deltaTime)
{
    ImGui::SetNextWindowSizeConstraints(ImVec2(200, 120), ImVec2(9999, 9999));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));

    if (ImGui::Begin("Viewport", &mOpen, ImGuiWindowFlags_NoCollapse))
    {
        ImVec2 contentSize = ImGui::GetContentRegionAvail();

        worldRenderer->mMutex.lock();
        OpenGLTexture const* tex = static_cast<OpenGLTexture const*>(worldRenderer->GetRenderOutput());
        ImGui::Image((void*)(tex->GetHandle()), ImVec2(contentSize.x, contentSize.y - 18), ImVec2(1, 1), ImVec2(0, 0));
        worldRenderer->mMutex.unlock();

        ImGui::Text("Fps: %d | Delta: %.2fm", 2, 1000.0f / 2);
    }
    else
    {
        // TODO: Destroy this
    }

    ImGui::End();

    ImGui::PopStyleVar();
}
