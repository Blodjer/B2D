#include "B2D_pch.h"
#include "WorldRenderer.h"

#include "GameEngine.h"
#include "Game/GameInstance.h"
#include "Game/Core/World.h"
#include "Graphics/Viewport.h"
#include "Graphics/Renderer/WorldRenderDataInterface.h"
#include "Graphics/Shader.h"
#include "Graphics/RenderObject.h"
#include "Graphics/Material.h"
#include "Graphics/Texture.h"
#include "Graphics/OpenGL/OpenGLTexture.h"
#include "Graphics/OpenGL/OpenGLMaterial.h"
#include "Graphics/GHI/GraphicsHardwareInterface.h"
#include "Graphics/GHI/GHIRenderTarget.h"
#include "Platform/GenericWindow.h"

void WorldRenderer::Init()
{
    IRenderer::Init();
}

void WorldRenderer::Shutdown()
{
    IRenderer::Shutdown();
}

bool WorldRenderer::ShouldRenderNextFrame()
{
    if (!GameEngine::Instance()->GetGameInstance() || !GameEngine::Instance()->GetGameInstance()->GetWorld())
    {
        return false;
    }

    m_wrdi = GameEngine::Instance()->GetGameInstance()->GetWorld()->GetWorldRenderDataInterface();
    return m_wrdi->GetPreparedFrame() != m_renderedFrame;
}

void WorldRenderer::RenderInternal(GHIRenderTarget* const renderTarget)
{
    RenderWorldFromViewportToRenderTarget(renderTarget, m_wrdi, m_viewport);
    //PostProcessPass(renderTarget, renderTarget, nullptr);
}

void WorldRenderer::RenderWorldFromViewportToRenderTarget(GHIRenderTarget* const target, WorldRenderDataInterface const* wrdi, CViewport const* const viewport)
{
    // flag: solid, unlit, wireframe,...

    if (B2D_CHECK_f(viewport == nullptr, "Unable to render because the world has no active viewport"))
    {
        return;
    }

    TMatrix viewProjectionMatrix;
    if (!viewport->GetViewProjectionMatrix(viewProjectionMatrix))
    {
        return;
    }

    IGraphicsHardwareInterface* const ghi = GameEngine::Instance()->GetGHI();

    ghi->BindRenderTargetAndClear(target);

    wrdi->StartRead();

    RenderObjectBuffer<QuadRenderObject> const& quadRenderObjectBuffer = wrdi->GetQuadBuffer();

    Material const* lastMaterial = nullptr;
    for (uint32 i = 0; i < quadRenderObjectBuffer.Size(); ++i)
    {
        QuadRenderObject const& ro = quadRenderObjectBuffer[i];

        Material const* const material = ro.m_material;
        OpenGLMaterial const* mat = static_cast<OpenGLMaterial*>(material->GetGHIMaterial());

        if (lastMaterial != material)
        {
            ghi->BindMaterial(material->GetGHIMaterial());

            for (uint32 i = 0; i < material->GetTextures().size(); ++i)
            {
                TextureRef const textureResource = material->GetTextures()[i];

                glActiveTexture(GL_TEXTURE0 + i);

                if (!textureResource.IsValid())
                {
                    glBindTexture(GL_TEXTURE_2D, 0);
                    continue;
                }

                ghi->BindTexture(textureResource->GetGHITexture());
            }

            GLuint ul1 = glGetUniformLocation(mat->GetHandle(), "viewprojection");
            glUniformMatrix4fv(ul1, 1, GL_FALSE, viewProjectionMatrix.GetPtr());

            lastMaterial = material;
        }

        GLuint ul2 = glGetUniformLocation(mat->GetHandle(), "model");
        glUniformMatrix4fv(ul2, 1, GL_FALSE, ro.m_matrix.GetPtr());

        glPolygonMode(GL_FRONT, GL_FILL);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    }

    m_renderedFrame.store(wrdi->GetPreparedFrame());
    wrdi->StopRead();
}

// void WorldRenderer::PostProcessPass(GHIRenderTarget* source, GHIRenderTarget* target, GHIMaterial* material)
// {
//     static GLuint VAO = 0;
//     static GLuint VBO = 0;
//     if (VBO == 0)
//     {
//         float const vertices[] = {
//             -1.0f, -1.0f,	0.0f, 0.0f,
//              1.0f, -1.0f,	1.0f, 0.0f,
//             -1.0f,  1.0f,	0.0f, 1.0f,
//              1.0f,  1.0f,	1.0f, 1.0f,
//         };
// 
//         glGenBuffers(1, &VBO);
//         glBindVertexArray(VAO);
//         glBindBuffer(GL_ARRAY_BUFFER, VBO);
//         glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
// 
//         glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 4, 0);
//         glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 4, (void*)(sizeof(float) * 2));
//         glEnableVertexAttribArray(0);
//         glEnableVertexAttribArray(1);
//     }
// 
//     IGraphicsHardwareInterface* const ghi = GameEngine::Instance()->GetGHI();
// 
//     ghi->BindRenderTarget(target);
// 
//     static VertexShaderRef ppVS = IResourceManager::Get<VertexShader>("Content/Shader/RenderTargetVS.glsl");
//     static PixelShaderRef ppPS = IResourceManager::Get<PixelShader>("Content/Shader/PostProcessPS.glsl");
//     static GHIMaterial* ppMaterial = ghi->CreateMaterial(ppVS->GetGHIShader(), ppPS->GetGHIShader());
//     ghi->BindMaterial(ppMaterial);
// 
//     OpenGLTexture const* tex = static_cast<OpenGLTexture const*>(source->GetTexture());
//     glActiveTexture(GL_TEXTURE0 + 0);
//     glBindTexture(GL_TEXTURE_2D, tex->GetHandle());
// 
//     glBindVertexArray(VAO);
// 
//     glPolygonMode(GL_FRONT, GL_FILL);
//     glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
// }
