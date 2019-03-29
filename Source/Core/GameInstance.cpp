#include "B2D_pch.h"
#include "GameInstance.h"

#include "Core/Core.h"
#include "GameEngine.h"
#include "Graphics/Viewport.h"
#include "LocalPlayer.h"
#include "ECS/World.h"
#include "ECS/Component/HoverComponent.h"
#include "ECS/Component/SpriteComponent.h"
#include "ECS/Component/TransformComponent.h"
#include "ECS/System/CharacterMovementSystem.h"
#include "ECS/System/HoverSystem.h"
#include "ECS/System/InputSystem.h"
#include "ECS/System/RenderSystem.h"
#include "Graphics/Shader.h"

CGameInstance::CGameInstance(CWindow* const owningWindow)
    : mWindow(owningWindow)
{
    mWorld = new World(this);

    mWorld->AddSystem<InputSystem>();
    mWorld->AddSystem<HoverSystem>();
    mWorld->AddSystem<CharacterMovementSystem>();
    mWorld->AddSystem<RenderSystem>();
}

CGameInstance::~CGameInstance()
{
    delete mWorld;

	for (auto p : mLocalPlayers)
	{
		delete p;
	}
}

void CGameInstance::Tick(float deltaTime)
{
    if (B2D_CHECKf(mWorld == nullptr, "No world loaded!"))
    {
        return;
    }

    mWorld->Update(deltaTime);
}

LocalPlayer* CGameInstance::AddLocalPlayer()
{
    mLocalPlayerIdCount++;
	LocalPlayer* newLocalPlayer = mLocalPlayers.emplace_back(new LocalPlayer(mLocalPlayerIdCount));

	return newLocalPlayer;
}

void CGameInstance::RemoveLocalPlayer(uint32 const playerId)
{
    for (std::vector<LocalPlayer*>::iterator it; it < mLocalPlayers.end(); ++it)
    {
        if ((*it)->GetId() == playerId)
        {
            std::iter_swap(it, mLocalPlayers.end());
            mLocalPlayers.pop_back();
        }
    }
}

LocalPlayer* CGameInstance::GetLocalPlayer(uint32 const playerId) const
{
    auto it = std::find_if(mLocalPlayers.begin(), mLocalPlayers.end(), [&playerId](LocalPlayer* player) {
        return player->GetId() == playerId;
    });

    if (it != mLocalPlayers.end())
    {
        return nullptr;
    }

    return *it;
}
