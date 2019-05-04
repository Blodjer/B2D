#include "B2D_pch.h"
#include "GameInstance.h"

#include "Core/Core.h"
#include "GameEngine.h"
#include "Graphics/Viewport.h"
#include "LocalPlayer.h"
#include "Game/Core/World.h"
#include "Game/Component/HoverComponent.h"
#include "Game/Component/SpriteComponent.h"
#include "Game/Component/TransformComponent.h"
#include "Game/System/CharacterMovementSystem.h"
#include "Game/System/HoverSystem.h"
#include "Game/System/InputSystem.h"
#include "Game/System/RenderSystem.h"
#include "Graphics/Renderer/WorldRenderer.h"
#include "Graphics/Shader.h"
#include "Graphics/RenderManger.h"

CGameInstance::CGameInstance(GenericWindow* const owningWindow)
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
            std::iter_swap(it, mLocalPlayers.end() - 1);
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
