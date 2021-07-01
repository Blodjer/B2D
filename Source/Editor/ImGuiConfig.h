#pragma once

// This file is used by the preprocessor and included by ImGui
// Includes need to be relative to this file

#ifndef B2D

#include "../Core/CoreAssertExtern.h"
#define IM_ASSERT(_expr) B2D_ASSERT_EXTERN(_expr)

#else

#define IM_ASSERT(_expr) B2D_ASSERT_s(_expr, Log::ESink::External)

#endif