#include "B2D_pch.h"
#include "RenderSystem.h"

#include "GameEngine.h"
#include "Core/Types/Matrix.h"
#include "Game/Component/SpriteComponent.h"
#include "Game/Component/TransformComponent.h"
#include "Game/Core/World.h"
#include "Game/GameInstance.h"
#include "Graphics/RenderObject.h"
#include "Graphics/Viewport.h"
#include "Platform/GenericWindow.h"

// Each World has it's own world renderer
// The world renderer render to a game viewport client (which is a rendertarget and a collection viewports base on the splitscreen configration)
// 
// The RenderSystem updates the RBO of the world renderer
// OR GAME RENDERER

// Should be able to switch from Editor to InGameEditor to Game (?)

    // Editor
        // Init
            /// AddView<>()
            /// AddView<>()

    // Game

    // IRenderManger
        // case Editor: non threaded world renderer
        // case InGameEditor: threaded game renderer
        // case Game: threaded game renderer

// EditorInstance

// WorldEditorView
    // RenderTarget*[] mRenderTarget;
    // World* world;
    // Tick()
        /// world->Render(mRenderTarget)

// World
    // Init(GameRenderer* const) [Player]
    // Init(nullptr) [Server]

// EditorRenderManager (nothing should be rendered outside of an EditorView)
    // vector<EditorView>
    // Render()
        /// RenderAllEditorViews()
            /// view->Render()
                /// ImGui::BeginWindow()
                /// ImGui::EndWindow()

// EditorRenderer : IRenderer
    // vector<IRenderManager>
    // Window? ownedWindow
    // Init(bool asThread, Window? window)
    // Render()
        /// if (asThread) return
    // RenderInternal()
        /// foreach (IRenderManager)
            /// IRenderManager->Render()
            /// DrawFPToFP(IRenderManager->renderTarget, mGameRenderTarget)
        /// ImGUI::DrawTexture(mGameRenderTarget)
        /// ImgUI::Render()
        /// ImgUI::NewFrame()

// GameRenderer : IRenderer (one per World)
    // WorldRenderer
    // UIRenderer
    // Window? ownedWindow
    //
    // Init(bool asThread, Window? window)
    // Render()
        /// if (asThread) return
    // RenderInternal()
        /// worldRenderer->Render()
        /// uiRenderer->Render()
        /// DrawFPToFP(wr, renderTarget)
        /// DrawFPToFP(ui, renderTarget)
        /// if (window) window->Swap()

// RENDERER AND THREAD?

// Who swaps?
// Only in Release the IGameRenderer has Swap Control (SC).
// In Profile and Debug the IRenderManger has SC and therefore only swaps every CPU frame.
// Generalization: If the IRenderManger only contains a IGameRenderer then the SC can be owned by the IGameRenderer and therefore the GPU.
//                 Else the IRenderManger would own the SC itself.

// Who owns the RenderTargets?

// WorldRenderer
    // RenderObjectBuffer
        // uint32 mPreparedFrame
    
    // uint32 mRenderedFrame
    // Render()
        //

// IRenderer
//      void Render(FB* target)

// MainThread
    // Editor
        // vector<IEditorView>
        // Tick()
            /// 

// RenderManager
    // vector<IRenderer>
    // T CreateRenderer<T>()
    // DeleteRenderer(IRenderer)
    

// IRenderer
    // FP renderTarget
    // mutex renderTargetMutex


// RenderThread
    // RenderManager
        // vector<IRenderer>
        // Render()
            /// 

// RenderManger
//      GameRenderManager (Thread) : IRenderer
//           Render()
//              foreach (IGameRender r : mGameRenderers)
//                  r->Render()
//              GetFramebuffersAndRenderTo mGameRenderTarget
//           FB mGameRenderTarget
//           Mutex mMutex
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

// WorldRenderer
//      RBO
//      Flags (wireframe, solid,...)
//      virtual Draw(RT* rt) override

// ResourceEditorView (Renderer to render as specific visual resource)
//  virtual Draw() = 0
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

/*
IViewport
    Tick(float deltaTime)

GameViewport : IViewport


EditorViewport : IViewport

*/
void RenderSystem::Update(float deltaTime)
{
    WorldRenderDataInterface* wrdi = mWorld->GetWorldRenderDataInterface();
    if (wrdi == nullptr)
    {
        return;
    }

    wrdi->ClearAndSetRenderData([&](RenderObjectBuffer<QuadRenderObject>& buffer) {

        for (SpriteComponent const& spriteComponent : ComponentItr<SpriteComponent, TransformComponent>(mWorld))
        {
            buffer.Add(
                spriteComponent.Sibling<TransformComponent>().matrix,
                spriteComponent.material
            );
        }
    });
}
