#include "B2D_pch.h"
#include "GHIRenderTarget.h"
#include "GHITexture.h"

uint32 GHIRenderTarget::GetWidth() const
{
    return GetTexture()->GetWidth();
}

uint32 GHIRenderTarget::GetHeight() const
{
    return GetTexture()->GetHeight();
}
