#include "B2D_pch.h"
#include "Renderer.h"

#include "Viewport.h"
#include "GameEngine.h"
#include "Platform/GenericWindow.h"
#include "Shader.h"
#include "RenderObject.h"
#include "Material.h"
#include "Texture.h"

#include <GL/glew.h>
#include <iostream>

CRenderer::CRenderer()
{
	glewInit();

	glEnable(GL_DEBUG_OUTPUT);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);

	glDebugMessageCallback([](GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam)
	{
		switch (severity)
		{
		case GL_DEBUG_SEVERITY_NOTIFICATION:
			B2D_CORE_INFO(message);
			break;
		case GL_DEBUG_SEVERITY_LOW:
			B2D_CORE_INFO(message);
			break;
		case GL_DEBUG_SEVERITY_MEDIUM:
			B2D_CORE_WARNING(message);
			break;
		case GL_DEBUG_SEVERITY_HIGH:
			B2D_CORE_ERROR(message);
			break;
		default:
			B2D_CORE_WARNING(message);
			break;
		}
	}, nullptr);
    
    B2D_CORE_INFO("Initialize OpenGL...");
    B2D_CORE_INFO("GL Version      {}", glGetString(GL_VERSION));
    B2D_CORE_INFO("GL Vendor       {}", glGetString(GL_VENDOR));
    B2D_CORE_INFO("GL Renderer     {}\n", glGetString(GL_RENDERER));

	glEnable(GL_TEXTURE_2D);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_GREATER, 0.4f);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	const float vertices[] = {
		-0.5f, -0.5f,	0.0f, 0.0f,
		 0.5f, -0.5f,	1.0f, 0.0f,
		-0.5f,  0.5f,	0.0f, 1.0f,
		 0.5f,  0.5f,	1.0f, 1.0f,
	};

	GLuint VBO;
	glGenBuffers(1, &VBO);
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

void CRenderer::PreRender()
{
    Clear();
}

void CRenderer::Draw(RenderObjectBuffer const& buffer, CViewport const* const viewport, CameraEntity const* const camera) // Camera/viewProjectionMatrix, Viewport, list of stuff to Render
{
    // flag: solid, unlit, wireframe,...

    TMatrix viewProjectionMatrix;
    if (!viewport->GetViewProjectionMatrix(viewProjectionMatrix))
    {
        return;
    }

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
            glBindTexture(GL_TEXTURE_2D, ro.mMaterial->mTextures[i]->mHandle);

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
}

void CRenderer::PostRender()
{
    CGameEngine::Instance()->GetMainWindow()->Swap();
}

void CRenderer::Clear()
{
	glClearColor(0.7f, 0, 0.7f, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
