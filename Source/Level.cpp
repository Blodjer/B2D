#include "Level.h"
#include "GameObject.h"
#include "Graphics.h"
#include "Debug.h"
#include "ComponentCollider.h"
#include "Camera.h"

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
	CCamera* pDefaultCamera = this->SpawnGameObject<CCamera>();
	pDefaultCamera->MakeActive();
}

CLevel::~CLevel()
{
	for (CGameObject* pGameObject : this->GameObjects)
	{
		delete pGameObject;
	}
}

CGameObject* CLevel::SpawnGameObject(SVector2 vSpawnPosition)
{
	return this->SpawnGameObject<CGameObject>(vSpawnPosition);
}

void CLevel::Tick(float fDeltaTime)
{
	for (CGameObject* pGameObject : this->GameObjects)
	{
		pGameObject->Update(fDeltaTime);
	}

	for (CGameObject* pGameObject : this->GameObjectsToAdd)
	{
		this->GameObjects.push_back(pGameObject);
		pGameObject->Update(0.f);
	}
	this->GameObjectsToAdd.clear();

	this->HandleCollision();
}

void CLevel::HandleCollision()
{
	std::vector<CComponentCollider*> aCollisionComponents;

	for (const CGameObject* pGameObject : this->GameObjects)
	{
		for (const CComponent* pComponent : pGameObject->GetComponents())
		{
			// aCollisionComponents.push_back(pComponent);
		}
	}

	for (const CComponent* pComponent : aCollisionComponents)
	{
		// if collision
	}
}

void CLevel::Draw(CGraphics* pGraphics)
{
	for (const CGameObject* pGameObject : this->GameObjects)
	{
		pGraphics->Draw(pGameObject);
	}
}
