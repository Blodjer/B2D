#include "B2D_pch.h"
#include "GameInstance.h"

#include "GameEngine.h"
#include "Graphics/Viewport.h"
#include "Input.h"
#include "Level.h"
#include "PlayerController.h"
#include "ECS/World.h"
#include "ECS/Component/HoverComponent.h"
#include "ECS/Component/SpriteComponent.h"
#include "ECS/Component/TransformComponent.h"
#include "ECS/System/CharacterMovementSystem.h"
#include "ECS/System/HoverSystem.h"
#include "ECS/System/InputSystem.h"
#include "ECS/System/RenderSystem.h"
#include "Graphics/Shader.h"

std::map<uint32, CPlayerController*> CGameInstance::mPlayerControllers;

CGameInstance::CGameInstance()
{
    mWorld = new World();

    mWorld->AddSystem<InputSystem>();
    mWorld->AddSystem<HoverSystem>();
    mWorld->AddSystem<CharacterMovementSystem>();
    mWorld->AddSystem<RenderSystem>();
}

CGameInstance::~CGameInstance()
{
    delete mWorld;

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
    if (mWorld == nullptr)
    {
        B2D_CORE_ERROR("No world loaded");
        return;
    }

    mWorld->Update(deltaTime);
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
