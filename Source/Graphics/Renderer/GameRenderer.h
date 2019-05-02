#pragma once

class WorldRenderer;

class GameRenderer // : IRenderer
{
public:
    GameRenderer();
    ~GameRenderer();

    WorldRenderer* GetWorldRenderer() const { return mWorldRenderer; }

private:
    WorldRenderer* mWorldRenderer = nullptr;
    //UiRenderer* mUiRenderer = nullptr;
};

