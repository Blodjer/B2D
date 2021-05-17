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

	GameEngine::Create(config);

	World* const world = GameEngine::Instance()->GetGameInstance()->GetWorld();
    //B2D::PopulateWorld(world);
	B2D_LOG_WARNING("Disabled loading of world!");

    GameEngine::Instance()->Run();
    GameEngine::Shutdown();

	return 0;
}