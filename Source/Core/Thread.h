#pragma once

#include <thread>

class Thread
{
protected:
    Thread();

public:
    template<class T>
    static Thread* Create()
    {
        Thread* t = new T();
        if (t->Init())
        {
            return nullptr;
        }

        t->mThread = std::thread(t->Run());

        return t;
    }

    virtual bool Init() = 0;
    virtual void Run() = 0;
    virtual void Stop() = 0;

private:
    std::thread mThread;

};

