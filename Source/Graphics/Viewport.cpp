#include "B2D_pch.h"
#include "Viewport.h"

#include "Core/Core.h"
#include "Game/CameraEntity.h"

#include <GL/glew.h>

CViewport::CViewport(int32 x, int32 y, uint32 width, uint32 height)
    : m_x(x)
    , m_y(y)
    , m_width(width)
    , m_height(height)
{
    
}

void CViewport::SetCamera(CameraEntity* camera)
{
	if (m_currentCamera == camera)
		return;

	m_currentCamera = camera;
}

void CViewport::SetSize(uint32 width, uint32 height)
{
	m_width = width;
	m_height = height;
}

bool CViewport::GetViewProjectionMatrix(TMatrix& matrix) const
{
    if (m_currentCamera == nullptr)
    {
        B2D_LOG_ERROR("No active camera for viewport");
        return false;
    }

    matrix = m_currentCamera->GetProjectionMatrix(this) * m_currentCamera->GetViewMatrix();
    return true;
}

void CViewport::Use()
{
	glViewport(m_x, m_y, m_width, m_height);
}
