#pragma once

class GHICommandList
{
public:
    virtual void Begin() {};
    virtual void End() {};

    virtual void DrawTest() = 0;
};
