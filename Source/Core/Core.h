#pragma once

#include "Debug/Debug.h"

#include "BasicTypes.h"
#include "Vector2.h"

#define B2D

#define FORCEINLINE __forceinline

#ifdef _DEBUG
#define B2D_DEBUG
#include "Debug/Debug.h"
#else
#define B2D_RELEASE
#endif
