#include "B2D_pch.h"
#include "DesktopPlatformApplication.h"

#include "GameEngine.h"
#include "Graphics/OpenGL/OpenGLGHI.h"
#include "Platform/Desktop/DesktopWindow.h"

#include <GLFW/glfw3.h>

#define DISPATCH_PLATFORM_MESSAGE(func, ...) \
    for (IPlatformMessageHandlerInterface* const handler : DesktopPlatformApplication::ms_instance->m_messageHandler) \
    { \
        if (handler->func(__VA_ARGS__)) \
        { \
            break; \
        } \
    } \

DesktopPlatformApplication* DesktopPlatformApplication::ms_instance = nullptr;

bool DesktopPlatformApplication::Init()
{
    if (B2D_CHECK(ms_instance != nullptr))
    {
        return false;
    }

    ms_instance = this;

    B2D_INFO("Initialize GLFW...");
    
    B2D_ASSERT(glfwInit() == GLFW_TRUE)

    B2D_INFO("GLFW version    {}.{}.{}\n", GLFW_VERSION_MAJOR, GLFW_VERSION_MINOR, GLFW_VERSION_REVISION);

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
    for (DesktopWindow* window : m_windows)
    {
        DestroyWindow(window);
    }

    glfwTerminate();
}

IGraphicsHardwareInterface* DesktopPlatformApplication::CreateGHI() const
{
    return new OpenGLGHI();
}

GenericWindow* DesktopPlatformApplication::MakeWindow(uint32 width, uint32 height, std::string const& title)
{
    GLFWwindow* const windowContext = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);

    DesktopWindow* const window = new DesktopWindow(windowContext, width, height);
    m_windows.emplace_back(window);

    glfwSetKeyCallback(windowContext, OnGlfwKeyCallback);
    glfwSetCharCallback(windowContext, OnGlfwCharCallback);
    glfwSetCursorPosCallback(windowContext, OnGlfwCursorPosCallback);
    glfwSetCursorEnterCallback(windowContext, OnGlfwCursorEnterCallback);
    glfwSetMouseButtonCallback(windowContext, OnGlfwMouseButtonCallback);
    glfwSetDropCallback(windowContext, OnGlfwDropCallback);
    
    return window;
}

void DesktopPlatformApplication::DestroyWindow(GenericWindow* window)
{
    auto const it = std::find_if(m_windows.begin(), m_windows.end(), [window](DesktopWindow* w) {
        return w == window;
    });

    if (B2D_CHECK(it == m_windows.end()))
    {
        return;
    }

    DesktopWindow* desktopWindow = static_cast<DesktopWindow*>(window);
    glfwDestroyWindow(desktopWindow->GetContext());

    m_windows.erase(it);
    //mWindows.shrink_to_fit();

    delete window;
}

GenericWindow* DesktopPlatformApplication::CreateOffscreenRenderContext()
{
    GLFWwindow* main = static_cast<GLFWwindow*>(GameEngine::Instance()->GetMainWindow()->GetGenericContext());

    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
    GLFWwindow* offscreenContext = glfwCreateWindow(100, 100, "", nullptr, main);
    DesktopWindow* const window = new DesktopWindow(offscreenContext, 100, 100);
    m_windows.emplace_back(window);

    GameEngine::Instance()->GetMainWindow()->MakeContextCurrent();

    return window;
}

void DesktopPlatformApplication::AddMessageHandler(IPlatformMessageHandlerInterface* messageHandler)
{
    auto const it = std::find_if(m_messageHandler.begin(), m_messageHandler.end(), [messageHandler](IPlatformMessageHandlerInterface* handler) {
        return messageHandler == handler;
    });

    if (B2D_CHECKf(it != m_messageHandler.end(), "Message handler was already added to application"))
    {
        return;
    }

    m_messageHandler.emplace_back(messageHandler);
}

void DesktopPlatformApplication::RemoveMessageHandler(IPlatformMessageHandlerInterface* messageHandler)
{
    auto const it = std::find_if(m_messageHandler.begin(), m_messageHandler.end(), [messageHandler](IPlatformMessageHandlerInterface* handler) {
        return messageHandler == handler;
    });

    if (B2D_CHECKf(it == m_messageHandler.end(), "Message handler is not used in application"))
    {
        return;
    }

    m_messageHandler.erase(it);
}

