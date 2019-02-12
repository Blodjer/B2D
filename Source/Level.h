#pragma once

#include <vector>
#include "Vector2.h"

class CLevel
{
public:
	CLevel();
	virtual ~CLevel();

	class CGameObject* SpawnGameObject(SVector2 vSpawnPosition = SVector2::Zero);

	template<class T>
	T* SpawnGameObject(SVector2 vSpawnPosition = SVector2::Zero)
	{
		static_assert(std::is_base_of<CGameObject, T>::value, "T must inherit from CGameObject");

		T* pGameObject = new T();
		pGameObject->SetPosition(vSpawnPosition);

		this->GameObjectsToAdd.push_back(pGameObject);

		return pGameObject;
	}

	void Tick(float fDeltaTime);
	void HandleCollision();

	void Draw(class CGraphics* pGraphics);

private:
	std::vector<class CGameObject*> GameObjectsToAdd;
	std::vector<class CGameObject*> GameObjects;
};

