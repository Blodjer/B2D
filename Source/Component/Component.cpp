#include "Component.h"

#include "GameObject.h"

CComponent::CComponent(CGameObject* const owner)
	: mOwner(owner)
{

}

TVec2 CComponent::GetWorldPosition() const
{
	return mOwner->GetPosition() + mRelativPosition;
}

const TVec2& CComponent::GetRelativePosition() const
{
	return mRelativPosition;
}

void CComponent::SetRelativePosition(const TVec2& position)
{
	mRelativPosition = position;
}
