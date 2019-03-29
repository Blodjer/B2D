#include "B2D_pch.h"
#include "RenderSystem.h"

#include "GL/glew.h"
#include "ECS/Component/SpriteComponent.h"
#include "ECS/Component/TransformComponent.h"
#include "Graphics/Viewport.h"
#include "Core/Types/Matrix.h"
#include "Core/GameEngine.h"
#include "Graphics/Renderer.h"
#include "Graphics/Window.h"
#include "Graphics/Shader.h"
#include "Core/GameInstance.h"

void RenderSystem::Update(float deltaTime)
{
    CViewport const* const viewport = mWorld->GetOwningGameInstance()->GetWindow()->GetViewport();
    if (B2D_CHECKf(viewport == nullptr, "Unable to render because the world has no active viewport"))
    {
        return;
    }

    CGameEngine::Instance()->GetGraphicsInstance()->PreRender();

    TMatrix viewProjectionMatrix;
    if (!viewport->GetViewProjectionMatrix(viewProjectionMatrix))
    {
        CGameEngine::Instance()->GetGraphicsInstance()->PostRender();
        return;
    }

    for (SpriteComponent const& spriteComponent : ComponentItr<SpriteComponent, TransformComponent>(mWorld))
    {
        CMaterial const& material = spriteComponent.material;
        CShader* const currentShader = material.mShader;
        currentShader->Use();

        for (uint32 i = 0; i < material.mTextures.size(); ++i)
        {
            if (material.mTextures[i] == nullptr)
            {
                glActiveTexture(GL_TEXTURE0 + i);
                glBindTexture(GL_TEXTURE_2D, 0);
                continue;
            }

            glActiveTexture(GL_TEXTURE0 + i);
            glBindTexture(GL_TEXTURE_2D, material.mTextures[i]->mHandle);

            switch (i)
            {
                case 0: currentShader->SetInt("texture0", i); break;
                case 1: currentShader->SetInt("texture1", i); break;
                case 2: currentShader->SetInt("texture2", i); break;
                case 3: currentShader->SetInt("texture3", i); break;
                default: B2D_TRAP("Not enough texture slots ({})!", i); break;
            }
        }

        TMatrix const& model = spriteComponent.Sibling<TransformComponent>().matrix;

        currentShader->SetMatrix("viewprojection", viewProjectionMatrix.GetPtr());
        currentShader->SetMatrix("model", model.GetPtr());

        // 		static float f = 0.0f;
        // 		f += 0.016f;
        // 		currentShader->SetFloat("rotation", f);

        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    }

    CGameEngine::Instance()->GetGraphicsInstance()->PostRender();
}
