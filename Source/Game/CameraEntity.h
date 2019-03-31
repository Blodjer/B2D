#pragma once
#include "Game/Core/SystemEntityObject.h"
#include "Game/Component/CameraComponent.h"
#include "Game/Component/TransformComponent.h"

class CameraEntity : public SystemEntityObject<CameraComponent, TransformComponent>
{
    DECLARE_SYSTEMENTITY(CameraEntity);

public:
    virtual void Initialize() override;
    virtual void Update(float deltaTime) override;

    void SetProjection(EProjection const projection);

    TMatrix GetProjectionMatrix(CViewport const* const viewport) const;
    TMatrix GetViewMatrix() const;

private:
    TMatrix CreateProjectionMatrix(EProjection const projection, CViewport const* const viewport) const;

};

