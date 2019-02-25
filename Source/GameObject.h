#pragma once

#include "Core/Core.h"

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
        B2D_STATIC_ASSERT_TYPE(CComponent, T)

		T* component = new T(this);
        mComponents.push_back(component);
		return component;
	}

	void RemoveComponent(CComponent* const pComponent);

	void SetPosition(TVec2 position, bool sweep = false);
	TVec2 GetPosition() const { return mPosition; }

	const std::vector<CComponent*>& GetComponents() const;

private:
	TVec2 mPosition = TVec2::Zero;

	std::vector<CComponent*> mComponents;
	std::vector<CComponent*> mComponentsToRemove;
};
