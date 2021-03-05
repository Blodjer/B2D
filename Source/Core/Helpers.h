#pragma once
#include "Core/Log.h"

#ifdef B2D_PLATFORM_WINDOWS
#define HANDLE_LAST_ERROR() ::_HandleError(STACK_LOCATION_INPUT);
namespace
{
    void _HandleError(STACK_LOCATION_SIGNATURE)
    {
        DWORD const error = GetLastError();
        if (error == 0)
        {
            return;
        }

        SetLastError(0);

        LPSTR messageBuffer = nullptr;
        size_t const size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS | FORMAT_MESSAGE_MAX_WIDTH_MASK, nullptr, error, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&messageBuffer, 0, nullptr);

        std::string const message(messageBuffer, size);
        LocalFree(messageBuffer);

        if (!message.empty())
        {
            B2D_ERROR("Error: {}\n" STACK_LOCATION_LOG_FORMAT, messageBuffer, STACK_LOCATION_PARAMS);
        }
        else
        {
            B2D_ERROR("Unknown Error\n" STACK_LOCATION_LOG_FORMAT, STACK_LOCATION_PARAMS);
        }
    }
}
#else
#define HANDLE_LAST_ERROR() B2D_STATIC_ASSERT(false, "Cannot use HANDLE_LAST_ERROR() in non Windows platform!");
#endif