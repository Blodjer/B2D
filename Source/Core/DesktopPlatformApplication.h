#pragma once
#include "PlatformInterface.h"

class CWindow;
struct GLFWwindow;
class IPlatformMessageInterface;

class DesktopPlatformApplication : public IPlatformApplicationInterface
{
public:
    virtual void Init() override;
    virtual void PollEvents() override;
    virtual void Shutdown() override;

    virtual CWindow* MakeWindow(uint32 width, uint32 height, std::string const& title) override;
    virtual void DestroyWindow(CWindow* window) override;

    void AddMessageHandler(IPlatformMessageInterface* messageInterface);
    void RemoveMessageHandler(IPlatformMessageInterface* messageInterface);

private:
    static void OnGLFWKeyCallback(GLFWwindow* window, int glfwKey, int scancode, int glfwAction, int glfwMods);

private:
    std::vector<IPlatformMessageInterface*> mMessageHandler;
    std::vector<CWindow*> mWindows;
};
