#include "Input.h"
#include "Debug.h"
#include "GameEngine.h"
#include "PlayerController.h"
#include "GameInstance.h"
#include "Shader.h"
#include <GLFW\glfw3.h>

CInput::CInput()
{
	glfwSetKeyCallback(CGameEngine::Instance()->GetWindow(), CInput::OnKey);
}

CInput::~CInput()
{

}

void CInput::OnKey(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		CGameEngine::Instance()->Shutdown();

	if (key == GLFW_KEY_F5 && action == GLFW_PRESS)
		CShader::ReloadAll();

	auto aPlayerControllers = CGameEngine::Instance()->GetGameInstance()->GetPlayerControllers();
	for (const auto& pPlayerController : *aPlayerControllers)
	{
		pPlayerController.second->ProcessInputKey(key, (EKeyEvent)action);
	}
}
