#include "Viewport.h"

#include <GL/glew.h>

CViewport::CViewport(int32 x, int32 y, uint32 width, uint32 height) :
	mX(x), mY(y),
	mWidth(width), mHeight(height)
{

}

void CViewport::SetCamera(CCamera* pCamera)
{
	if (mCurrentCamera == pCamera)
		return;

	mCurrentCamera = pCamera;
	mCurrentCamera->SetViewport(this);
}

CCamera* CViewport::GetCamera() const
{
	return mCurrentCamera;
}

void CViewport::SetSize(uint32 width, uint32 height)
{
	mWidth = width;
	mHeight = height;

	if (mCurrentCamera != nullptr)
	{
		mCurrentCamera->OnViewportSizeChanged();
	}
}

uint32 CViewport::GetWidth() const
{
	return mWidth;
}

uint32 CViewport::GetHeight() const
{
	return mHeight;
}

void CViewport::Use()
{
	glViewport(mX, mY, mWidth, mHeight);
}
