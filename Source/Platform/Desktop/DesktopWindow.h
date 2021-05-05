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

    bool IsFocused() const;
    void Focus();

	virtual void SetShouldClose(bool close) override;
	virtual bool ShouldClose() const override;

    GLFWwindow* GetGLFWHandle() const { return m_context; }
	virtual void* GetNativeHandle() const override;

public:
    virtual void MakeContextCurrent() override;
	virtual void Swap() override;

	void OnGlfwFramebufferSizeCallback(int width, int height);

private:
	uint32 m_width;
	uint32 m_height;

    GLFWwindow* m_context = nullptr;

};

