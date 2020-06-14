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
    WorldRenderer* m_worldRenderer = nullptr;
    bool m_open = true;
    std::string m_viewportName;
};

