#pragma once

#include "Core/Core.h"

class CGameInstance;
struct GLFWwindow;
class CViewport;

class CWindow final
{
    friend class CGameEngine;
    friend class CRenderer;

public:
	CWindow(GLFWwindow* context, uint32 width, uint32 height);
	~CWindow();

    bool IsCurrentContext() const;
    
    void AssignGameInstance(CGameInstance* gameInstance);
    CGameInstance* GetAssignedGameInstance() const { return mAssignedGameInstance; }

    CViewport* const GetViewport() const { return mViewports[0]; }
    std::vector<CViewport*> const& GetViewports() const { return mViewports; }

	void SetSize(uint32 width, uint32 height);
	void SetVsync(bool enable);

    bool IsFocused() const;
    void Focus();

	void SetShouldClose(bool close);
	bool ShouldClose() const;

    GLFWwindow* GetContext() const { return mContext; }

private:
    void MakeContextCurrent();
	void Swap();

	void OnFramebufferSizeCallback(int width, int height);

private:
	uint32 mWidth;
	uint32 mHeight;

    GLFWwindow* mContext = nullptr;
    CGameInstance* mAssignedGameInstance = nullptr;

    std::vector<CViewport*> mViewports;

};

