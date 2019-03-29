#pragma once

class CWindow;

class IPlatformApplicationInterface
{
public:
    virtual void Init() = 0;
    virtual void PollEvents() = 0;
    virtual void Shutdown() = 0;

    virtual CWindow* MakeWindow(uint32 width, uint32 height, std::string const& title) = 0;
    virtual void DestroyWindow(CWindow* window) = 0;
};