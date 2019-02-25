#pragma once

#include "Application.h"
#include "Core/GameEngine.h"
#include "Core/GameInstance.h"

extern void B2D::Config(ApplicationConfig& config);
extern void B2D::PopulateLevel(CLevel* const level);

int main(int argc, const char*[])
{
	ApplicationConfig config;
	B2D::Config(config);

	Log::Init(config.name);

	CGameEngine* const engine = new CGameEngine(config);

	CLevel* const level = engine->GetGameInstance()->LoadLevel<CLevel>();
    B2D::PopulateLevel(level);

	engine->Run();

	delete engine;
}