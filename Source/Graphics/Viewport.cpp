#include "B2D_pch.h"
#include "Viewport.h"

#include "Core/Core.h"
#include "ECS/CameraEntity.h"

#include <GL/glew.h>

CViewport* CViewport::Instance = nullptr;

CViewport::CViewport(int32 x, int32 y, uint32 width, uint32 height) :
	mX(x), mY(y),
	mWidth(width), mHeight(height)
{
    Instance = this;
}

void CViewport::SetCamera(CameraEntity* camera)
{
	if (mCurrentCamera == camera)
		return;

	mCurrentCamera = camera;
}

void CViewport::SetSize(uint32 width, uint32 height)
{
	mWidth = width;
	mHeight = height;
}

bool CViewport::GetViewProjectionMatrix(TMatrix& matrix) const
{
    if (mCurrentCamera == nullptr)
    {
        B2D_CORE_ERROR("No active camera for viewport");
        return false;
    }

    matrix = mCurrentCamera->GetProjectionMatrix(this) * mCurrentCamera->GetViewMatrix();
    return true;
}

void CViewport::Use()
{
	glViewport(mX, mY, mWidth, mHeight);
}
