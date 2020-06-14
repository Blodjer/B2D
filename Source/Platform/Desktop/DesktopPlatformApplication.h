#pragma once
#include "Platform/PlatformInterface.h"

class DesktopWindow;
struct GLFWwindow;
class IPlatformMessageHandlerInterface;
class GenericWindow;

class DesktopPlatformApplication : public IPlatformApplicationInterface
{
public:
    virtual bool Init() override;
    virtual void PollEvents() override;
    virtual void Shutdown() override;

    virtual IGraphicsHardwareInterface* CreateGHI() const override;

    virtual GenericWindow* MakeWindow(uint32 width, uint32 height, std::string const& title) override;
    virtual void DestroyWindow(GenericWindow* window) override;

    virtual GenericWindow* CreateOffscreenRenderContext() override;

    virtual void AddMessageHandler(IPlatformMessageHandlerInterface* messageHandler) override;
    virtual void RemoveMessageHandler(IPlatformMessageHandlerInterface* messageHandler) override;

private:
    // GLFW Callbacks

    static void OnGlfwErrorCallback(int error, const char* description);

    static void OnGlfwKeyCallback(GLFWwindow* window, int glfwKey, int scancode, int glfwAction, int glfwMods);
    static void OnGlfwCharCallback(GLFWwindow* window, unsigned int codepoint);

    static void OnGlfwCursorPosCallback(GLFWwindow* window, double xpos, double ypos);
    static void OnGlfwCursorEnterCallback(GLFWwindow* window, int entered);
    static void OnGlfwMouseButtonCallback(GLFWwindow* window, int button, int glfwAction, int glfwMods);

    static void OnGlfwJoystickCallback(int glfwJoystickId, int event);

    static void OnGlfwDropCallback(GLFWwindow* window, int count, const char** paths);

private:
    std::vector<IPlatformMessageHandlerInterface*> m_messageHandler;
    std::vector<DesktopWindow*> m_windows;

    static DesktopPlatformApplication* ms_instance;
};
