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
static std::vector<RenderObject> rob3;

static CViewport const* v2;
static CameraEntity const* c2;

bool sw = true;
bool finished = true;

void RENDER()
{
    CGameEngine::Instance()->GetMainWindow()->MakeContextCurrent();

    bool mysw = true;
    while (true)
    {
        while (mysw == sw)
        {
            Sleep(0);
        }

        finished = false;

        std::vector<RenderObject> const& robb = sw ? rob2 : rob3;

        CGameEngine::Instance()->GetGraphicsInstance()->PreRender();
        CGameEngine::Instance()->GetGraphicsInstance()->Draw(robb, v2, c2);

        mysw = sw;
        finished = true;
    }
}

void RenderSystem::Update(float deltaTime)
{
    if (!finished)
    {
        return;
    }

    v2 = mWorld->GetOwningGameInstance()->GetWindow()->GetViewport();
    if (B2D_CHECKf(v2 == nullptr, "Unable to render because the world has no active viewport"))
    {
        return;
    }

    c2 = v2->GetCamera();

    std::vector<RenderObject>& robb = !sw ? rob2 : rob3;

    robb.clear();
    robb.reserve(mWorld->GetComponents<SpriteComponent>().size());
        
    for (SpriteComponent const& spriteComponent : ComponentItr<SpriteComponent, TransformComponent>(mWorld))
    {
        TMatrix const& model = spriteComponent.Sibling<TransformComponent>().matrix;
        CMaterial const& material = spriteComponent.material;

        robb.emplace_back(model, material);
    }

    if (finished)
    {
        CGameEngine::Instance()->GetGraphicsInstance()->PostRender();
        sw = !sw;
    }

    static bool bb = true;
    if (bb)
    {
        glfwMakeContextCurrent(nullptr);
        bb = false;
        std::thread ren(RENDER);
        ren.detach();
    }
}
