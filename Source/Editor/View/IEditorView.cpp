#include "B2D_pch.h"
#include "IEditorView.h"

#include "Editor/EditorModule.h"
#include "GameEngine.h"

void IEditorView::Close()
{
    GameEngine::Instance()->GetModuleManager()->Get<EditorModule>()->CloseEditorView(this);
}
