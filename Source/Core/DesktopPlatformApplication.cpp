#include "B2D_pch.h"
#include "DesktopPlatformApplication.h"

#include "Graphics/Window.h"

#include <GLFW/glfw3.h>

void DesktopPlatformApplication::Init()
{
    glfwInit();
    glfwSetErrorCallback([](int error, const char* description)
    {
        B2D_CORE_ERROR("GLFW {0}: {1}", error, description);
    });
}

void DesktopPlatformApplication::PollEvents()
{
    glfwPollEvents();
}

void DesktopPlatformApplication::Shutdown()
{
    for (CWindow* window : mWindows)
    {
        DestroyWindow(window);
    }

    glfwTerminate();
}

CWindow* DesktopPlatformApplication::MakeWindow(uint32 width, uint32 height, std::string const& title)
{
    GLFWwindow* const windowContext = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);

    CWindow* window = new CWindow(windowContext, width, height);
    mWindows.emplace_back(window);

    glfwSetKeyCallback(windowContext, OnGLFWKeyCallback);

    glfwSetCursorPosCallback(windowContext, [](GLFWwindow* window, double x, double y) {
        CWindow* const userPointer = static_cast<CWindow*>(glfwGetWindowUserPointer(window));
        // TODO
    });

    return window;
}

void DesktopPlatformApplication::DestroyWindow(CWindow* window)
{
    glfwDestroyWindow(window->GetContext());
    delete window;
}

void DesktopPlatformApplication::AddMessageHandler(IPlatformMessageInterface* messageInterface)
{
    auto const it = std::find_if(mMessageHandler.begin(), mMessageHandler.end(), [messageInterface](IPlatformMessageInterface* handler) {
        return messageInterface == handler;
    });

    if (B2D_CHECKf(it != mMessageHandler.end(), "Message handler was already added to application"))
    {
        return;
    }

    mMessageHandler.emplace_back(messageInterface);
}

void DesktopPlatformApplication::RemoveMessageHandler(IPlatformMessageInterface* messageInterface)
{
    auto const it = std::find_if(mMessageHandler.begin(), mMessageHandler.end(), [messageInterface](IPlatformMessageInterface* handler) {
        return messageInterface == handler;
    });

    if (B2D_CHECKf(it == mMessageHandler.end(), "Message handler is not used in application"))
    {
        return;
    }

    mMessageHandler.erase(it);
}

void DesktopPlatformApplication::OnGLFWKeyCallback(GLFWwindow* window, int glfwKey, int scancode, int glfwAction, int glfwMods)
{
    CWindow* const userPointer = static_cast<CWindow*>(glfwGetWindowUserPointer(window));

    //TODO: Translate to B2D
    EKey key = static_cast<EKey>(glfwKey);

    EKeyEvent keyEvent = EKeyEvent::NONE;
    switch (glfwAction)
    {
        case GLFW_PRESS:
            keyEvent = EKeyEvent::KEY_DOWN; break;
        case GLFW_RELEASE:
            keyEvent = EKeyEvent::KEY_UP; break;
        default:
            B2D_CORE_WARNING("Unhandled key ({}) action ({})", glfwKey, glfwAction);
            return;
    }

    if (glfwGetWindowAttrib(window, GLFW_FOCUSED))
    {
        // window has input focus
    }

    int count;
    uint8 const* const axes = glfwGetJoystickButtons(GLFW_JOYSTICK_1, &count);
//     glfwGetGamepadName();
//     glfwGetJoystickName();

    //Input::OnKey(key, keyEvent);
}
