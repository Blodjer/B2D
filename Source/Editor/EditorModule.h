#pragma once
#include "Engine/EngineModule.h"
#include "Platform/PlatformMessageHandlerInterface.h"

class IEditorView;

class EditorModule final : public IEngineModule, public IPlatformMessageHandlerInterface
{
protected:
    virtual void Init() override;
    void InitDefaultEditorViews();

    virtual void Shutdown() override;

    virtual void BeginFrame() override;
    virtual void Tick(float deltaTime) override;
    virtual void EndFrame() override;

public:
    void Draw();

public:
    template<class C>
    C* CreateEditorView()
    {
        B2D_STATIC_ASSERT_TYPE(IEditorView, C);

        C* editorView = new C();
        m_editorViews.emplace_back(editorView);
        return editorView;
    }

    void CloseEditorView(IEditorView* editorView);

private:
    void ClosePendingEditorViews();

private:
    std::vector<IEditorView*> m_editorViews;
    std::queue<IEditorView*> m_editorToRemove;

};

