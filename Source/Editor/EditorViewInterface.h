#pragma once

#include "imgui/imgui.h"

class IEditorView
{
    friend class EditorModule;

protected:
    IEditorView() = default;
    ~IEditorView() = default;

    virtual void Tick(float deltaTime) = 0;
};

