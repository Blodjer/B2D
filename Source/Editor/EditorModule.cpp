#include "B2D_pch.h"
#include "EditorModule.h"

#include "GameEngine.h"
#include "Graphics/GHI/GraphicsHardwareInterface.h"
#include "Input/Input.h"
#include "Platform/GenericWindow.h"
#include "Platform/PlatformInterface.h"
#include "View/GameSystemView.h"
#include "View/GameSystemProfilerView.h"
#include "View/IEditorView.h"
#include "View/SimpleFrameTimeView.h"
#include "View/WorldEditorView.h"

#include "ImGuiCommon.h"

bool EditorModule::Init()
{
    IMGUI_CHECKVERSION();

    B2D_LOG_INFO("Initialize ImGui...");
    B2D_LOG_INFO("ImGui version\t{}", IMGUI_VERSION);

    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
    //io.ConfigViewportsNoAutoMerge = true;
    //io.ConfigViewportsNoTaskBarIcon = true;

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsClassic();

    // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
    ImGuiStyle& style = ImGui::GetStyle();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }

    // Setup ImGui on our platform
    if (!GameEngine::Instance()->GetPAI()->ImGui_Init())
    {
        B2D_LOG_ERROR("PAI failed to init ImGui!");
        ImGui::DestroyContext();
        return false;
    }

    // Setup ImGui on our render platform
    if (!GameEngine::Instance()->GetGHI()->ImGui_Init())
    {
        B2D_LOG_ERROR("GHI failed to init ImGui!");
        ImGui::DestroyContext();
        return false;
    }

    InitDefaultEditorViews();

    GameEngine::Instance()->GetPAI()->AddMessageHandler(this);

    return true;
}

void EditorModule::InitDefaultEditorViews()
{
    CreateEditorView<SimpleFrameTimeView>();
    //CreateEditorView<WorldEditorView>();
    CreateEditorView<GameSystemView>();
    CreateEditorView<GameSystemProfilerView>();
}

void EditorModule::Shutdown()
{
    GameEngine::Instance()->GetPAI()->RemoveMessageHandler(this);

    for (IEditorView* editorView : m_editorViews)
    {
        CloseEditorView(editorView);
    }

    GameEngine::Instance()->GetGHI()->ImGui_Shutdow();
    GameEngine::Instance()->GetPAI()->ImGui_Shutdow();
    ImGui::DestroyContext();
}

void EditorModule::BeginFrame()
{
    GameEngine::Instance()->GetGHI()->ImGui_BeginFrame();
    GameEngine::Instance()->GetPAI()->ImGui_BeginFrame();
    ImGui::NewFrame();
}

void EditorModule::Tick(float deltaTime)
{
    if (Input::IsKey(EKey::V, EKeyEvent::Press))
    {
        CreateEditorView<WorldEditorView>();
    }

    for (IEditorView* const editorView : m_editorViews)
    {
        editorView->Tick(deltaTime);
    }

    ClosePendingEditorViews();
}

void EditorModule::EndFrame()
{
    ImGui::EndFrame();
}

void EditorModule::Draw(GHICommandBuffer& commandBuffer)
{
    ImGui::Render();
    GameEngine::Instance()->GetGHI()->ImGui_Render(&commandBuffer);

    ImGui::UpdatePlatformWindows();
    ImGui::RenderPlatformWindowsDefault();

    GameEngine::Instance()->GetMainWindow()->MakeContextCurrent();
}

void EditorModule::CloseEditorView(IEditorView* editorView)
{
    m_editorToRemove.emplace(editorView);
    editorView = nullptr;
}

void EditorModule::ClosePendingEditorViews()
{
    while (!m_editorToRemove.empty())
    {
        IEditorView* editor = m_editorToRemove.front();
        m_editorToRemove.pop();

        auto it = std::find(m_editorViews.begin(), m_editorViews.end(), editor);
        if (it != m_editorViews.end())
        {
            std::iter_swap(it, m_editorViews.end() - 1);
            m_editorViews.pop_back();
        }

        delete editor;
    }
}
