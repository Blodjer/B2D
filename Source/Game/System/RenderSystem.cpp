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

static CViewport const* viewport;
static CameraEntity const* camera;

static uint32 preparedFrame = -1;
static uint32 renderedFrame = -1;

static RenderObjectBuffer buffer(100000);

static std::mutex mtx;

// Render Game
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

        CRenderer* r = CGameEngine::Instance()->GetRenderer();

        // Each World has it's own world renderer
        // The world renderer render to a game viewport client (which is a rendertarget and a collection viewports base on the splitscreen configration)
        // 
        // The RenderSystem updates the RBO of the world renderer

        // WorldRenderer
        //      RBO
        //
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

        r->BeginRender();
        r->Draw(buffer, viewport, camera);
        r->EndRender();

        renderedFrame = preparedFrame;
        mtx.unlock();
    }
}

void RenderSystem::Update(float deltaTime)
{
    if (preparedFrame != renderedFrame)
    {
        return;
    }

    if (!mtx.try_lock())
    {
        return;
    }

    viewport = mWorld->GetOwningGameInstance()->GetWindow()->GetViewport();
    if (B2D_CHECKf(viewport == nullptr, "Unable to render because the world has no active viewport"))
    {
        return;
    }

    camera = viewport->GetCamera();

    uint32 i = 0;
    for (SpriteComponent const& spriteComponent : ComponentItr<SpriteComponent, TransformComponent>(mWorld))
    {
        buffer[i].mMatrix = spriteComponent.Sibling<TransformComponent>().matrix;
        buffer[i].mMaterial = &spriteComponent.material;
        i++;
    }
    buffer.Size() = i;

    preparedFrame++;
    
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
