#pragma once
#include "Editor/View/IEditorView.h"

class WorldRenderer;

class WorldEditorView : public IEditorView
{
public:
    WorldEditorView();
    ~WorldEditorView();

protected:
    virtual void Tick(float deltaTime) override;

private:
    WorldRenderer* worldRenderer = nullptr;
    bool mOpen = true;
    std::string mViewportName;
};

