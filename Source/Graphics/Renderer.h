#pragma once

#include <vector>

class CGameObject;
class CViewport;

class CRenderer
{
public:
	CRenderer();
	~CRenderer();
	
	void Draw(CViewport const* const viewport, std::vector<CGameObject*> const& gameObjects);
	void Clear();

};

struct RenderObject
{
	void* mMaterial;
	void* mMesh;
};