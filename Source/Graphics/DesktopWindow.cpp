#include "B2D_pch.h"
#include "DesktopWindow.h"

#include "Core/DesktopPlatformApplication.h"
#include "Graphics/Viewport.h"
#include "Input/Input.h"

#include <GLFW/glfw3.h>

#define CURRENT_CONTEXT_CHECK() B2D_ASSERTf(IsCurrentContext(), "Window is not current context")

DesktopWindow::DesktopWindow(GLFWwindow* context, uint32 width, uint32 height)
    : GenericWindow(width, height)
{
	mContext = context;
    mWidth = width;
    mHeight = height;
	
    glfwSetWindowUserPointer(context, this);

    MakeContextCurrent();
	SetVsync(false);


	glfwSetFramebufferSizeCallback(GetContext(), [](GLFWwindow* window, int width, int height) {
		DesktopWindow* const userPointer = static_cast<DesktopWindow*>(glfwGetWindowUserPointer(window));
		userPointer->OnGlfwFramebufferSizeCallback(width, height);
	});
}

DesktopWindow::~DesktopWindow()
{

}

bool DesktopWindow::IsCurrentContext() const
{
    return glfwGetCurrentContext() == mContext;
}


void DesktopWindow::SetSize(uint32 width, uint32 height)
{
	glfwSetWindowSize(mContext, width, height);

	mWidth = width;
	mHeight = height;
	GetViewport()->SetSize(width, height);
}

void DesktopWindow::SetVsync(bool enable)
{
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
    return glfwGetWindowAttrib(mContext, GLFW_FOCUSED);
}

void DesktopWindow::Focus()
{
    glfwFocusWindow(mContext);
}

void DesktopWindow::MakeContextCurrent()
{
    if (IsCurrentContext())
    {
        return;
    }

	glfwMakeContextCurrent(mContext);
}

void DesktopWindow::Swap()
{
	glfwSwapBuffers(mContext);
}

void DesktopWindow::SetShouldClose(bool close)
{
	glfwSetWindowShouldClose(mContext, close);
}

bool DesktopWindow::ShouldClose() const
{
	return glfwWindowShouldClose(mContext);
}

void DesktopWindow::OnGlfwFramebufferSizeCallback(int width, int height)
{
	mWidth = width;
	mHeight = height;
    GetViewport()->SetSize(width, height);
}
