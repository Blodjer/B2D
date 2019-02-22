#include "GameObject.h"

#include "Component/Component.h"

#include <algorithm>

CGameObject::CGameObject()
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

void CGameObject::SetPosition(TVec2 position, bool sweep)
{
	if (sweep)
	{
		// do collision test
	}

	mPosition = position;
}

const std::vector<CComponent*>& CGameObject::GetComponents() const
{
	return mComponents;
}
