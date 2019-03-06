#pragma once

#include "Core/Types/BasicTypes.h"

class CGameEngine;
struct GLFWwindow;
class Graphics;
class CViewport;

#if _DEBUG
#define CURRENT_CONTEXT_CHECK() if (!IsCurrentContext()) { __debugbreak(); }
#else
#define CURRENT_CONTEXT_CHECK()
#endif

class CWindow final
{
	friend CGameEngine;
	friend Graphics;

public:
	CWindow(uint32 width, uint32 height, std::string const& title);
	~CWindow();

	bool IsCurrentContext() const { return gCurrentContext == this; }
	
	GLFWwindow* GetWindow() const { return mGlfwWindow; }
	CViewport* GetViewport() const { return mViewport; }

	void SetSize(uint32 width, uint32 height);
	void SetVsync(bool enable);

	void SetShouldClose(bool close);
	bool ShouldClose() const;

public:
	void MakeContextCurrent();

	void Swap();

	void OnFramebufferSizeChanged(int width, int height);

private:
	static CWindow* gCurrentContext;

	uint32 mWidth;
	uint32 mHeight;

private:
	GLFWwindow* mGlfwWindow;
	CViewport* mViewport;

};

