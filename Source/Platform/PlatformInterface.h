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

    virtual GenericWindow* CreateOffscreenRenderContext() = 0;

    virtual void AddMessageHandler(IPlatformMessageHandlerInterface* messageHandler) = 0;
    virtual void RemoveMessageHandler(IPlatformMessageHandlerInterface* messageHandler) = 0;

protected:
    friend class EditorModule;
    virtual bool ImGui_Init() { B2D_LOG_ERROR("ImGui not implemented on this platform!"); return false; }
    virtual void ImGui_Shutdow() { }
    virtual void ImGui_BeginFrame() { }
};