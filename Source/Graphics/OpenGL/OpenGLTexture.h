#pragma once

#include "Graphics/GHI/GHITexture.h"
#include <GL/glew.h>

class OpenGLTexture : public GHITexture
{
public:
    OpenGLTexture(GLuint handle, uint32 width, uint32 height);

    virtual void* GetNativePtr() const override { return reinterpret_cast<void*>(static_cast<size_t>(m_handle)); };

    FORCEINLINE GLuint GetHandle() const { return m_handle; }

private:
    GLuint m_handle = 0;

};

