#include "B2D_pch.h"
#include "CameraEntity.h"
#include "Input/Input.h"

void CameraEntity::Initialize()
{
    TransformComponent& transform = GetComponent<TransformComponent>();
    transform.position.z = -10.0f;
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
}

void CameraEntity::SetProjection(EProjection const projection)
{
    CameraComponent& camera = GetComponent<CameraComponent>();

    if (camera.targetProjection != projection)
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
        float aspect = static_cast<float>(viewport->GetWidth()) / static_cast<float>(viewport->GetHeight());
        float invAspect = static_cast<float>(viewport->GetHeight()) / static_cast<float>(viewport->GetWidth());

        float fovRadians = UMath::Max(0.001f, UMath::DegToRad(camera.fov));
        float horizontalFov = 2.0f * UMath::ArcTan(UMath::Tan(fovRadians * 0.5f) * invAspect);

        return TMatrix::Perspective(horizontalFov, aspect, camera.nearPlane, camera.farPlane);
    }
    else if (projection == EProjection::Orthographic)
    {
        float aspect = static_cast<float>(viewport->GetHeight()) / static_cast<float>(viewport->GetWidth());
        float width = camera.orthoWidth; UMath::Max(0.001f, camera.orthoWidth);
        float height = width * aspect;
        return TMatrix::Orthographic(width * -0.5f, width * 0.5f, height * -0.5f, height * 0.5f, camera.nearPlane, camera.farPlane);
    }

    B2D_TRAP_f("Projection mode {} not implemented", static_cast<uint32>(projection));
    return TMatrix();
}

TMatrix CameraEntity::GetViewMatrix() const
{
    TransformComponent& transform = GetComponent<TransformComponent>();
    return TMatrix::LookAt(transform.position, TVec3::Zero, TVec3::Up);
}
