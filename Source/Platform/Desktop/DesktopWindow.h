#pragma once

#include "Platform/GenericWindow.h"

class CGameInstance;
class CViewport;
class DesktopPlatformApplication;
struct GLFWwindow;
class IPlatformApplicationInterface;

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

    GLFWwindow* GetContext() const { return mContext; }

public:
    virtual void MakeContextCurrent() override;
	virtual void Swap() override;

	void OnGlfwFramebufferSizeCallback(int width, int height);

private:
	uint32 mWidth;
	uint32 mHeight;

    GLFWwindow* mContext = nullptr;

};

