#pragma once
#include "Engine/EngineModule.h"

class IEditorView;

class EditorModule final : public IEngineModule
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
        mEditorViews.emplace_back(editorView);
        return editorView;
    }

    void CloseEditorView(IEditorView*& editorView);

private:
    std::vector<IEditorView*> mEditorViews;

};
