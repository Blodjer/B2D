#pragma once

#include "Application.h"
#include "GameEngine.h"
#include "Game/GameInstance.h"

extern void B2D::Config(ApplicationConfig& config);
extern void B2D::PopulateWorld(World* const world);

int main(int argc, const char*[])
{
	ApplicationConfig config;
	B2D::Config(config);

	Log::Init(config.name);

	CGameEngine::Create(config);

	World* const world = CGameEngine::Instance()->GetGameInstance()->GetWorld();
    B2D::PopulateWorld(world);

    CGameEngine::Instance()->Run();
    CGameEngine::Instance()->Shutdown();
}