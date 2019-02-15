#pragma once

#include "Core/Vector2.h"

#include <vector>

class CComponent;

class CGameObject
{
public:
	CGameObject();
	virtual ~CGameObject();
	
	virtual void Update(float fDeltaTime);

	template<class T>
	T* AddComponent()
	{
		static_assert(std::is_base_of<CComponent, T>::value, "T must inherit from CComponent");

		T* component = new T(this);
		mComponentsToAdd.push_back(component);
		return component;
	}

	void RemoveComponent(CComponent* const pComponent);

	void SetPosition(SVector2 position, bool sweep = false);
	SVector2 GetPosition() const { return mPosition; }

	const std::vector<CComponent*>& GetComponents() const;

private:
	SVector2 mPosition = SVector2::Zero;

	std::vector<CComponent*> mComponentsToAdd;
	std::vector<CComponent*> mComponents;
	std::vector<CComponent*> mComponentsToRemove;
};
