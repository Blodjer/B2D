#pragma once

#include "Editor/ImGuiCommon.h"

class IEditorView
{
    friend class EditorModule;

protected:
    IEditorView() = default;
    virtual ~IEditorView() = default;

    virtual void Tick(float deltaTime) = 0;

    void Close();
};

