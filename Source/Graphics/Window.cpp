#include "B2D_pch.h"
#include "Window.h"

#include "Graphics/Viewport.h"
#include "Input/Input.h"

#include <GLFW/glfw3.h>

#define CURRENT_CONTEXT_CHECK() B2D_ASSERTf(IsCurrentContext(), "Window is not current context")

CWindow::CWindow(GLFWwindow* context, uint32 width, uint32 height)
{
	mContext = context;
    mWidth = width;
    mHeight = height;
	
    glfwSetWindowUserPointer(context, this);

    MakeContextCurrent();
	SetVsync(false);

	mViewports.emplace_back(new CViewport(0, 0, width, height));

	glfwSetFramebufferSizeCallback(GetContext(), [](GLFWwindow* window, int width, int height) {
		CWindow* const userPointer = static_cast<CWindow*>(glfwGetWindowUserPointer(window));
		userPointer->OnFramebufferSizeCallback(width, height);
	});
}

CWindow::~CWindow()
{
    for (CViewport* viewport : mViewports)
    {
        delete viewport;
    }
}

bool CWindow::IsCurrentContext() const
{
    return glfwGetCurrentContext() == mContext;
}

void CWindow::AssignGameInstance(CGameInstance* gameInstance)
{
    B2D_CHECKf(mAssignedGameInstance != nullptr, "Window already has a assigned game instance");
    mAssignedGameInstance = gameInstance;
}

void CWindow::SetSize(uint32 width, uint32 height)
{
	glfwSetWindowSize(mContext, width, height);

	mWidth = width;
	mHeight = height;
	GetViewport()->SetSize(width, height);
}

void CWindow::SetVsync(bool enable)
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

bool CWindow::IsFocused() const
{
    return glfwGetWindowAttrib(mContext, GLFW_FOCUSED);
}

void CWindow::Focus()
{
    glfwFocusWindow(mContext);
}

void CWindow::MakeContextCurrent()
{
    if (IsCurrentContext())
    {
        return;
    }

	glfwMakeContextCurrent(mContext);
}

void CWindow::Swap()
{
	glfwSwapBuffers(mContext);
}

void CWindow::SetShouldClose(bool close)
{
	glfwSetWindowShouldClose(mContext, close);
}

bool CWindow::ShouldClose() const
{
	return glfwWindowShouldClose(mContext);
}

void CWindow::OnFramebufferSizeCallback(int width, int height)
{
	mWidth = width;
	mHeight = height;
    GetViewport()->SetSize(width, height);
}
