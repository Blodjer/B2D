#pragma once

#include "Application.h"
#include "Core/GameEngine.h"
#include "Core/GameInstance.h"

extern void B2D::Config(ApplicationConfig& config);
extern void B2D::PopulateWorld(World* const world);

int main(int argc, const char*[])
{
	ApplicationConfig config;
	B2D::Config(config);

	Log::Init(config.name);

	CGameEngine* const engine = new CGameEngine(config);

	World* const world = engine->GetGameInstance()->GetWorld();
    B2D::PopulateWorld(world);

	engine->Run();

	delete engine;
}