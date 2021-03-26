#pragma once
#include "Editor/View/IEditorView.h"

class WorldRenderer;
class CViewport;

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

    CViewport* m_viewport = nullptr;
    std::string m_viewportName;
};

