#include "Graphics.h"

#include "Camera.h"
#include "Component/ComponentCollider.h"
#include "Component/IComponentRenderer.h"
#include "GameObject.h"
#include "Shader.h"
#include "Viewport.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>

CGraphics::CGraphics()
{
	mWindow = glfwCreateWindow(1280, 720, "Game", nullptr, nullptr);
	glfwSetWindowUserPointer(mWindow, this);
	glfwMakeContextCurrent(mWindow);
	glewInit();

	glEnable(GL_DEBUG_OUTPUT);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	glDebugMessageCallback([](GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam)
	{
		std::cerr << message << std::endl;
	}, nullptr);

	printf("Version:  %s\n", glGetString(GL_VERSION));
	printf("Vendor:	  %s\n", glGetString(GL_VENDOR));
	printf("Renderer: %s\n\n", glGetString(GL_RENDERER));

	glfwSetFramebufferSizeCallback(mWindow, [](GLFWwindow* window, int width, int height) {
		CGraphics* pGraphics = (CGraphics*)glfwGetWindowUserPointer(window);
		pGraphics->OnFramebufferSizeChanged(width, height);
	});

	glfwSwapInterval(1);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_GREATER, 0.4f);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	mViewport = new CViewport(0, 0, 1280, 720);

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

CGraphics::~CGraphics()
{
	CShader::UnloadAll();

	glfwDestroyWindow(mWindow);

	delete mViewport;
}

void CGraphics::Draw(const CGameObject* const gameObject)
{
	mViewport->Use();
	CCamera* const pCamera = mViewport->GetCamera();
	if (pCamera == nullptr)
	{
		glClearColor(0, 0, 0, 1);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		return;
	}

	for (auto component : gameObject->GetComponents())
	{
		auto componentRenderer = dynamic_cast<IComponentRenderer*>(component);
		if (componentRenderer != nullptr)
			componentRenderer->Draw(this);
	}

	for (auto component : gameObject->GetComponents())
	{
		auto componentCollider = dynamic_cast<CComponentCollider*>(component);
		if (componentCollider != nullptr)
			componentCollider->DrawShape(this);
	}

	if (gameObject->GetComponents().size() == 1)
		return;

	TMatrix model;
	//model = TMatrix::Rotate(model, static_cast<float>(glfwGetTime()) * glm::radians(50.0f), TVec3(0.5f, 1.0f, 0.0f));
	model = TMatrix::Scale(model, TVec3(100, 100, 100));

	if (glfwGetKey(mWindow, GLFW_KEY_F3) == GLFW_PRESS)
		pCamera->SetProjection(CCamera::EProjection::Perspective);
	if (glfwGetKey(mWindow, GLFW_KEY_F4) == GLFW_PRESS)
		pCamera->SetProjection(CCamera::EProjection::Orthographic);

	if (glfwGetKey(mWindow, GLFW_KEY_F1) == GLFW_PRESS)
	{
		CShader* const wireframeShader = CShader::Load("Content/Shader/SpriteVS.glsl", "Content/Shader/FillFS.glsl");
		wireframeShader->Use();
		wireframeShader->SetMatrix("model", model.GetPtr());
		wireframeShader->SetMatrix("view", pCamera->GetViewMatrix().GetPtr());
		wireframeShader->SetMatrix("projection", pCamera->GetProjectionMatrix().GetPtr());

		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

		delete wireframeShader;
	}

	CShader* const currentShader = CShader::Load("Content/Shader/SpriteVS.glsl", "Content/Shader/SpriteFS.glsl");
	currentShader->Use();
	currentShader->SetMatrix("model", model.GetPtr());
	currentShader->SetMatrix("view", pCamera->GetViewMatrix().GetPtr());
	currentShader->SetMatrix("projection", pCamera->GetProjectionMatrix().GetPtr());
	static float f = 0.0f;
	f += 0.016f;
	currentShader->SetFloat("rotation", f);
	currentShader->SetInt("ourTexture", 0);
	currentShader->SetInt("ourTextures", 1);
	
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	model = TMatrix::Translate(TMatrix(), TVec3(5, 0, 100));
	model = TMatrix::Scale(model, TVec3(100, 100, 100));
	currentShader->SetMatrix("model", model.GetPtr());
	currentShader->SetMatrix("view", pCamera->GetViewMatrix().GetPtr());
	currentShader->SetMatrix("projection", pCamera->GetProjectionMatrix().GetPtr());

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	//delete currentShader;
}

void CGraphics::Draw(const CShader* const shader)
{
	shader->Use();
}

void CGraphics::Swap()
{
	glfwSwapBuffers(mWindow);
}

void CGraphics::Clear()
{
	glClearColor(0.7f, 0, 0.7f, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void CGraphics::OnFramebufferSizeChanged(int width, int height)
{
	mViewport->SetSize(width, height);
}