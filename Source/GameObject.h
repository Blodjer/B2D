#pragma once

#include <vector>
#include "Vector2.h"

class GameObject
{
public:
	GameObject();
	~GameObject();

	virtual void Update(float fDeltaTime);

	template<class T>
	T* AddComponent()
	{
		static_assert(std::is_base_of<IComponent, T>::value, "T must inherit from IComponent");

		T* pComponent = new T(this);
		this->ComponentsToAdd.push_back(pComponent);
		return pComponent;
	}

	void RemoveComponent(class IComponent* pComponent);

	void SetPosition(Vector2 vPosition, bool bSweep = false);
	Vector2 GetPosition() { return this->Position; }

	const std::vector<IComponent*>& GetComponents() const;

private:
	Vector2 Position = Vector2::Zero;

	std::vector<IComponent*> ComponentsToAdd;
	std::vector<IComponent*> Components;
	std::vector<IComponent*> ComponentsToRemove;
};
