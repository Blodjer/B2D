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
    mWorldRenderer = GameEngine::Instance()->GetRenderManager()->CreateRenderer<WorldRenderer>();

    mViewportName = "Viewport";
    mViewportName += "##";
    mViewportName += std::to_string(reinterpret_cast<uintptr_t>(this));
}

WorldEditorView::~WorldEditorView()
{
    GameEngine::Instance()->GetRenderManager()->DeleteRenderer(mWorldRenderer);
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

        mWorldRenderer->mMutex.lock();

        ImTextureID texID = 0;
        if (GHITexture* texture = mWorldRenderer->GetRenderOutput())
        {
            texID = texture->GetNativePtr();
        }

        ImGui::Image(texID, ImVec2(contentSize.x, contentSize.y - 17), ImVec2(1, 1), ImVec2(0, 0));
        mWorldRenderer->mMutex.unlock();

        ImGui::Text(" Render: %.2fms", mWorldRenderer->GetRenderTime());
    }

    ImGui::End();
    
    ImGui::PopStyleVar();
    ImGui::PopStyleVar();

    if (!mOpen)
    {
        Close();
    }
}
