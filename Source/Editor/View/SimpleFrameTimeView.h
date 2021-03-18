#pragma once
#include "Editor/View/IEditorView.h"

class SimpleFrameTimeView : public IEditorView
{
public:
    SimpleFrameTimeView();
    ~SimpleFrameTimeView();

protected:
    virtual void Tick(float deltaTime) override;

private:
    bool m_open = true;

};

