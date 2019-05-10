#pragma once
#include "IEditorView.h"

class GameSystemProfilerView : public IEditorView
{
public:
    GameSystemProfilerView();
    ~GameSystemProfilerView();

protected:
    virtual void Tick(float deltaTime) override;

};

