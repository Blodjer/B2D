#pragma once

#include "Core/Core.h"

#include <math.h>
#include <algorithm>

class BMath
{
public:
	FORCEINLINE static float Min(float fA, float fB) { return std::min(fA, fB); }
	FORCEINLINE static float Max(float fA, float fB) { return std::max(fA, fB); }
	FORCEINLINE static float Clamp(float fValue, float fMin, float fMax) { return std::clamp(fValue, fMin, fMax); }
	FORCEINLINE static float Clamp01(float fValue) { return Clamp(fValue, 0.0f, 1.0f); }

	FORCEINLINE static float Sin(float value) { return sin(value); }
	FORCEINLINE static float Cos(float value) { return cos(value); }
	FORCEINLINE static float Tan(float value) { return tan(value); }

	FORCEINLINE static void RandomInit(int seed) { srand(seed); }
	FORCEINLINE static float RandomRange(float min, float max) { return min + (max - min) * (rand() / static_cast<float>(RAND_MAX)); }

private:
	BMath() = default;
};