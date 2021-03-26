#pragma once

#include "Core/Core.h"

class CameraEntity;

class CViewport final
{
public:
	CViewport(uint32 width, uint32 height);
	~CViewport() = default;

public:
	void SetCamera(CameraEntity* camera);
    void SetSize(uint32 width, uint32 height);

    bool GetViewProjectionMatrix(TMatrix& matrix) const;

    CameraEntity* GetCamera() const { return m_currentCamera; }
    uint32 GetWidth() const { return m_width; }
    uint32 GetHeight() const { return m_height; }

private:
    CameraEntity* m_currentCamera;

    uint32 m_width;
    uint32 m_height;

};

