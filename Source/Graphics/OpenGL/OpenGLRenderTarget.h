#pragma once
#include "Graphics/GHI/GHIRenderTarget.h"
#include <GL/glew.h>

class OpenGLRenderTarget : public GHIRenderTarget
{
public:
    OpenGLRenderTarget(GLuint handle, GHITexture* targetTexture);
    
    GLuint GetHandle() const { return m_handle; }

    virtual GHITexture* GetTexture() const override { return m_targetTexture; };

private:
    GLuint m_handle;
    GHITexture* m_targetTexture;

};

