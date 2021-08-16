#include "B2D_pch.h"
#include "WorldEditorView.h"

#include "GameEngine.h"
#include "Graphics/OpenGL/OpenGLTexture.h"
#include "Graphics/Renderer/WorldRenderer.h"
#include "Graphics/RenderManager.h"
#include "../EditorModule.h"
#include "Graphics/Viewport.h"
#include "Game/Component/CameraComponent.h"
#include "Game/CameraEntity.h"
#include "Platform/GenericWindow.h"
#include "Game/GameInstance.h"

WorldEditorView::WorldEditorView()
{
    static uint32 count = 0;
    count++;

    m_viewportName = "Viewport";
    m_viewportName += "##";
    m_viewportName += std::to_string(count);

    m_viewport = new CViewport(100, 100);

    m_worldRenderer = GameEngine::Instance()->GetRenderManager()->CreateRenderer<WorldRenderer>();
    m_worldRenderer->m_viewport = m_viewport;
}

WorldEditorView::~WorldEditorView()
{
    GameEngine::Instance()->GetRenderManager()->DeleteRenderer(m_worldRenderer);
    // TODO: Remove temporary camera

    delete m_viewport;
}

void WorldEditorView::Tick(float deltaTime)
{
    if (!m_viewport->GetCamera() && GameEngine::Instance()->GetGameInstance() && GameEngine::Instance()->GetGameInstance()->GetWorld())
    {
        CameraEntity* camera = GameEngine::Instance()->GetGameInstance()->GetWorld()->AddSystemEntityObject<CameraEntity>();
        m_viewport->SetCamera(camera);
    }

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));

    ImGui::SetNextWindowSizeConstraints(ImVec2(200, 120), ImVec2(9999, 9999));
    ImGui::SetNextWindowSize(ImVec2(800, 500), ImGuiCond_Once);

    if (ImGui::Begin(m_viewportName.c_str(), &m_open, ImGuiWindowFlags_NoCollapse))
    {
        if (m_viewport->GetCamera())
        {
            CameraComponent& cameraComponent = m_viewport->GetCamera()->GetComponent<CameraComponent>();

            bool isPerspective = cameraComponent.targetProjection == EProjection::Perspective;
            if (ImGui::Checkbox("Perspective", &isPerspective))
            {
                m_viewport->GetCamera()->SetProjection(isPerspective ? EProjection::Perspective : EProjection::Orthographic);
            }

            //cameraComponent.projection = isPerspective ? EProjection::Perspective : EProjection::Orthographic;
            if (isPerspective)
            {
                ImGui::SliderFloat("Field Of View", &cameraComponent.fov, 1.0f, 180.0f, "%.0f deg");
            }
            else
            {
                ImGui::SliderFloat("Orthographic Width", &cameraComponent.orthoWidth, 0.0f, 1000.0f);
            }


            ImGui::InputFloat("Near Plane", &cameraComponent.nearPlane);
            ImGui::InputFloat("Far Plane", &cameraComponent.farPlane);
        }

        ImVec2 const contentSize = ImGui::GetContentRegionAvail();
        ImVec2 viewportSize(contentSize.x, contentSize.y - 17.0f);
        viewportSize.x = UMath::Max(viewportSize.x, 100.0f);
        viewportSize.y = UMath::Max(viewportSize.y, 100.0f);

        uint32 const viewportWidth = UMath::FloorToUInt(viewportSize.x);
        uint32 const viewportHeight = UMath::FloorToUInt(viewportSize.y);
        m_viewport->SetSize(viewportWidth, viewportHeight);

        ImTextureID texID = 0;
        if (GHITexture const* texture = m_worldRenderer->GetRenderOutput())
        {
            texID = texture->GetNativePtr();
        }

        ImGui::Image(texID, ImVec2(contentSize.x, contentSize.y - 17), ImVec2(1, 1), ImVec2(0, 0));

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
