#pragma once

#include "B2D_pch.h"

#define B2D

#define INLINE inline
#define FORCEINLINE __forceinline

#ifdef _DEBUG
#define B2D_DEBUG
#else
#define B2D_RELEASE
#endif

#include "Math/BMath.h"

#include "Core/Types/BasicTypes.h"
#include "Core/Types/Vector2.h"
#include "Core/Types/Vector3.h"
#include "Core/Types/Matrix.h"
#include "Core/Types/Delegate.h"
