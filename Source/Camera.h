#pragma once

#include "GameObject.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

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
	glm::mat4 CreateProjectionMatrix(EProjection projection);

	void MakeActive();

	const float* GetViewMatrix() const;
	const float* GetProjectionMatrix() const;

private:
	void SetViewport(CViewport* viewport);
	void OnViewportSizeChanged();

private:
	glm::mat4 mViewMatrix;
	glm::mat4 mProjectionMatrix;

	EProjection mProjection;
	EProjection mTargetProjection;
	float mProjectionLerp;

	CViewport* mCurrentViewport;
};

