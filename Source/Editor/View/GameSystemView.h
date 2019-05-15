#pragma once
#include "IEditorView.h"

class GameSystemView : public IEditorView
{
protected:
    virtual void Tick(float deltaTime) override;

private:
    static void PrintMask(uint16 mask);
};

