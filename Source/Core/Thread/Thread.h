#pragma once

#include <thread>

class Runnable;

class IThread
{
protected:
    IThread() = default;

    virtual void Run() = 0;

protected:
    Runnable* m_runnable = nullptr;
};

class Thread : IThread
{
public:
    static Thread* Create(Runnable* runnable)
    {
        Thread* newThread = new Thread();
        newThread->m_runnable = runnable;
        newThread->m_thread = std::thread(&Thread::Run, newThread);

        return newThread;
    }

protected:
    virtual void Run() override;

private:
    std::thread m_thread;
    Runnable* m_runnable = nullptr;

};
