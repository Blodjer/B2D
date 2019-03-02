#include "B2D_pch.h"
#include "GameObject.h"

#include "Component/Component.h"
#include "Math/Transform.h"

CGameObject::CGameObject()
    : mTransform()
    , mTransformMatrix(TMatrix::Identity)
    , mTransformMatrixIsInvalid(true)
{
    
}

CGameObject::~CGameObject()
{
	for (CComponent* const component : mComponents)
	{
		delete component;
	}
}

void CGameObject::RemoveComponent(CComponent* const component)
{
	mComponentsToRemove.push_back(component);
}

std::vector<CComponent*> const& CGameObject::GetComponents() const
{
    return mComponents;
}

void CGameObject::Update(float deltaTime)
{
	for (CComponent* const component : mComponents)
	{
		component->Update(deltaTime);
	}

	for (CComponent* const component : mComponentsToRemove)
	{
		mComponents.erase(std::remove(mComponents.begin(), mComponents.end(), component), mComponents.end());
		delete component;
	}
	mComponentsToRemove.clear();
}

void CGameObject::SetPosition(TVec3 position, bool sweep)
{
    if (mTransform.GetPosition() == position)
    {
        return;
    }

	if (sweep)
	{
		// do collision test
	}

    mTransform.SetPosition(position);

    InvalidateTransformMatrix();
}

TMatrix const& CGameObject::GetTransformMatrix()
{
    if (mTransformMatrixIsInvalid)
    {
        mTransform.CalculateMatrix(mTransformMatrix);
        mTransformMatrixIsInvalid = false;
    }

    return mTransformMatrix;
}

void CGameObject::InvalidateTransformMatrix()
{
    mTransformMatrixIsInvalid = true;

    for (CComponent* const c : GetComponents())
    {
        c->InvalidateWorldTransformMatrix();
    }
}
