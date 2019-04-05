#include "B2D_pch.h"
#include "RenderSystem.h"

#include "GameEngine.h"
#include "Core/Types/Matrix.h"
#include "Game/GameInstance.h"
#include "Game/Component/SpriteComponent.h"
#include "Game/Component/TransformComponent.h"
#include "Graphics/Renderer.h"
#include "Graphics/RenderObject.h"
#include "Graphics/Shader.h"
#include "Graphics/Viewport.h"
#include "Platform/GenericWindow.h"
#include <thread>
#include "Platform/Desktop/DesktopWindow.h"

#include <GLFW/glfw3.h>

static std::vector<RenderObject> rob2;

static CViewport const* v2;
static CameraEntity const* c2;

int copiedFrame = 0;
int preparedFrame = -1;
int renderedFrame = -1;
bool finished = true;

std::mutex mtx;

void RENDER()
{
    CGameEngine::Instance()->GetMainWindow()->MakeContextCurrent();

    while (true)
    {
        while (preparedFrame == renderedFrame)
        {
            Sleep(0);
        }

        mtx.lock();

        std::vector<RenderObject> const& robb = rob2;

        CGameEngine::Instance()->GetGraphicsInstance()->PreRender();
        CGameEngine::Instance()->GetGraphicsInstance()->Draw(robb, v2, c2);
        CGameEngine::Instance()->GetGraphicsInstance()->PostRender();

        renderedFrame = preparedFrame;
        mtx.unlock();
        Sleep(16);
    }
}

void RenderSystem::Update(float deltaTime)
{
    bool l = mtx.try_lock();
    if (!l || preparedFrame != renderedFrame)
    {
        if (l)
            mtx.unlock();

        return;
    }

    v2 = mWorld->GetOwningGameInstance()->GetWindow()->GetViewport();
    if (B2D_CHECKf(v2 == nullptr, "Unable to render because the world has no active viewport"))
    {
        return;
    }

    c2 = v2->GetCamera();

    std::vector<RenderObject>& robb = rob2;
    
    robb.clear();
    robb.reserve(mWorld->GetComponents<SpriteComponent>().size());

    int i = 0;
    for (SpriteComponent const& spriteComponent : ComponentItr<SpriteComponent, TransformComponent>(mWorld))
    {
        TMatrix const& model = spriteComponent.Sibling<TransformComponent>().matrix;
        CMaterial const& material = spriteComponent.material;

        robb.emplace_back(model, material);
    }
    copiedFrame++;

    preparedFrame = copiedFrame;
    
    mtx.unlock();

    static bool bb = true;
    if (bb)
    {
        glfwMakeContextCurrent(nullptr);
        bb = false;
        std::thread ren(RENDER);
        ren.detach();
    }
}
