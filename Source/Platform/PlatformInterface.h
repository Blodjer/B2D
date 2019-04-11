#pragma once

class GenericWindow;
class IGraphicsHardwareInterface;
class IPlatformMessageHandlerInterface;

class IPlatformApplicationInterface
{
public:
    virtual bool Init() = 0;
    virtual void PollEvents() = 0;
    virtual void Shutdown() = 0;

    virtual IGraphicsHardwareInterface* CreateGHI() const = 0;

    virtual GenericWindow* MakeWindow(uint32 width, uint32 height, std::string const& title) = 0;
    virtual void DestroyWindow(GenericWindow* window) = 0;

    virtual void AddMessageHandler(IPlatformMessageHandlerInterface* messageHandler) = 0;
    virtual void RemoveMessageHandler(IPlatformMessageHandlerInterface* messageHandler) = 0;
};