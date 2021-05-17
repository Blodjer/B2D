#pragma once

#ifndef B2D

// This file is used by the preprocessor and included by ImGui
// Includes need to be relative to this file

#include "../Core/CoreAssertExtern.h"
#define IM_ASSERT(_expr) B2D_ASSERT_EXTERN(_expr)

#endif