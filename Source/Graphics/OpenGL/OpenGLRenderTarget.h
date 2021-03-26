#pragma once
#include "Graphics/GHI/GHIRenderTarget.h"
#include <GL/glew.h>

class OpenGLRenderTarget : public GHIRenderTarget
{
public:
    OpenGLRenderTarget(GLuint handle, GHITexture* targetTexture, GHITexture* depthTexture);
    
    GLuint GetHandle() const { return m_handle; }

    virtual GHITexture* GetTexture() const override { return m_targetTexture; };
    GHITexture* GetDepthTexture() const { return m_depthTexture; };

private:
    GLuint m_handle;
    GHITexture* m_targetTexture;
    GHITexture* m_depthTexture;

};

