#pragma once

class GHITexture;

class GHIRenderTarget
{
protected:
    GHIRenderTarget() = default;

private:
    GHIRenderTarget(GHIRenderTarget const&) = delete;
    GHIRenderTarget& operator=(GHIRenderTarget const&) = delete;

public:
    virtual GHITexture* GetTexture() const = 0;

};
