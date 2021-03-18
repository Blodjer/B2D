#include "B2D_pch.h"
#include "EditorModule.h"

#include "View/IEditorView.h"
#include "GameEngine.h"
#include "Platform/GenericWindow.h"
#include "View/GameSystemView.h"
#include "View/GameSystemProfilerView.h"
#include "View/SimpleFrameTimeView.h"
#include "View/WorldEditorView.h"

#include <GLFW/glfw3.h>
#include <imgui/imgui.h>
#include "imgui_impl_opengl3.h"
#include "imgui_impl_glfw.h"

void EditorModule::Init()
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
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

    // Setup Platform/Renderer bindings
    ImGui_ImplGlfw_InitForOpenGL(static_cast<GLFWwindow*>(GameEngine::Instance()->GetMainWindow()->GetGenericContext()), true);
    ImGui_ImplOpenGL3_Init("#version 410 core");

    InitDefaultEditorViews();
}

void EditorModule::InitDefaultEditorViews()
{
    CreateEditorView<SimpleFrameTimeView>();
    CreateEditorView<WorldEditorView>();
    CreateEditorView<GameSystemView>();
    CreateEditorView<GameSystemProfilerView>();
}

void EditorModule::Shutdown()
{
    for (IEditorView* editorView : m_editorViews)
    {
        CloseEditorView(editorView);
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void EditorModule::BeginFrame()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void EditorModule::Tick(float deltaTime)
{
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

void EditorModule::Draw()
{
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    if (true)
    {
        GLFWwindow* backup_current_context = glfwGetCurrentContext();
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
        glfwMakeContextCurrent(backup_current_context);
    }
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
