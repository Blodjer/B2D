#pragma once

#include "Core/Core.h"

class CGameObject;
class CRenderer;
class CViewport;

class CLevel
{
public:
	CLevel();
	virtual ~CLevel();

	CGameObject* SpawnGameObject(TVec2 spawnPosition = TVec2::Zero);

	template<class T>
	T* SpawnGameObject(TVec2 spawnPosition = TVec2::Zero)
	{
		static_assert(std::is_base_of<CGameObject, T>::value, "T must inherit from CGameObject");

		T* gameObject = new T();
		gameObject->SetPosition(spawnPosition);

		mGameObjectsToAdd.push_back(gameObject);

		return gameObject;
	}

	void Tick(float deltaTime);
	void HandleCollision();

	void Draw(CViewport const* const viewport, CRenderer* renderer);

private:
	std::vector<CGameObject*> mGameObjectsToAdd;
	std::vector<CGameObject*> mGameObjects;
};

