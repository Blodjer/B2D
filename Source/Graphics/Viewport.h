#pragma once

#include "Core/Core.h"

class CameraEntity;

class CViewport final
{
public:
	CViewport(int32 x, int32 y, uint32 width, uint32 height);
	~CViewport() = default;

public:
	void SetCamera(CameraEntity* camera);
	void SetSize(uint32 width, uint32 height);

public:
    bool GetViewProjectionMatrix(TMatrix& matrix) const;

    CameraEntity* GetCamera() const { return m_currentCamera; }
    uint32 GetWidth() const { return m_width; }
    uint32 GetHeight() const { return m_height; }

	void Use();

private:
    CameraEntity* m_currentCamera;

    int32 m_x;
    int32 m_y;
    uint32 m_width;
    uint32 m_height;

};

