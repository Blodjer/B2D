#pragma once

class CViewport;

class CCamera final
{
	friend CViewport;

public:
	enum class EProjection
	{
		Perspective,
		Orthographic
	};

public:
	CCamera();
	~CCamera() = default;

	void Update(float deltaTime);

	void SetProjection(EProjection projection);
	TMatrix CreateProjectionMatrix(EProjection projection);

	void MakeActive();

    TMatrix const& GetViewMatrix() const { return mViewMatrix; }
	TMatrix const& GetProjectionMatrix() const { return mProjectionMatrix; }

private:
	void SetViewport(CViewport* viewport);
	void OnViewportSizeChanged();

private:
	TMatrix mViewMatrix;
    TMatrix mProjectionMatrix;

	EProjection mProjection;
	EProjection mTargetProjection;
	float mProjectionLerp;

	CViewport* mCurrentViewport = nullptr;
};

