#include "B2D_pch.h"
#include "Camera.h"

#include "Core/GameEngine.h"
#include "Graphics/Viewport.h"
#include "Graphics/Window.h"

CCamera::CCamera()
	: CGameObject()
{
	mProjection = EProjection::Perspective;
	mTargetProjection = mProjection;
	mProjectionLerp = 0.f;

	TVec2 const vPosition = GetPosition();
	mViewMatrix = TMatrix::LookAt(
		TVec3(vPosition.X, vPosition.Y, -300.0f),
		TVec3(0.0f, 0.0f, 0.0f),
		TVec3(0.0f, 1.0f, 0.0f));

	mProjectionMatrix = CreateProjectionMatrix(mProjection);
}

void CCamera::Update(float deltaTime)
{
	CGameObject::Update(deltaTime);

	static float acc = 0.f;
	acc += deltaTime;

	TVec2 vPosition = GetPosition();

	float radius = 100.0f;
	float camX = UMath::Sin(acc) * radius + vPosition.X;
	float camY = UMath::Cos(acc) * radius + vPosition.Y;
	mViewMatrix = TMatrix::LookAt(TVec3(camX, camY, -300), TVec3(0.0, 0.0, 0.0), TVec3(0.0, 1.0, 0.0));

	if (mProjectionLerp < 1.f)
	{
		mProjectionLerp = UMath::Min(mProjectionLerp + deltaTime, 1.f);

		auto from = CreateProjectionMatrix(mProjection);
		auto to = CreateProjectionMatrix(mTargetProjection);

		float fLerp = powf(mProjectionLerp, mTargetProjection == EProjection::Orthographic ? 0.05f : 6.f);
		mProjectionMatrix = TMatrix::Lerp(from, to, fLerp);

		if (mProjectionLerp >= 1.f)
		{
			mProjection = mTargetProjection;
		}
	}
}

void CCamera::SetProjection(EProjection projection)
{
	if (mTargetProjection != projection)
	{
		mTargetProjection = projection;
		mProjectionLerp = 0.f;
	}
}

TMatrix CCamera::CreateProjectionMatrix(EProjection projection)
{
	if (mCurrentViewport == nullptr)
		return TMatrix();

	uint32 width = mCurrentViewport->GetWidth();
	uint32 height = mCurrentViewport->GetHeight();

	if (projection == EProjection::Perspective)
	{
		return TMatrix::Perspective(glm::radians(45.0f), static_cast<float>(width) / static_cast<float>(height), 0.1f, 100000.0f);
	}
	else if (projection == EProjection::Orthographic)
	{
		return TMatrix::Orthographic(width * -0.5f, width * 0.5f, height * -0.5f, height * 0.5f, 0.1f, 10000.0f);
	}

	return TMatrix();
}

void CCamera::SetViewport(CViewport* viewport)
{
	mCurrentViewport = viewport;

	mProjectionLerp = 1.f;
	mProjection = mTargetProjection;
	mProjectionMatrix = CreateProjectionMatrix(mProjection);
}

void CCamera::OnViewportSizeChanged()
{
	mProjectionLerp = 1.f;
	mProjection = mTargetProjection;
	mProjectionMatrix = CreateProjectionMatrix(mProjection);
}

void CCamera::MakeActive()
{
	CGameEngine::Instance()->GetWindow()->GetViewport()->SetCamera(this);
}

const TMatrix& CCamera::GetViewMatrix() const
{
	return mViewMatrix;
}

const TMatrix& CCamera::GetProjectionMatrix() const
{
	return mProjectionMatrix;
}
