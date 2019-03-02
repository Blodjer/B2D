#pragma once

#include "Core/Core.h"
#include "Math/Transform.h"

class CGameObject;

class CComponent
{
public:
	CComponent(CGameObject* const owner);
	virtual ~CComponent() = default;
	virtual void Update(float deltaTime) { };

	void SetRelativePosition(TVec3 const& position);
	TVec3 const& GetRelativePosition() const;

    TVec3 GetWorldPosition() const;

    TMatrix const& GetWorldTransformMatrix();
    void InvalidateWorldTransformMatrix();

protected:
	CGameObject* const mOwner;

private:
    Transform mRelativeTransform;

    TMatrix mWorldTransformMatrix;
    bool mWorldTransformMatrixIsInvalid;

};
