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
#include "ECS/Component/TestComponent.h"
#include "ECS/Component/TransformComponent.h"
#include "ECS/System/HoverSystem.h"
#include "ECS/System/TestSystem.h"
#include "ECS/System/RenderSystem.h"
#include "Graphics/Shader.h"

std::map<uint32, CPlayerController*> CGameInstance::mPlayerControllers;

CGameInstance::CGameInstance()
{
    mWorld = new World();
    mWorld->AddSystem<TestSystem>();
    mWorld->AddSystem<HoverSystem>();
    mWorld->AddSystem<RenderSystem>();

    EntityID entity = mWorld->AddEntity<Entity>();
    mWorld->AddComponent<TestComponent>(entity);
    mWorld->AddComponent<TransformComponent>(entity);
    mWorld->AddComponent<HoverComponent>(entity);
    SpriteComponent* s = mWorld->AddComponent<SpriteComponent>(entity, CShader::Load("Content/Shader/DefaultVS.glsl", "Content/Shader/SpritePS.glsl"));

    ResourcePtr<CTexture> texture = IResourceManager::Get<CTexture>("Content/Sprites/diff.png");
    ResourcePtr<CTexture> texture2 = IResourceManager::Get<CTexture>("Content/Sprites/norm.png");
    s->material.SetTexture(0, texture);
    s->material.SetTexture(1, texture2);

    for (int i = 0; i < 10000; i++)
    {
        EntityID entity2 = mWorld->AddEntity<Entity>();

        s = mWorld->AddComponent<SpriteComponent>(entity2, CShader::Load("Content/Shader/DefaultVS.glsl", "Content/Shader/SpritePS.glsl"));
        s->material.SetTexture(0, texture);
        s->material.SetTexture(1, texture2);

        TransformComponent* t = mWorld->AddComponent<TransformComponent>(entity2);
        t->position = TVec3(UMath::RandomRange(-5.0f, 5.0f), UMath::RandomRange(-3.0f, 3.0f), UMath::RandomRange(-1.0f, 1.0f));
        t->matrix = TMatrix::Translate(t->matrix, t->position);
        t->matrix = TMatrix::Scale(t->matrix, t->scale);

        HoverComponent* h = mWorld->AddComponent<HoverComponent>(entity2);
        h->speed = UMath::RandomRange(1.0f, 5.0f);
    }

    for (int i = 0; i < 10000; i++)
    {
        EntityID entity2 = mWorld->AddEntity<Entity>();

        ResourcePtr<CTexture> textures = IResourceManager::Get<CTexture>("Content/Sprites/Character.png");
        s = mWorld->AddComponent<SpriteComponent>(entity2, CShader::Load("Content/Shader/DefaultVS.glsl", "Content/Shader/SimpleSpritePS.glsl"));
        s->material.SetTexture(0, textures);

        TransformComponent* t = mWorld->AddComponent<TransformComponent>(entity2);
        t->position = TVec3(UMath::RandomRange(-5.0f, 5.0f), UMath::RandomRange(-3.0f, 3.0f), UMath::RandomRange(-1.0f, 1.0f));
        t->matrix = TMatrix::Translate(t->matrix, t->position);
        t->matrix = TMatrix::Scale(t->matrix, t->scale);

        HoverComponent* h = mWorld->AddComponent<HoverComponent>(entity2);
        h->speed = UMath::RandomRange(1.0f, 5.0f);
    }
}

CGameInstance::~CGameInstance()
{
	delete mLoadedLevel;

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
	if (mLoadedLevel == nullptr)
	{
        B2D_CORE_ERROR("No level loaded");
        return;
	}

    mLoadedLevel->Tick(deltaTime);
    mWorld->Update(deltaTime);
}

void CGameInstance::Draw(CViewport const* const viewport, CRenderer* graphics)
{
    if (mLoadedLevel == nullptr)
    {
        return;
    }

	mLoadedLevel->Draw(viewport, graphics);
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
