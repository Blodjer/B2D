#include "Level.h"
#include "GameObject.h"
#include "Graphics.h"
#include "Debug.h"
#include "ComponentCollider.h"

/*
receive input
update physics
collision detection
collision resolution
render
repeat

AABB
SAT
Raycast
*/

Level::Level()
{
	
}

Level::~Level()
{
	for (auto gameObject : this->GameObjects)
	{
		delete gameObject;
	}
}

GameObject* Level::SpawnGameObject(Vector2 vSpawnPosition)
{
	return this->SpawnGameObject<GameObject>(vSpawnPosition);
}

void Level::Tick(float fDeltaTime)
{
	for (auto pGameObject : this->GameObjects)
	{
		pGameObject->Update(fDeltaTime);
	}

	for (auto pGameObject : this->GameObjectsToAdd)
	{
		this->GameObjects.push_back(pGameObject);
		pGameObject->Update(0.f);
	}
	this->GameObjectsToAdd.clear();

	this->HandleCollision();
}

void Level::HandleCollision()
{
	std::vector<ComponentCollider*> aCollisionComponents;

	for (auto pGameObject : this->GameObjects)
	{
		for (auto pComponent : pGameObject->GetComponents())
		{
			//aCollisionComponents.push_back(pComponent);
		}
	}

	for (auto pComponent : aCollisionComponents)
	{
		// if collision
	}
}

void Level::Draw(Graphics* pGraphics)
{
	for (auto pGameObject : this->GameObjects)
	{
		pGraphics->Draw(pGameObject);
	}
}
