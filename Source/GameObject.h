#pragma once

#include "Core/Core.h"
#include "Math/Transform.h"

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

    std::vector<CComponent*> const& GetComponents() const;

	void SetPosition(TVec3 position, bool sweep = false);
	TVec3 GetPosition() const { return mTransform.GetPosition(); }

    TMatrix const& GetTransformMatrix();

private:
    void InvalidateTransformMatrix();

private:
    Transform mTransform;
    TMatrix mTransformMatrix;
    bool mTransformMatrixIsInvalid;

	std::vector<CComponent*> mComponents;
	std::vector<CComponent*> mComponentsToRemove;
};
