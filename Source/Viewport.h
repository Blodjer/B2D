#pragma once

#include "Camera.h"

class CCamera;

class CViewport final
{
	friend CCamera;

public:
	CViewport(int x, int y, int width, int height);
	~CViewport() = default;

private:
	void SetCamera(CCamera* pCamera);
public:
	CCamera* GetCamera() const;

	void SetSize(int width, int height);
	int GetWidth() const;
	int GetHeight() const;

	void Use();

private:
	CCamera* mCurrentCamera;

	int mX;
	int mY;
	int mWidth;
	int mHeight;

};

