#include "B2D_pch.h"
#include "Component.h"

#include "GameObject.h"

CComponent::CComponent(CGameObject* const owner)
    : mOwner(owner)
    , mRelativeTransform()
    , mWorldTransformMatrix(TMatrix::Identity)
    , mWorldTransformMatrixIsInvalid(true)
{

}

TVec3 const& CComponent::GetRelativePosition() const
{
	return mRelativeTransform.GetPosition();
}

void CComponent::SetRelativePosition(TVec3 const& position)
{
    if (mRelativeTransform.GetPosition() == position)
    {
        return;
    }

    mRelativeTransform.SetPosition(position);
    
    InvalidateWorldTransformMatrix();
}

TVec3 CComponent::GetWorldPosition() const
{
    return mOwner->GetPosition() + GetRelativePosition();
}

TMatrix const& CComponent::GetWorldTransformMatrix()
{
    if (mWorldTransformMatrixIsInvalid)
    {
        mWorldTransformMatrix = mOwner->GetTransformMatrix();
        mWorldTransformMatrix = TMatrix::Translate(mWorldTransformMatrix, mRelativeTransform.GetPosition());
        mWorldTransformMatrix = TMatrix::Scale(mWorldTransformMatrix, mRelativeTransform.GetScale());

        mWorldTransformMatrixIsInvalid = false;
    }

    return mWorldTransformMatrix;
}

void CComponent::InvalidateWorldTransformMatrix()
{
    mWorldTransformMatrixIsInvalid = true;
}
