#include "B2D_pch.h"
#include "CameraEntity.h"

void CameraEntity::Initialize()
{
    TransformComponent& transform = GetComponent<TransformComponent>();
    transform.position.Z = -10.0f;
}

void CameraEntity::Update(float deltaTime)
{
    CameraComponent& camera = GetComponent<CameraComponent>();

    if (camera.projectionLerp < 1.f)
    {
        camera.projectionLerp = UMath::Min(camera.projectionLerp + deltaTime, 1.f);
        if (camera.projectionLerp >= 1.f)
        {
            camera.projection = camera.targetProjection;
        }
    }
    else
    {
        SetProjection(camera.projection == EProjection::Orthographic ? EProjection::Perspective : EProjection::Orthographic);
    }
}

void CameraEntity::SetProjection(EProjection const projection)
{
    CameraComponent& camera = GetComponent<CameraComponent>();

    if (camera.projection != projection)
    {
        camera.targetProjection = projection;
        camera.projectionLerp = 0.f;
    }
}

TMatrix CameraEntity::GetProjectionMatrix(CViewport const* const viewport) const
{
    CameraComponent const& camera = GetComponent<CameraComponent>();

    if (camera.projectionLerp < 1.f)
    {
        auto from = CreateProjectionMatrix(camera.projection, viewport);
        auto to = CreateProjectionMatrix(camera.targetProjection, viewport);

        float alpha = UMath::Pow(camera.projectionLerp, camera.targetProjection == EProjection::Orthographic ? 0.3f : 4.f);
        return TMatrix::Lerp(from, to, alpha);
    }
    else
    {
        return CreateProjectionMatrix(camera.projection, viewport);
    }
}

TMatrix CameraEntity::CreateProjectionMatrix(EProjection const projection, CViewport const* const viewport) const
{
    CameraComponent const& camera = GetComponent<CameraComponent>();
    
    if (projection == EProjection::Perspective)
    {
        if (camera.useAspectRatio && camera.aspectRatio > 0.0f)
        {
            return TMatrix::Perspective(camera.fov, camera.aspectRatio, camera.nearPlane, camera.farPlane);
        }
        else
        {
            return TMatrix::Perspective(camera.fov, static_cast<float>(viewport->GetWidth()) / static_cast<float>(viewport->GetHeight()), camera.nearPlane, camera.farPlane);
        }
    }
    else if (projection == EProjection::Orthographic)
    {
        return TMatrix::Orthographic(viewport->GetWidth() * -0.5f, viewport->GetWidth() * 0.5f, viewport->GetHeight() * -0.5f, viewport->GetHeight() * 0.5f, camera.nearPlane, camera.farPlane);
    }

    return TMatrix();
}

TMatrix CameraEntity::GetViewMatrix() const
{
    TransformComponent& transform = GetComponent<TransformComponent>();
    return TMatrix::LookAt(transform.position, TVec3::Zero, TVec3::Up);
}
