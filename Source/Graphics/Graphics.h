#pragma once

#include <string>
#include <vector>

struct GLFWwindow;
class CGameObject;
class CShader;
class CViewport;

class CGraphics
{
public:
	CGraphics();
	~CGraphics();
	
	void Draw(const CGameObject* const gameObject);
	void Draw(const CShader* const shader);
	void Swap();
	void Clear();

	GLFWwindow* GetWindow() const { return mWindow; }
	CViewport* GetViewport() const { return mViewport; };

public:
	void OnFramebufferSizeChanged(int width, int height);

private:
	GLFWwindow* mWindow;
	CViewport* mViewport;

};

