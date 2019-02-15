#include "Viewport.h"

#include <GL/glew.h>

CViewport::CViewport(int x, int y, int width, int height) :
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

void CViewport::SetSize(int width, int height)
{
	mWidth = width;
	mHeight = height;

	if (mCurrentCamera != nullptr)
	{
		mCurrentCamera->OnViewportSizeChanged();
	}
}

int CViewport::GetWidth() const
{
	return mWidth;
}

int CViewport::GetHeight() const
{
	return mHeight;
}

void CViewport::Use()
{
	glViewport(mX, mY, mWidth, mHeight);
}
