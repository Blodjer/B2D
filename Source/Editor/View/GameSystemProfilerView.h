#pragma once
#include "IEditorView.h"

class GameSystemProfilerView : public IEditorView
{
protected:
    virtual void Tick(float deltaTime) override;
};

