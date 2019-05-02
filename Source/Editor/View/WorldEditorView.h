#pragma once
#include "Editor/EditorViewInterface.h"

class WorldRenderer;

class WorldEditorView : public IEditorView
{
public:
    WorldEditorView();
    ~WorldEditorView();

protected:
    virtual void Tick(float deltaTime) override;

private:
    WorldRenderer* worldRenderer;
    bool mOpen;
};

