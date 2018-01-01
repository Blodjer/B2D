#include "GameInstance.h"
#include "GameEngine.h"
#include <SDL2\SDL.h>
#include <algorithm>
#include "PlayerController.h"
#include "Level.h"
#include "Input.h"
#include "Debug.h"

std::map<int, PlayerController*> GameInstance::PlayerControllers;

GameInstance::GameInstance()
{

}

GameInstance::~GameInstance()
{
	delete this->LoadedLevel;

	for (auto pPlayerController : this->PlayerControllers)
	{
		delete pPlayerController.second;
	}
	this->PlayerControllers.clear();
}

void GameInstance::HandleInput(SDL_Event& pEvent)
{
	if (pEvent.type == SDL_KEYDOWN)
	{
		if (pEvent.key.repeat == 0)
		{
			if (pEvent.key.keysym.scancode == SDL_SCANCODE_ESCAPE)
			{
				GameEngine::Instance()->Shutdown();
				return;
			}

			for (auto pPlayerController : this->PlayerControllers)
			{
				pPlayerController.second->ProcessInputKey(pEvent.key.keysym.scancode, KeyEvent::KEY_DOWN);
			}
		}
	}
	else if (pEvent.type == SDL_KEYUP)
	{
		if (pEvent.key.repeat == 0)
		{
			for (auto pPlayerController : this->PlayerControllers)
			{
				pPlayerController.second->ProcessInputKey(pEvent.key.keysym.scancode, KeyEvent::KEY_UP);
			}
		}
	}
}

void GameInstance::Tick(float fDeltaTime)
{
	if (this->LoadedLevel != NULL)
	{
		this->LoadedLevel->Tick(fDeltaTime);
	}
}

void GameInstance::Draw(Graphics* pGraphics)
{
	if (this->LoadedLevel != NULL)
	{
		this->LoadedLevel->Draw(pGraphics);
	}
}

PlayerController* GameInstance::AddPlayerController(int iId)
{
	if (PlayerControllers.find(iId) == PlayerControllers.end())
	{
		PlayerControllers[iId] = new PlayerController(iId);
	}

	return PlayerControllers[iId];
}

void GameInstance::RemovePlayerController(int iId)
{
	PlayerControllers.erase(iId);
}
