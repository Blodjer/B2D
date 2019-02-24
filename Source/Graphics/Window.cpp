#include "B2D_pch.h"
#include "Window.h"

#include "Graphics/Viewport.h"

#include <GLFW/glfw3.h>

CWindow* CWindow::gCurrentContext = nullptr;

CWindow::CWindow(uint32 width, uint32 height, std::string const& title)
{
	mGlfwWindow = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
	glfwSetWindowUserPointer(mGlfwWindow, this);
	MakeContextCurrent();

	SetVsync(true);

	mViewport = new CViewport(0, 0, width, height);

	glfwSetFramebufferSizeCallback(mGlfwWindow, [](GLFWwindow* window, int width, int height) {
		CWindow* const userPointer = static_cast<CWindow*>(glfwGetWindowUserPointer(window));
		userPointer->OnFramebufferSizeChanged(width, height);
	});
}

CWindow::~CWindow()
{
	glfwDestroyWindow(mGlfwWindow);
	delete mViewport;
}

void CWindow::SetSize(uint32 width, uint32 height)
{
	glfwSetWindowSize(mGlfwWindow, width, height);

	mWidth = width;
	mHeight = height;
	mViewport->SetSize(width, height);
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

void CWindow::MakeContextCurrent()
{
	gCurrentContext = this;
	glfwMakeContextCurrent(mGlfwWindow);
}

void CWindow::Swap()
{
	glfwSwapBuffers(mGlfwWindow);
}

void CWindow::SetShouldClose(bool close)
{
	glfwSetWindowShouldClose(mGlfwWindow, close);
}

bool CWindow::ShouldClose() const
{
	return glfwWindowShouldClose(mGlfwWindow);
}

void CWindow::OnFramebufferSizeChanged(int width, int height)
{
	mWidth = width;
	mHeight = height;
	mViewport->SetSize(width, height);
}
