#pragma once
#include "Graphics/GHI/GHIRenderTarget.h"
#include "Graphics/GHI/GHITexture.h"
#include <GL/glew.h>

class OpenGLRenderTarget : public GHIRenderTarget
{
public:
    OpenGLRenderTarget(GLuint handle, GHITexture* targetTexture, GHITexture* depthTexture);

    GLuint GetHandle() const { return m_handle; }

    GHITexture* GetTexture() const { return m_targetTexture; };
    GHITexture* GetDepthTexture() const { return m_depthTexture; };

    uint32 GetWidth() const { return m_targetTexture->GetWidth(); }
    uint32 GetHeight() const { return m_targetTexture->GetHeight(); }

private:
    GLuint m_handle;
    GHITexture* m_targetTexture;
    GHITexture* m_depthTexture;

};

