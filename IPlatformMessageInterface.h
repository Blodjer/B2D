#pragma once

class IPlatformMessageInterface
{
public:
    virtual bool OnKeyEvent() { return false; }
    virtual bool OnCursorEvent() { return false; }
};