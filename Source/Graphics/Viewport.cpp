#include "B2D_pch.h"
#include "Viewport.h"

#include "Core/Core.h"
#include "Game/CameraEntity.h"

CViewport::CViewport(uint32 width, uint32 height)
    : m_width(width)
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
