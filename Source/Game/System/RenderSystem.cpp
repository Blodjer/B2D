#include "B2D_pch.h"
#include "RenderSystem.h"

#include "GameEngine.h"
#include "Core/Types/Matrix.h"
#include "Game/Component/SpriteComponent.h"
#include "Game/Component/TransformComponent.h"
#include "Game/Core/World.h"
#include "Game/GameInstance.h"
#include "Graphics/Renderer.h"
#include "Graphics/RenderObject.h"
#include "Graphics/Renderer/WorldRenderer.h"
#include "Graphics/Viewport.h"
#include "Platform/GenericWindow.h"
#include <thread>

#include <GLFW/glfw3.h>

// Each World has it's own world renderer
// The world renderer render to a game viewport client (which is a rendertarget and a collection viewports base on the splitscreen configration)
// 
// The RenderSystem updates the RBO of the world renderer
// OR GAME RENDERER

// RENDERER AND THREAD?

// Who swaps?
// Only in Release the IGameRenderer has Swap Control (SC).
// In Profile and Debug the IRenderManger has SC and therefore only swaps every CPU frame.
// Generalization: If the IRenderManger only contains a IGameRenderer then the SC can be owned by the IGameRenderer and therefore the GPU.
//                 Else the IRenderManger would own the SC itself.

// RenderManger
//      InGameEditor : EditorRenderer (SC)
//
//      GameRenderManager (Thread)
//           FB mGameRenderTarget
//           vector<IGameRenderer> mGameRenderers
//               WorldRenderer : IGameRenderer RBO->FB
///              UiRenderer : IGameRenderer ?->FB
///              TransitionRenderer : IGameRenderer
//
//      Editor : EditorRenderer
//      ResourceViews : EditorRenderer

// WORLD RENDER THREAD
// INGAME UI RENDER THREAD

// RenderThread : Thread
//      ThreadInit()
//          MakeContextCurrentForThisThread();
//
//      Run()
//          for (IRenderer renderer : mRenderers)
//              renderer->Draw()

// RendererManager
//      vector<IRenderer> mRenderers
//      

// IRenderer
//      virtual Draw(RT* rt) = 0;
//      RenderTarget/Texture
//      GetTexture()

// WorldRenderer
//      RBO
//      Flags (wireframe, solid,...)
//      virtual Draw(RT* rt) override

// ResourceViewRenderer (Renderer to render as specific visual resource)
//  virtual Draw(IResource, RT) = 0
//      TextureRenderer
//      ShaderRenderer

// DrawGameViewportToRenderTarget(GameViewportClient* c)
    // SetRenderTarget()
        /// glBindFramebuffer()
    // Clear()
        /// glClear()
    // foreach c->Viewport
        // DrawSceneFromViewport (ROB, viewport)
            // SetViewport()
                /// glUseViewport
            // foreach RO in ROB
                // DrawRO
                    /// glActivateTexture
                    /// glUseProgram
                    /// glDrawArray
// Draw GameViewport
    // SetRenderTarget()
        /// glBindFramebuffer()
    // DrawRenderTarget()
// Swap

// GameViewport
//      Viewports[]
//      RenderTarget* (optional)

static CViewport const* viewport;
static CameraEntity const* camera;

static WorldRenderer* wr = nullptr;

// Render Game
void RENDER()
{
    CGameEngine::Instance()->GetMainWindow()->MakeContextCurrent();

    while (true)
    {
        while (wr->PendingRendering())
        {
            Sleep(0);
        }

        wr->Lock();

        CRenderer* r = CGameEngine::Instance()->GetRenderer();

        r->BeginRender();
        r->DrawSceneToRenderTarget(wr->GetRenderObjectBuffer(), viewport, camera);
        r->EndRender();

        renderedFrame = preparedFrame;
        wr->Unlock();
    }
}

void RenderSystem::Update(float deltaTime)
{
    WorldRenderer* renderer = mWorld->GetRenderer();

    if (!renderer->IsWaitingForNewData())
    {
        return;
    }

    if (!renderer->TryLock())
    {
        return;
    }

    RenderObjectBuffer& buffer = renderer->GetRenderObjectBuffer();

    viewport = mWorld->GetOwningGameInstance()->GetWindow()->GetViewport();
    if (B2D_CHECKf(viewport == nullptr, "Unable to render because the world has no active viewport"))
    {
        return;
    }

    camera = viewport->GetCamera();

    buffer.Clear();
    for (SpriteComponent const& spriteComponent : ComponentItr<SpriteComponent, TransformComponent>(mWorld))
    {
        buffer.Add(
            spriteComponent.Sibling<TransformComponent>().matrix,
            spriteComponent.material
        );
    }

    preparedFrame++;
    renderer->Unlock();

    static bool bb = true;
    if (bb)
    {
        wr = renderer;
        glfwMakeContextCurrent(nullptr);
        bb = false;
        std::thread ren(RENDER);
        ren.detach();
    }
}

bool RenderThread::Init()
{
    return true;
}

void RenderThread::Run()
{
//     CGameEngine::Instance()->GetMainWindow()->MakeContextCurrent();
// 
//     while (true)
//     {
//         while (preparedFrame == renderedFrame)
//         {
//             Sleep(0);
//         }
// 
//         mtx.lock();
// 
//         CRenderer* r = CGameEngine::Instance()->GetRenderer();
// 
//         r->BeginRender();
//         r->DrawSceneToRenderTarget(buffer, viewport, camera);
//         r->EndRender();
// 
//         renderedFrame = preparedFrame;
//         mtx.unlock();
//     }
}

void RenderThread::Stop()
{
    
}
