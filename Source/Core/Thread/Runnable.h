#pragma once

class Runnable
{
    friend class Thread;

protected:
    Runnable() = default;
    virtual ~Runnable() = default;

protected:
    virtual bool Init() = 0;
    virtual void Run() = 0;
    virtual void Shutdown() = 0;

};

