#pragma once

#include "Core/Vector2.h"

#include <vector>

class CGameObject;
class CGraphics;

class CLevel
{
public:
	CLevel();
	virtual ~CLevel();

	CGameObject* SpawnGameObject(SVector2 spawnPosition = SVector2::Zero);

	template<class T>
	T* SpawnGameObject(SVector2 spawnPosition = SVector2::Zero)
	{
		static_assert(std::is_base_of<CGameObject, T>::value, "T must inherit from CGameObject");

		T* gameObject = new T();
		gameObject->SetPosition(spawnPosition);

		mGameObjectsToAdd.push_back(gameObject);

		return gameObject;
	}

	void Tick(float deltaTime);
	void HandleCollision();

	void Draw(CGraphics* pGraphics);

private:
	std::vector<CGameObject*> mGameObjectsToAdd;
	std::vector<CGameObject*> mGameObjects;
};

