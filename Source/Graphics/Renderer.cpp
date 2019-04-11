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
#include "GHI/GraphicsHardwareInterface.h"

#include <GL/glew.h>
#include <iostream>

GLuint fb = 0;
GLuint renderedTexture;
GLuint VAO;
GLuint VBO;

CRenderer::CRenderer(IGraphicsHardwareInterface* ghi)
    : mGHI(ghi)
{
	const float vertices[] = {
		-0.5f, -0.5f,	0.0f, 0.0f,
		 0.5f, -0.5f,	1.0f, 0.0f,
		-0.5f,  0.5f,	0.0f, 1.0f,
		 0.5f,  0.5f,	1.0f, 1.0f,
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

    //

    glGenFramebuffers(1, &fb);
    glBindFramebuffer(GL_FRAMEBUFFER, fb);

    glGenTextures(1, &renderedTexture);
    glBindTexture(GL_TEXTURE_2D, renderedTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1920, 1080, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    // Set "renderedTexture" as our colour attachement #0
    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, renderedTexture, 0);

    // Set the list of draw buffers.
    GLenum DrawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
    glDrawBuffers(1, DrawBuffers); // "1" is the size of DrawBuffers

    B2D_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
}

CRenderer::~CRenderer()
{
	
}

void CRenderer::BeginRender()
{
    mGHI->Clear();
}

void CRenderer::Draw(RenderObjectBuffer const& buffer, CViewport const* const viewport, CameraEntity const* const camera) // Camera/viewProjectionMatrix, Viewport, list of stuff to Render
{
    // flag: solid, unlit, wireframe,...

    TMatrix viewProjectionMatrix;
    if (!viewport->GetViewProjectionMatrix(viewProjectionMatrix))
    {
        return;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, fb);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // we're not using the stencil buffer now
    glEnable(GL_DEPTH_TEST);
    glViewport(0, 0, 1920, 1080);

    for (uint32 i = 0; i < buffer.Size() * 0.01f; ++i)
    {
        RenderObject const& ro = buffer[i];
        CShader const* const shader = ro.mMaterial->mShader;
        shader->Use();

        for (uint32 i = 0; i < ro.mMaterial->mTextures.size(); ++i)
        {
            if (ro.mMaterial->mTextures[i] == nullptr)
            {
                glActiveTexture(GL_TEXTURE0 + i);
                glBindTexture(GL_TEXTURE_2D, 0);
                continue;
            }

            glActiveTexture(GL_TEXTURE0 + i);
            OpenGLTexture const* tex = static_cast<OpenGLTexture const*>(ro.mMaterial->mTextures[i]->GetGHITexture());
            glBindTexture(GL_TEXTURE_2D, tex->GetHandle());

            switch (i)
            {
                case 0: shader->SetInt("texture0", i); break;
                case 1: shader->SetInt("texture1", i); break;
                case 2: shader->SetInt("texture2", i); break;
                case 3: shader->SetInt("texture3", i); break;
                default: B2D_TRAP("Not enough texture slots ({})!", i); break;
            }
        }

        shader->SetMatrix("viewprojection", viewProjectionMatrix.GetPtr());
        shader->SetMatrix("model", ro.mMatrix.GetPtr());

        // 		static float f = 0.0f;
        // 		f += 0.016f;
        // 		currentShader->SetFloat("rotation", f);

        glPolygonMode(GL_FRONT, GL_FILL);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    }

    CShader* sss = CShader::Load("Content/Shader/RenderTargetVS.glsl", "Content/Shader/PostProcessPS.glsl");
    sss->Use();
    sss->SetInt("texture0", renderedTexture);
      
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClearColor(0.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
     
    glBindVertexArray(VAO);
    glActiveTexture(GL_TEXTURE0 + 1);
    glBindTexture(GL_TEXTURE_2D, renderedTexture);
    glViewport(0, 0, 1920, 1080);
// 
    //glDrawArrays(GL_TRIANGLES, 0, 3);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

void CRenderer::EndRender()
{
    CGameEngine::Instance()->GetMainWindow()->Swap();
}
