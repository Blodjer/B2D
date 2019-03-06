#pragma once

#include "Camera.h"
#include "Core/Core.h"
#include "Graphics/Viewport.h"

class CCamera;

class CViewport final
{
	friend CCamera;

public:
	CViewport(int32 x, int32 y, uint32 width, uint32 height);
	~CViewport() = default;

    static CViewport* sy;

private:
	void SetCamera(CCamera* pCamera);
public:
	CCamera* GetCamera() const;

	void SetSize(uint32 width, uint32 height);
	uint32 GetWidth() const;
	uint32 GetHeight() const;

	void Use();

private:
	CCamera* mCurrentCamera;

	int32 mX;
	int32 mY;
	uint32 mWidth;
	uint32 mHeight;

};

