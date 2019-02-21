#include "Level.h"

#include "Camera.h"
#include "Component/ComponentCollider.h"
#include "GameObject.h"
#include "Graphics/Renderer.h"

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

CLevel::CLevel()
{
	CCamera* defaultCamera = SpawnGameObject<CCamera>();
	defaultCamera->MakeActive();
}

CLevel::~CLevel()
{
	for (CGameObject* gameObject : mGameObjects)
	{
		delete gameObject;
	}
}

CGameObject* CLevel::SpawnGameObject(TVec2 spawnPosition)
{
	return SpawnGameObject<CGameObject>(spawnPosition);
}

void CLevel::Tick(float deltaTime)
{
	for (CGameObject* gameObject : mGameObjects)
	{
		gameObject->Update(deltaTime);
	}

	for (CGameObject* gameObject : mGameObjectsToAdd)
	{
		mGameObjects.push_back(gameObject);
		gameObject->Update(0.f);
	}
	mGameObjectsToAdd.clear();

	HandleCollision();
}

void CLevel::HandleCollision()
{
	std::vector<CComponentCollider*> collisionComponents;

	for (const CGameObject* gameObject : mGameObjects)
	{
		for (const CComponent* component : gameObject->GetComponents())
		{
			// aCollisionComponents.push_back(pComponent);
		}
	}

	for (const CComponent* component : collisionComponents)
	{
		// if collision
	}
}

void CLevel::Draw(CViewport const* const viewport, CRenderer* renderer)
{
	renderer->Draw(viewport, mGameObjects);
}
