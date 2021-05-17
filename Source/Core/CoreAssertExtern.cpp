#include "B2D_pch.h"

#include "CoreAssertExtern.h"

void B2D_Extern::Assert(STACKLOCATION_SIGNATURE, char const* message)
{
    Log::Assert(STACKLOCATION_PARAMS, message, Log::ESink::External, 1);
}