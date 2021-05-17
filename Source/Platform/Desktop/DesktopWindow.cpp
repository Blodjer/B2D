#include "B2D_pch.h"
#include "DesktopWindow.h"

#include "Platform/Desktop/DesktopPlatformApplication.h"
#include "Graphics/Viewport.h"
#include "Input/Input.h"

#include <GLFW/glfw3.h>

#ifdef B2D_PLATFORM_WINDOWS
	#define GLFW_EXPOSE_NATIVE_WIN32
#endif
#include <GLFW/glfw3native.h>

#define CURRENT_CONTEXT_CHECK() B2D_ASSERT_f(IsCurrentContext(), "Window is not current context")

DesktopWindow::DesktopWindow(GLFWwindow* context, uint32 width, uint32 height)
    : GenericWindow(width, height)
{
	m_context = context;
    m_width = width;
    m_height = height;
	
    glfwSetWindowUserPointer(context, this);

    MakeContextCurrent();
	SetVsync(false);

	glfwSetFramebufferSizeCallback(m_context, [](GLFWwindow* window, int width, int height) {
		DesktopWindow* const userPointer = static_cast<DesktopWindow*>(glfwGetWindowUserPointer(window));
		userPointer->OnGlfwFramebufferSizeCallback(width, height);
	});
}

DesktopWindow::~DesktopWindow()
{

}

bool DesktopWindow::IsCurrentContext() const
{
    return glfwGetCurrentContext() == m_context;
}

void DesktopWindow::SetSize(uint32 width, uint32 height)
{
	glfwSetWindowSize(m_context, width, height);

	m_width = width;
	m_height = height;
	GetViewport()->SetSize(width, height);
}

void DesktopWindow::SetVsync(bool enable)
{
    if (glfwGetWindowAttrib(m_context, GLFW_CLIENT_API) == GLFW_NO_API)
    {
		B2D_LOG_ERROR("VSync option not implemented for used client API!");
        return;
    }

	CURRENT_CONTEXT_CHECK();

	if (enable)
	{
		glfwSwapInterval(1);
	}
	else
	{
		glfwSwapInterval(0);
	}
}

bool DesktopWindow::IsFocused() const
{
    return glfwGetWindowAttrib(m_context, GLFW_FOCUSED);
}

void DesktopWindow::Focus()
{
    glfwFocusWindow(m_context);
}

void DesktopWindow::MakeContextCurrent()
{
    if (glfwGetWindowAttrib(m_context, GLFW_CLIENT_API) == GLFW_NO_API)
    {
        return;
    }

    if (IsCurrentContext())
    {
        return;
    }

	glfwMakeContextCurrent(m_context);
}

void DesktopWindow::Swap()
{
	// TODO: Swap in GHI?

    if (glfwGetWindowAttrib(m_context, GLFW_CLIENT_API) == GLFW_NO_API)
    {
        return;
    }

	glfwSwapBuffers(m_context);
}

void DesktopWindow::SetShouldClose(bool close)
{
	glfwSetWindowShouldClose(m_context, close);
}

bool DesktopWindow::ShouldClose() const
{
	return glfwWindowShouldClose(m_context);
}

void* DesktopWindow::GetNativeHandle() const
{
#ifdef GLFW_EXPOSE_NATIVE_WIN32
    return static_cast<void*>(glfwGetWin32Window(m_context));
#else
	B2D_COMPILE_ERROR("Native handle getter not implemented for this platform!");
#endif
}

void DesktopWindow::OnGlfwFramebufferSizeCallback(int width, int height)
{
	m_width = width;
	m_height = height;
    GetViewport()->SetSize(width, height);
}
