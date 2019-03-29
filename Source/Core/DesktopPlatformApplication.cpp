#include "B2D_pch.h"
#include "DesktopPlatformApplication.h"

#include "GameEngine.h"
#include "Graphics/DesktopWindow.h"

#include <GLFW/glfw3.h>

#define DISPATCH_PLATFORM_MESSAGE(func, ...) \
    for (IPlatformMessageHandlerInterface* const handler : DesktopPlatformApplication::sInstance->mMessageHandler) \
    { \
        if (handler->func(__VA_ARGS__)) \
        { \
            break; \
        } \
    } \

DesktopPlatformApplication* DesktopPlatformApplication::sInstance = nullptr;

bool DesktopPlatformApplication::Init()
{
    if (B2D_CHECK(sInstance != nullptr))
    {
        return false;
    }

    sInstance = this;

    B2D_CORE_INFO("Initialize GLFW...");
    
    if (glfwInit() == GLFW_FALSE)
    {
        return false;
    }

    B2D_CORE_INFO("GLFW version    {}.{}.{}\n", GLFW_VERSION_MAJOR, GLFW_VERSION_MINOR, GLFW_VERSION_REVISION);

    glfwSetErrorCallback(OnGlfwErrorCallback);
    glfwSetJoystickCallback(OnGlfwJoystickCallback);

    return true;
}

void DesktopPlatformApplication::PollEvents()
{
    glfwPollEvents();
}

void DesktopPlatformApplication::Shutdown()
{
    for (DesktopWindow* window : mWindows)
    {
        DestroyWindow(window);
    }

    glfwTerminate();
}

GenericWindow* DesktopPlatformApplication::MakeWindow(uint32 width, uint32 height, std::string const& title)
{
    GLFWwindow* const windowContext = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);

    DesktopWindow* const window = new DesktopWindow(windowContext, width, height);
    mWindows.emplace_back(window);

    glfwSetKeyCallback(windowContext, OnGlfwKeyCallback);
    glfwSetCursorPosCallback(windowContext, OnGlfwCursorPosCallback);
    glfwSetCursorEnterCallback(windowContext, OnGlfwCursorEnterCallback);
    glfwSetMouseButtonCallback(windowContext, OnGlfwMouseButtonCallback);
    glfwSetDropCallback(windowContext, OnGlfwDropCallback);

    return window;
}

void DesktopPlatformApplication::DestroyWindow(GenericWindow* window)
{
    auto const it = std::find_if(mWindows.begin(), mWindows.end(), [window](DesktopWindow* w) {
        return w == window;
    });

    if (B2D_CHECK(it == mWindows.end()))
    {
        return;
    }

    DesktopWindow* desktopWindow = static_cast<DesktopWindow*>(window);
    glfwDestroyWindow(desktopWindow->GetContext());

    mWindows.erase(it);
    //mWindows.shrink_to_fit();

    delete window;
}

void DesktopPlatformApplication::AddMessageHandler(IPlatformMessageHandlerInterface* messageHandler)
{
    auto const it = std::find_if(mMessageHandler.begin(), mMessageHandler.end(), [messageHandler](IPlatformMessageHandlerInterface* handler) {
        return messageHandler == handler;
    });

    if (B2D_CHECKf(it != mMessageHandler.end(), "Message handler was already added to application"))
    {
        return;
    }

    mMessageHandler.emplace_back(messageHandler);
}

void DesktopPlatformApplication::RemoveMessageHandler(IPlatformMessageHandlerInterface* messageHandler)
{
    auto const it = std::find_if(mMessageHandler.begin(), mMessageHandler.end(), [messageHandler](IPlatformMessageHandlerInterface* handler) {
        return messageHandler == handler;
    });

    if (B2D_CHECKf(it == mMessageHandler.end(), "Message handler is not used in application"))
    {
        return;
    }

    mMessageHandler.erase(it);
}


void DesktopPlatformApplication::OnGlfwErrorCallback(int error, const char* description)
{
    B2D_CORE_ERROR("GLFW {0}: {1}", error, description);
}

void DesktopPlatformApplication::OnGlfwKeyCallback(GLFWwindow* window, int glfwKey, int scancode, int glfwAction, int glfwMods)
{
    DesktopWindow* const userPointer = static_cast<DesktopWindow*>(glfwGetWindowUserPointer(window));
    
    //TODO: Translate to B2D in a safer way
    EKey key = static_cast<EKey>(glfwKey);

    EKeyEvent keyEvent = EKeyEvent::NONE;
    switch (glfwAction)
    {
        case GLFW_PRESS:
            keyEvent = EKeyEvent::PRESS; break;
        case GLFW_RELEASE:
            keyEvent = EKeyEvent::RELEASE; break;
        default:
            B2D_CORE_WARNING("Unhandled key callback (action: {})", glfwAction);
            return;
    }

    DISPATCH_PLATFORM_MESSAGE(OnKeyEvent, key, keyEvent);
}


void DesktopPlatformApplication::OnGlfwCursorPosCallback(GLFWwindow* window, double xpos, double ypos)
{
    DesktopWindow* const userPointer = static_cast<DesktopWindow*>(glfwGetWindowUserPointer(window));
}


void DesktopPlatformApplication::OnGlfwCursorEnterCallback(GLFWwindow* window, int entered)
{
    DesktopWindow* const userPointer = static_cast<DesktopWindow*>(glfwGetWindowUserPointer(window));
}

void DesktopPlatformApplication::OnGlfwMouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
    DesktopWindow* const userPointer = static_cast<DesktopWindow*>(glfwGetWindowUserPointer(window));
}

void DesktopPlatformApplication::OnGlfwJoystickCallback(int glfwJoystickId, int event)
{
    switch (event)
    {
        case GLFW_CONNECTED:
            break;
        case GLFW_DISCONNECTED:
            break;
        default:
            B2D_CORE_WARNING("Unhandled joystick callback (event: {})", event);
            return;
    }

    int count;
    uint8 const* const axes = glfwGetJoystickButtons(GLFW_JOYSTICK_1, &count);
    //     glfwGetGamepadName();
    //     glfwGetJoystickName();
}

void DesktopPlatformApplication::OnGlfwDropCallback(GLFWwindow* window, int count, const char** paths)
{
    DesktopWindow* const userPointer = static_cast<DesktopWindow*>(glfwGetWindowUserPointer(window));
}
