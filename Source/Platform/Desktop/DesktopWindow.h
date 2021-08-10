#pragma once

#include "Platform/GenericWindow.h"

struct GLFWwindow;

class DesktopWindow : public GenericWindow
{
public:
	DesktopWindow(GLFWwindow* context, uint32 width, uint32 height);
	virtual ~DesktopWindow();

    bool IsCurrentContext() const;
    
	virtual void SetSize(uint32 width, uint32 height) override;
	virtual void SetVsync(bool enable) override;

	virtual bool IsMinimized() const override { return m_isMinimized; }

    bool IsFocused() const;
    void Focus();

	virtual void SetShouldClose(bool close) override;
	virtual bool ShouldClose() const override;

    GLFWwindow* GetGLFWHandle() const { return m_context; }
	virtual void* GetNativeHandle() const override;

    virtual void MakeContextCurrent() override;
	virtual void Present() override;

private:
	void OnGlfwFramebufferSizeCallback(int width, int height);
	void OnGlfwWindowIconifyCallback(int iconified);

private:
	uint32 m_width = 0;
	uint32 m_height = 0;
	bool m_isMinimized = false;

    GLFWwindow* m_context = nullptr;

};

