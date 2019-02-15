#include "Camera.h"

#include "Core/GameEngine.h"
#include "Graphics/Graphics.h"
#include "Graphics/Viewport.h"
#include "Math/BMath.h"

CCamera::CCamera()
	: CGameObject()
{
	mProjection = EProjection::Perspective;
	mTargetProjection = mProjection;
	mProjectionLerp = 0.f;

	SVector2 const vPosition = GetPosition();
	mViewMatrix = glm::lookAt(
		glm::vec3(vPosition.X, vPosition.Y, -300.0f),
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 1.0f, 0.0f));

	mProjectionMatrix = CreateProjectionMatrix(mProjection);
}

static glm::mat4 MatrixLerp(const glm::mat4& from, const glm::mat4& to, float fTime)
{
	float ret[16] = { 0.0 };
	const float* f = (const float*)glm::value_ptr(from);
	const float* t = (const float*)glm::value_ptr(to);
	for (int i = 0; i < 16; i++)
		ret[i] = f[i] + fTime * (t[i] - f[i]);

	return glm::make_mat4(ret);
}

void CCamera::Update(float deltaTime)
{
	CGameObject::Update(deltaTime);

	static float acc = 0.f;
	acc += deltaTime;

	SVector2 vPosition = GetPosition();

	float radius = 100.0f;
	float camX = sin(acc) * radius + vPosition.X;
	float camY = cos(acc) * radius + vPosition.Y;
	mViewMatrix = glm::lookAt(glm::vec3(camX, camY, -300), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0));

	if (mProjectionLerp < 1.f)
	{
		mProjectionLerp = BMath::Min(mProjectionLerp + deltaTime, 1.f);

		auto from = CreateProjectionMatrix(mProjection);
		auto to = CreateProjectionMatrix(mTargetProjection);

		float fLerp = powf(mProjectionLerp, mTargetProjection == EProjection::Orthographic ? 0.05f : 6.f);
		mProjectionMatrix = MatrixLerp(from, to, fLerp);

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

glm::mat4 CCamera::CreateProjectionMatrix(EProjection projection)
{
	if (mCurrentViewport == nullptr)
		return glm::mat4();

	int width = mCurrentViewport->GetWidth();
	int height = mCurrentViewport->GetHeight();

	if (projection == EProjection::Perspective)
	{
		return glm::perspective<float>(glm::radians(45.0f), (float)width / (float)height, 0.1f, 100000.0f);
	}
	else if (projection == EProjection::Orthographic)
	{
		return glm::ortho<float>(width * -0.5f, width * 0.5f, -height * 0.5f, height * 0.5f, 0.1f, 10000.0f);
	}

	return glm::mat4();
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
	CGameEngine::Instance()->GetGraphicsInstance()->GetViewport()->SetCamera(this);
}

const float* CCamera::GetViewMatrix() const
{
	return glm::value_ptr(mViewMatrix);
}

const float* CCamera::GetProjectionMatrix() const
{
	return glm::value_ptr(mProjectionMatrix);
}
