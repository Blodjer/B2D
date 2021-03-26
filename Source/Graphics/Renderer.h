#pragma once

class CameraEntity;
class CViewport;
class WorldRenderDataInterface;
class GHIMaterial;
class GHIRenderTarget;

class CRenderer
{
public:
    static void RenderWorldFromViewportToRenderTarget(GHIRenderTarget* const target, WorldRenderDataInterface const* wrdi, CViewport const* const viewport);
    static void PostProcessPass(GHIRenderTarget* source, GHIRenderTarget* target, GHIMaterial* material);
};
