#pragma once

#define B2D

#define INLINE inline
#define FORCEINLINE __forceinline

#ifdef _DEBUG
#define B2D_DEBUG
#include "Debug/Debug.h"
#else
#define B2D_RELEASE
#endif

#include "Math/BMath.h"

#include "Core/Types/BasicTypes.h"
#include "Core/Types/Vector2.h"
#include "Core/Types/Delegate.h"
