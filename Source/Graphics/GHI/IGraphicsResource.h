#pragma once

class IGraphicsResource
{
protected:
    IGraphicsResource() = default;
    ~IGraphicsResource() = default;

private:
    IGraphicsResource(IGraphicsResource const&) = delete;
    IGraphicsResource& operator=(IGraphicsResource const&) = delete;
};