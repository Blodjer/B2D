#pragma once

#include "Application.h"
#include "Core/GameEngine.h"
#include "Core/GameInstance.h"

extern void B2D::Config(ApplicationConfig& config);

int main(int argc, const char*[])
{
	ApplicationConfig config;
	B2D::Config(config);

#if _DEBUG
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), { 0, 1 });
#endif
	Log::Init(config.name);

	CGameEngine* const engine = new CGameEngine(config);
	engine->GetGameInstance()->LoadLevel<TestScene>();

	engine->Run();

	delete engine;
}