void DesktopPlatformApplication::OnGlfwErrorCallback(int error, const char* description)
{
    B2D_ERROR("GLFW {0}: {1}", error, description);
}

void DesktopPlatformApplication::OnGlfwKeyCallback(GLFWwindow* window, int glfwKey, int scancode, int glfwAction, int glfwMods)
{
    DesktopWindow* const userPointer = static_cast<DesktopWindow*>(glfwGetWindowUserPointer(window));
    
    if (glfwKey < 0)
    {
        B2D_WARNING("Unhandled key type (scancode: {})", scancode);
        return;
    }

    //TODO: Translate to B2D in a safer way
    EKey key = static_cast<EKey>(glfwKey);

    EKeyEvent keyEvent = EKeyEvent::None;
    switch (glfwAction)
    {
        case GLFW_PRESS:
            keyEvent = EKeyEvent::Press; break;
        case GLFW_RELEASE:
            keyEvent = EKeyEvent::Release; break;
        case GLFW_REPEAT:
            keyEvent = EKeyEvent::Repeat; break;
        default:
            B2D_WARNING("Unhandled key callback (action: {})", glfwAction);
            return;
    }

    DISPATCH_PLATFORM_MESSAGE(OnKeyEvent, userPointer, scancode, key, keyEvent);
}


void DesktopPlatformApplication::OnGlfwCharCallback(GLFWwindow* window, unsigned int codepoint)
{
    DesktopWindow* const userPointer = static_cast<DesktopWindow*>(glfwGetWindowUserPointer(window));

    DISPATCH_PLATFORM_MESSAGE(OnKeyChar, userPointer, static_cast<uint32>(codepoint));
}

void DesktopPlatformApplication::OnGlfwCursorPosCallback(GLFWwindow* window, double xpos, double ypos)
{
    DesktopWindow* const userPointer = static_cast<DesktopWindow*>(glfwGetWindowUserPointer(window));

    TVec2 pos(xpos, ypos);
    DISPATCH_PLATFORM_MESSAGE(OnMouseMove, userPointer, pos);
}

void DesktopPlatformApplication::OnGlfwCursorEnterCallback(GLFWwindow* window, int entered)
{
    DesktopWindow* const userPointer = static_cast<DesktopWindow*>(glfwGetWindowUserPointer(window));
    
    if (entered == GLFW_TRUE)
    {
        DISPATCH_PLATFORM_MESSAGE(OnMouseEnter, userPointer);
    }
    else
    {
        DISPATCH_PLATFORM_MESSAGE(OnMouseLeave, userPointer);
    }
}

void DesktopPlatformApplication::OnGlfwMouseButtonCallback(GLFWwindow* window, int button, int glfwAction, int glfwMods)
{
    DesktopWindow* const userPointer = static_cast<DesktopWindow*>(glfwGetWindowUserPointer(window));
    
    EMouseButton mouseButton = static_cast<EMouseButton>(button);

    EMouseButtonEvent mouseButtonEvent;
    switch (glfwAction)
    {
        case GLFW_PRESS:
            mouseButtonEvent = EMouseButtonEvent::Press; break;
        case GLFW_RELEASE:
            mouseButtonEvent = EMouseButtonEvent::Release; break;
        default:
            B2D_WARNING("Unhandled mouse button callback (action: {})", glfwAction);
            return;
    }

    DISPATCH_PLATFORM_MESSAGE(OnMouseButton, userPointer, mouseButton, mouseButtonEvent);
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
            B2D_WARNING("Unhandled joystick callback (event: {})", event);
            return;
    }

    int count;
    uint8 const* const axes = glfwGetJoystickButtons(GLFW_JOYSTICK_1, &count);
    //     glfwGetGamepadName();
    //     glfwGetJoystickName();

    B2D_WARNING("OnGlfwJoystickCallback is not implemented");
}

void DesktopPlatformApplication::OnGlfwDropCallback(GLFWwindow* window, int count, const char** paths)
{
    DesktopWindow* const userPointer = static_cast<DesktopWindow*>(glfwGetWindowUserPointer(window));

    B2D_WARNING("OnGlfwDropCallback is not implemented");
}
