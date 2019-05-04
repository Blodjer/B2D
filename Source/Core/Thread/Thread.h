#pragma once

#include <thread>

class Runnable;

class IThread
{
protected:
    IThread() = default;

    virtual void Run() = 0;

protected:
    Runnable* mRunnable = nullptr;
};

class Thread : IThread
{
public:
    static Thread* Create(Runnable* runnable)
    {
        Thread* newThread = new Thread();
        newThread->mRunnable = runnable;
        newThread->mThread = std::thread(&Thread::Run, newThread);

        return newThread;
    }

protected:
    virtual void Run() override;

private:
    std::thread mThread;
    Runnable* mRunnable = nullptr;

};
