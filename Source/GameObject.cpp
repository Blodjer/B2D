#include "GameObject.h"
#include <algorithm>
#include "IComponent.h"
#include "Debug.h"

GameObject::GameObject()
{
	
}

GameObject::~GameObject()
{
	for (auto component : this->ComponentsToAdd)
	{
		delete component;
	}

	for (auto component : this->Components)
	{
		delete component;
	}
}

void GameObject::RemoveComponent(IComponent* component)
{
	this->ComponentsToRemove.push_back(component);
}

void GameObject::Update(float deltaTime)
{
	for (auto component : this->ComponentsToAdd)
	{
		this->Components.push_back(component);
	}
	this->ComponentsToAdd.clear();

	for (auto component : this->Components)
	{
		component->Update(deltaTime);
	}

	for (auto component : this->ComponentsToRemove)
	{
		this->Components.erase(std::remove(this->Components.begin(), this->Components.end(), component), this->Components.end());
		delete component;
	}
	this->ComponentsToRemove.clear();
}

void GameObject::SetPosition(Vector2 vPosition, bool bSweep)
{
	if (bSweep)
	{
		// do collision test
	}

	this->Position = vPosition;
}

const std::vector<IComponent*>& GameObject::GetComponents() const
{
	return this->Components;
}
