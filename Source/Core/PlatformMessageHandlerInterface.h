#pragma once

class IPlatformMessageHandlerInterface
{
public:
    virtual bool OnKeyEvent(EKey key, EKeyEvent event) { return false; }
};