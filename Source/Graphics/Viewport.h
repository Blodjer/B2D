#pragma once

#include "Core/Core.h"

class CameraEntity;

class CViewport final
{
public:
	CViewport(int32 x, int32 y, uint32 width, uint32 height);
	~CViewport() = default;

    static CViewport* Instance; // TODO

public:
	void SetCamera(CameraEntity* camera);
	void SetSize(uint32 width, uint32 height);

public:
    bool GetViewProjectionMatrix(TMatrix& matrix) const;

    CameraEntity* GetCamera() const { return mCurrentCamera; }
    uint32 GetWidth() const { return mWidth; }
    uint32 GetHeight() const { return mHeight; }

	void Use();

private:
    CameraEntity* mCurrentCamera;

    int32 mX;
    int32 mY;
    uint32 mWidth;
    uint32 mHeight;

};

