#include "B2D_pch.h"
#include "Renderer.h"

#include "Camera.h"
#include "Component/ComponentCollider.h"
#include "Component/IComponentRenderer.h"
#include "GameObject.h"
#include "Material.h"
#include "Shader.h"
#include "Texture.h"
#include "Viewport.h"

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

	B2D_CORE_INFO("Version:  {0}", glGetString(GL_VERSION));
	B2D_CORE_INFO("Vendor:   {0}", glGetString(GL_VENDOR));
	B2D_CORE_INFO("Renderer: {0}\n", glGetString(GL_RENDERER));

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

void CRenderer::Draw(CViewport const* const viewport, std::vector<CGameObject*> const& gameObjects)
{
	CCamera const* const pCamera = viewport->GetCamera();
	if (pCamera == nullptr)
	{
		return;
	}

	std::vector<IComponentRenderer const*> renderQueue;
	for (CGameObject const* const gameObject : gameObjects)
	{
		if (gameObject->GetComponents().empty())
			continue;

		for (auto component : gameObject->GetComponents())
		{
			auto componentRenderer = dynamic_cast<IComponentRenderer*>(component);
			if (componentRenderer != nullptr && componentRenderer->GetMaterial() != nullptr)
			{
				renderQueue.push_back(componentRenderer);
			}
		}
	}

	for (IComponentRenderer const* const component : renderQueue)
	{
		CMaterial const* const material = component->GetMaterial();
		CShader* const currentShader = material->mShader;
		currentShader->Use();

		for (uint16 i = 0; i < material->mTextures.size(); ++i)
		{
			if (material->mTextures[i] == nullptr)
			{
				glActiveTexture(GL_TEXTURE0 + i);
				glBindTexture(GL_TEXTURE_2D, 0);
				continue;
			}

			glActiveTexture(GL_TEXTURE0 + i);
			glBindTexture(GL_TEXTURE_2D, material->mTextures[i]->mHandle);

			std::string textureUniform = "texture";
			textureUniform.append(std::to_string(i));

			currentShader->SetInt(textureUniform, i);
		}

		TMatrix model(1.0f);
		model = TMatrix::Translate(model, TVec3(component->GetRelativePosition().X, component->GetRelativePosition().Y, component->GetRelativePosition().Y));
		model = TMatrix::Scale(model, TVec3(140, 140, 140));

		currentShader->SetMatrix("model", model.GetPtr());
		currentShader->SetMatrix("view", pCamera->GetViewMatrix().GetPtr());
		currentShader->SetMatrix("projection", pCamera->GetProjectionMatrix().GetPtr());

// 		static float f = 0.0f;
// 		f += 0.016f;
// 		currentShader->SetFloat("rotation", f);

		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

		if (false)
		{
			model = TMatrix::Translate(model, TVec3(0, 0, -0.001f));
			
			CShader* const wireframeShader = CShader::Load("Content/Shader/SimpleSpriteVS.glsl", "Content/Shader/FillPS.glsl");
			wireframeShader->Use();
			wireframeShader->SetMatrix("model", model.GetPtr());
			wireframeShader->SetMatrix("view", pCamera->GetViewMatrix().GetPtr());
			wireframeShader->SetMatrix("projection", pCamera->GetProjectionMatrix().GetPtr());
		
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		}
	}
}

void CRenderer::Clear()
{
	glClearColor(0.7f, 0, 0.7f, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
