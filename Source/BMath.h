#pragma once

#include <math.h>
#include <algorithm>

class BMath
{
public:
	__forceinline static float Min(float fA, float fB) { return std::min(fA, fB); }
	__forceinline static float Max(float fA, float fB) { return std::max(fA, fB); }
	__forceinline static float Clamp(float fValue, float fMin, float fMax) { return std::clamp(fValue, fMin, fMax); }
	__forceinline static float Clamp01(float fValue) { return Clamp(fValue, 0.0f, 1.0f); }

	__forceinline static float Sin(float value) { return sin(value); }
	__forceinline static float Cos(float value) { return cos(value); }
	__forceinline static float Tan(float value) { return tan(value); }

	__forceinline static void RandomInit(int seed) { srand(seed); }
	__forceinline static float RandomRange(float min, float max) { return min + (max - min) * (rand() / (float)RAND_MAX); }

private:
	BMath() = default;
};