#pragma once

class CameraEntity;
class CViewport;
class GHIRenderTarget;
class IGraphicsHardwareInterface;
class RenderObject;
class RenderObjectBuffer;

class CRenderer
{
public:
	CRenderer(IGraphicsHardwareInterface* ghi);
	~CRenderer();
	
    void BeginRender();
    void DrawSceneToRenderTarget(GHIRenderTarget* renderTarget, RenderObjectBuffer const& buffer, CViewport const* const viewport, CameraEntity const* const camera);
    void EndRender();

private:
    IGraphicsHardwareInterface* mGHI = nullptr;
};
