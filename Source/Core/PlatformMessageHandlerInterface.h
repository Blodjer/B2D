#pragma once

class GenericWindow;

class IPlatformMessageHandlerInterface
{
public:
    virtual bool OnKeyEvent(GenericWindow* window, int32 scancode, EKey key, EKeyEvent event) { return false; }
    virtual bool OnKeyChar(GenericWindow* window, uint32 codepoint) { return false; }

    virtual bool OnMouseButton(GenericWindow* window, EMouseButton button, EMouseButtonEvent event) { return false; }
    virtual bool OnMouseMove(GenericWindow* window, TVec2 position) { return false; }
    virtual bool OnMouseEnter(GenericWindow* window) { return false; }
    virtual bool OnMouseLeave(GenericWindow* window) { return false; }
};