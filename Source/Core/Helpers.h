#pragma once
#include "Core/Log.h"

#ifdef B2D_PLATFORM_WINDOWS
#define HANDLE_LAST_ERROR() ::_HandleError(STACKLOCATION_INPUT);
namespace
{
    void _HandleError(STACKLOCATION_SIGNATURE)
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
            B2D_LOG_ERROR("Error: {}\n" STACKLOCATION_LOG_FORMAT, messageBuffer, STACKLOCATION_PARAMS);
        }
        else
        {
            B2D_LOG_ERROR("Unknown Error\n" STACKLOCATION_LOG_FORMAT, STACKLOCATION_PARAMS);
        }
    }
}
#else
#define HANDLE_LAST_ERROR() B2D_COMPILE_ERROR("Cannot use HANDLE_LAST_ERROR() in non Windows platform!");
#endif