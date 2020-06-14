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
    static uint32 count = 0;
    count++;

    m_worldRenderer = GameEngine::Instance()->GetRenderManager()->CreateRenderer<WorldRenderer>();

    m_viewportName = "Viewport";
    m_viewportName += "##";
    m_viewportName += std::to_string(count);
}

WorldEditorView::~WorldEditorView()
{
    GameEngine::Instance()->GetRenderManager()->DeleteRenderer(m_worldRenderer);
}

void WorldEditorView::Tick(float deltaTime)
{
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));

    ImGui::SetNextWindowSizeConstraints(ImVec2(200, 120), ImVec2(9999, 9999));
    ImGui::SetNextWindowSize(ImVec2(800, 500), ImGuiCond_Once);

    if (ImGui::Begin(m_viewportName.c_str(), &m_open, ImGuiWindowFlags_NoCollapse))
    {
        ImVec2 contentSize = ImGui::GetContentRegionAvail();

        m_worldRenderer->m_mutex.lock();

        ImTextureID texID = 0;
        if (GHITexture* texture = m_worldRenderer->GetRenderOutput())
        {
            texID = texture->GetNativePtr();
        }

        ImGui::Image(texID, ImVec2(contentSize.x, contentSize.y - 17), ImVec2(1, 1), ImVec2(0, 0));
        m_worldRenderer->m_mutex.unlock();

        ImGui::Text(" Render: %.2fms", m_worldRenderer->GetRenderTime());
    }

    ImGui::End();
    
    ImGui::PopStyleVar();
    ImGui::PopStyleVar();

    if (!m_open)
    {
        Close();
    }
}
