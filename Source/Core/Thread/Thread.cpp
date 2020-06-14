#include "B2D_pch.h"
#include "Thread.h"

#include "Runnable.h"

void Thread::Run()
{
    if (!m_runnable->Init())
    {
        return;
    }

    m_runnable->Run();

    m_runnable->Shutdown();
}
