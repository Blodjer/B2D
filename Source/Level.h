#pragma once

#include <vector>
#include "Vector2.h"

class Level
{
public:
	Level();
	virtual ~Level();

	class GameObject* SpawnGameObject(Vector2 vSpawnPosition = Vector2::Zero);

	template<class T>
	T* SpawnGameObject(Vector2 vSpawnPosition = Vector2::Zero)
	{
		static_assert(std::is_base_of<GameObject, T>::value, "T must inherit from GameObject");

		T* pGameObject = new T();
		pGameObject->SetPosition(vSpawnPosition);

		this->GameObjectsToAdd.push_back(pGameObject);

		return pGameObject;
	}

	void Tick(float fDeltaTime);
	void HandleCollision();

	void Draw(class Graphics* pGraphics);

private:
	std::vector<class GameObject*> GameObjectsToAdd;
	std::vector<class GameObject*> GameObjects;
};

