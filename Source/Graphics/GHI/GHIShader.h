#pragma once

class GHIShader
{
protected:
    GHIShader() = default;

public:
    virtual ~GHIShader() = default;

private:
    GHIShader(GHIShader const&) = delete;
    GHIShader& operator=(GHIShader const&) = delete;
};