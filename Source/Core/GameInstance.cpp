#include "GameInstance.h"

#include "GameEngine.h"
#include "Graphics/Viewport.h"
#include "Input.h"
#include "Level.h"
#include "PlayerController.h"

#include <algorithm>

std::map<uint32, CPlayerController*> CGameInstance::mPlayerControllers;

CGameInstance::CGameInstance()
{

}

CGameInstance::~CGameInstance()
{
	delete mLoadedLevel;

	for (const auto& pPlayerController : mPlayerControllers)
	{
		delete pPlayerController.second;
	}
	mPlayerControllers.clear();
}

void CGameInstance::HandleInput(uint32 pEvent)
{
	
}

void CGameInstance::Tick(float deltaTime)
{
	if (mLoadedLevel != nullptr)
	{
		mLoadedLevel->Tick(deltaTime);
	}
}

void CGameInstance::Draw(CViewport const* const viewport, CRenderer* graphics)
{
	if (mLoadedLevel != nullptr)
	{
		mLoadedLevel->Draw(viewport, graphics);
	}
}

CPlayerController* CGameInstance::AddPlayerController(uint32 id)
{
	if (mPlayerControllers.find(id) == mPlayerControllers.end())
	{
		mPlayerControllers[id] = new CPlayerController(id);
	}

	return mPlayerControllers[id];
}

void CGameInstance::RemovePlayerController(uint32 id)
{
	mPlayerControllers.erase(id);
}
