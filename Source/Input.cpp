#include "Input.h"

#include "Core/GameEngine.h"
#include "Core/GameInstance.h"
#include "Graphics/Shader.h"
#include "Graphics/Window.h"
#include "PlayerController.h"

#include <GLFW/glfw3.h>

CInput::CInput()
{
	glfwSetKeyCallback(CGameEngine::Instance()->GetWindow()->GetWindow(), CInput::OnKey);
}

CInput::~CInput()
{

}

void CInput::OnKey(GLFWwindow* window, int glfwKey, int scancode, int glfwAction, int glfwMods)
{
	EKey const key = static_cast<EKey>(glfwKey);
	EKeyEvent const keyEvent = static_cast<EKeyEvent>(glfwAction);

	if (key == EKey::ESCAPE && keyEvent == EKeyEvent::KEY_DOWN)
		CGameEngine::Instance()->Shutdown();

	if (key == EKey::F5 && keyEvent == EKeyEvent::KEY_DOWN)
		CShader::ReloadAll();

	auto playerControllers = CGameEngine::Instance()->GetGameInstance()->GetPlayerControllers();
	for (const auto& playerController : *playerControllers)
	{
		playerController.second->ProcessInputKey(key, keyEvent);
	}
}
