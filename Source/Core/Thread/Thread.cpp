#include "B2D_pch.h"
#include "Thread.h"

#include "Runnable.h"

void Thread::Run()
{
    if (!mRunnable->Init())
    {
        return;
    }

    mRunnable->Run();

    mRunnable->Shutdown();
}
