#pragma once

class CameraEntity;
class CViewport;
class WorldRenderDataInterface;

class CRenderer
{
public:
    static void DrawWorldFromViewport(WorldRenderDataInterface* wrdi, CViewport const* const viewport, CameraEntity const* const camera);
};
