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

	CGameObject* SpawnGameObject(TVec3 spawnPosition = TVec3::Zero);

	template<class T>
	T* SpawnGameObject(TVec3 spawnPosition = TVec3::Zero)
	{
        B2D_STATIC_ASSERT_TYPE(CGameObject, T);

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

