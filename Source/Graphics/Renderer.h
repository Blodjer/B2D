#pragma once

class CameraEntity;
class CViewport;
class WorldRenderDataInterface;
class GHIMaterial;
class GHIRenderTarget;

class CRenderer
{
public:
    static void RenderWorldFromViewportToRenderTarget(GHIRenderTarget* const target, WorldRenderDataInterface* wrdi, CViewport const* const viewport, CameraEntity const* const camera);
    static void PostProcessPass(GHIRenderTarget* source, GHIRenderTarget* target, GHIMaterial* material);
};
