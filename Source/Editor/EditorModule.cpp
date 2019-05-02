#include "B2D_pch.h"
#include "EditorModule.h"

#include "EditorViewInterface.h"
#include "GameEngine.h"
#include "Platform/GenericWindow.h"
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
    ImGui_ImplGlfw_InitForOpenGL(static_cast<GLFWwindow*>(CGameEngine::Instance()->GetMainWindow()->GetGenericContext()), true);
    ImGui_ImplOpenGL3_Init("#version 410 core");

    InitDefaultEditorViews();
}

void EditorModule::InitDefaultEditorViews()
{
    CreateEditorView<WorldEditorView>();
}

void EditorModule::Shutdown()
{
    for (IEditorView* editorView : mEditorViews)
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
    for (IEditorView* const editorView : mEditorViews)
    {
        editorView->Tick(deltaTime);
    }
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

void EditorModule::CloseEditorView(IEditorView*& editorView)
{
    auto it = std::find(mEditorViews.begin(), mEditorViews.end(), editorView);
    if (it != mEditorViews.end())
    {
        std::iter_swap(it, mEditorViews.end());
        mEditorViews.pop_back();
    }

    delete editorView;
    editorView = nullptr;
}
