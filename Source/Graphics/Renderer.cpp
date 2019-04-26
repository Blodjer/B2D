#include "B2D_pch.h"
#include "Renderer.h"

#include "Viewport.h"
#include "GameEngine.h"
#include "Platform/GenericWindow.h"
#include "Shader.h"
#include "RenderObject.h"
#include "Material.h"
#include "Texture.h"
#include "OpenGL/OpenGLTexture.h"
#include "Graphics/GHI/GraphicsHardwareInterface.h"
#include "Graphics/GHI/GHIRenderTarget.h"
#include "Graphics/OpenGL/OpenGLMaterial.h"

#include <GL/glew.h>
#include <iostream>
#include "Input/Input.h"

GLuint VAO;
GLuint VBO;

CRenderer::CRenderer(IGraphicsHardwareInterface* ghi)
    : mGHI(ghi)
{
	const float vertices[] = {
        -1.0f, -1.0f,	0.0f, 0.0f,
         1.0f, -1.0f,	1.0f, 0.0f,
        -1.0f,  1.0f,	0.0f, 1.0f,
         1.0f,  1.0f,	1.0f, 1.0f,
	};

	GLuint VBO;
	glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 4, 0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 4, (void*)(sizeof(float) * 2));
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
}

CRenderer::~CRenderer()
{
	
}

void CRenderer::BeginRender()
{
    mGHI->Clear(true, true, true);
}

void CRenderer::DrawSceneToRenderTarget(GHIRenderTarget* renderTarget, RenderObjectBuffer const& buffer, CViewport const* const viewport, CameraEntity const* const camera)
{
    // flag: solid, unlit, wireframe,...

    if (B2D_CHECKf(viewport == nullptr, "Unable to render because the world has no active viewport"))
    {
        return;
    }

    TMatrix viewProjectionMatrix;
    if (!viewport->GetViewProjectionMatrix(viewProjectionMatrix))
    {
        return;
    }

    mGHI->BindRenderTargetAndClear(renderTarget);

    for (uint32 i = 0; i < buffer.Size(); ++i)
    {
        RenderObject const& ro = buffer[i];

        Material const* const material = ro.mMaterial;
        mGHI->BindMaterial(material->GetGHIMaterial());

        for (uint32 i = 0; i < ro.mMaterial->GetTextures().size(); ++i)
        {
            TextureRef const textureResource = ro.mMaterial->GetTextures()[i];

            glActiveTexture(GL_TEXTURE0 + i);

            if (!textureResource.IsValid())
            {
                glBindTexture(GL_TEXTURE_2D, 0);
                continue;
            }

            mGHI->BindTexture(textureResource->GetGHITexture());
        }

        OpenGLMaterial* mat = static_cast<OpenGLMaterial*>(material->GetGHIMaterial());

        GLuint ul1 = glGetUniformLocation(mat->GetHandle(), "viewprojection");
        glUniformMatrix4fv(ul1, 1, GL_FALSE, viewProjectionMatrix.GetPtr());

        GLuint ul2 = glGetUniformLocation(mat->GetHandle(), "model");
        glUniformMatrix4fv(ul2, 1, GL_FALSE, ro.mMatrix.GetPtr());

        glPolygonMode(GL_FRONT, GL_FILL);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    }

    static VertexShaderRef rtVS = IResourceManager::Get<VertexShader>("Content/Shader/RenderTargetVS.glsl");
    static PixelShaderRef ppPS = IResourceManager::Get<PixelShader>("Content/Shader/PostProcessPS.glsl");
    static Material* rtMat = new Material(rtVS, ppPS);

    mGHI->BindMaterial(rtMat->GetGHIMaterial());
    
    glActiveTexture(GL_TEXTURE0 + 1);
    //mGHI->BindTexture(renderTarget->GetTexture());
    OpenGLTexture const* tex = static_cast<OpenGLTexture const*>(renderTarget->GetTexture());
    glBindTexture(GL_TEXTURE_2D, tex->GetHandle());

    GLuint ul3 = glGetUniformLocation(static_cast<OpenGLMaterial*>(rtMat->GetGHIMaterial())->GetHandle(), "texture0");
    glUniform1i(ul3, tex->GetHandle());

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glEnable(GL_DEPTH_TEST);
    //mGHI->Clear(true, false, false);

    glBindVertexArray(VAO);

    glViewport(0, 0, viewport->GetWidth(), viewport->GetHeight());
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

//     uint32 w = viewport->GetWidth() * 0.5f;
//     uint32 h = viewport->GetHeight() * 0.5f;
// 
//     glViewport(0, 0, w, h);
//     glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
// 
//     glViewport(w, 0, w, h);
//     glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
// 
//     glViewport(0, h, w, h);
//     glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
// 
//     glViewport(w, h, w, h);
//     glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

void CRenderer::EndRender()
{
    CGameEngine::Instance()->GetMainWindow()->Swap();
}
