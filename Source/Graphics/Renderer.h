#pragma once

class CameraEntity;
class CViewport;
class RenderObject;
class RenderObjectBuffer;

class CRenderer
{
public:
	CRenderer();
	~CRenderer();
	
    void PreRender();
    void Draw(RenderObjectBuffer const& buffer, CViewport const* const viewport, CameraEntity const* const camera);
    void PostRender();

private:
	void Clear();

};
