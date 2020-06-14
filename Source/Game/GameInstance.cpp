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
    : m_window(owningWindow)
{
    m_world = new World(this);

    m_world->AddSystem<InputSystem>();
    m_world->AddSystem<HoverSystem>();
    m_world->AddSystem<CharacterMovementSystem>();
    m_world->AddSystem<RenderSystem>();
}

CGameInstance::~CGameInstance()
{
    delete m_world;

	for (auto p : m_localPlayers)
	{
		delete p;
	}
}

void CGameInstance::Tick(float deltaTime)
{
    if (B2D_CHECKf(m_world == nullptr, "No world loaded!"))
    {
        return;
    }

    m_world->Tick(deltaTime);
}

LocalPlayer* CGameInstance::AddLocalPlayer()
{
    m_localPlayerIdCount++;
	LocalPlayer* newLocalPlayer = m_localPlayers.emplace_back(new LocalPlayer(m_localPlayerIdCount));

	return newLocalPlayer;
}

void CGameInstance::RemoveLocalPlayer(uint32 const playerId)
{
    for (std::vector<LocalPlayer*>::iterator it; it < m_localPlayers.end(); ++it)
    {
        if ((*it)->GetId() == playerId)
        {
            std::iter_swap(it, m_localPlayers.end() - 1);
            m_localPlayers.pop_back();
        }
    }
}

LocalPlayer* CGameInstance::GetLocalPlayer(uint32 const playerId) const
{
    auto it = std::find_if(m_localPlayers.begin(), m_localPlayers.end(), [&playerId](LocalPlayer* player) {
        return player->GetId() == playerId;
    });

    if (it != m_localPlayers.end())
    {
        return nullptr;
    }

    return *it;
}
