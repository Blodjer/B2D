#include "GameInstance.h"

#include "Debug/Debug.h"
#include "GameEngine.h"
#include "Input.h"
#include "Level.h"
#include "PlayerController.h"

#include <algorithm>

std::map<int, CPlayerController*> CGameInstance::PlayerControllers;

CGameInstance::CGameInstance()
{

}

CGameInstance::~CGameInstance()
{
	delete this->LoadedLevel;

	for (const auto& pPlayerController : this->PlayerControllers)
	{
		delete pPlayerController.second;
	}
	this->PlayerControllers.clear();
}

void CGameInstance::HandleInput(int pEvent)
{
	/*
	if (pEvent.type == SDL_KEYDOWN)
	{
		if (pEvent.key.repeat == 0)
		{
			if (pEvent.key.keysym.scancode == SDL_SCANCODE_ESCAPE)
			{
				CGameEngine::Instance()->Shutdown();
				return;
			}

			for (auto pPlayerController : this->PlayerControllers)
			{
				pPlayerController.second->ProcessInputKey(pEvent.key.keysym.scancode, EKeyEvent::KEY_DOWN);
			}
		}
	}
	else if (pEvent.type == SDL_KEYUP)
	{
		if (pEvent.key.repeat == 0)
		{
			for (auto pPlayerController : this->PlayerControllers)
			{
				pPlayerController.second->ProcessInputKey(pEvent.key.keysym.scancode, EKeyEvent::KEY_UP);
			}
		}
	}
	*/
}

void CGameInstance::Tick(float fDeltaTime)
{
	if (this->LoadedLevel != NULL)
	{
		this->LoadedLevel->Tick(fDeltaTime);
	}
}

void CGameInstance::Draw(CGraphics* pGraphics)
{
	if (this->LoadedLevel != NULL)
	{
		this->LoadedLevel->Draw(pGraphics);
	}
}

CPlayerController* CGameInstance::AddPlayerController(int iId)
{
	if (PlayerControllers.find(iId) == PlayerControllers.end())
	{
		PlayerControllers[iId] = new CPlayerController(iId);
	}

	return PlayerControllers[iId];
}

void CGameInstance::RemovePlayerController(int iId)
{
	PlayerControllers.erase(iId);
}
