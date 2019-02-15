#include "Component.h"

#include "GameObject.h"

CComponent::CComponent(CGameObject* const owner)
	: mOwner(owner)
{

}

SVector2 CComponent::GetWorldPosition() const
{
	return mOwner->GetPosition() + mRelativPosition;
}

const SVector2& CComponent::GetRelativePosition() const
{
	return mRelativPosition;
}

void CComponent::SetRelativePosition(const SVector2& position)
{
	mRelativPosition = position;
}
