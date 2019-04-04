#pragma once

class CameraEntity;
class CViewport;
class RenderObject;

class CRenderer
{
public:
	CRenderer();
	~CRenderer();
	
    void PreRender();
    void Draw(std::vector<RenderObject> const& scene, CViewport const* const viewport, CameraEntity const* const camera);
    void PostRender();

private:
	void Clear();

};
