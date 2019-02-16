#pragma once

#include "GameObject.h"

class CViewport;

class CCamera final : public CGameObject
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

	virtual void Update(float deltaTime) override;

	void SetProjection(EProjection projection);
	TMatrix CreateProjectionMatrix(EProjection projection);

	void MakeActive();

	TMatrix const& GetViewMatrix() const;
	TMatrix const& GetProjectionMatrix() const;

private:
	void SetViewport(CViewport* viewport);
	void OnViewportSizeChanged();

private:
	TMatrix mViewMatrix;
	TMatrix mProjectionMatrix;

	EProjection mProjection;
	EProjection mTargetProjection;
	float mProjectionLerp;

	CViewport* mCurrentViewport;
};

