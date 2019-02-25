#pragma once

#include "Core/Core.h"

#include <cmath>

class CGameEngine;

class UMath
{
	friend CGameEngine;

public:
	template<typename T>
	FORCEINLINE static T Min(T a, T b) { return std::min(a, b); }

	template<typename T>
	FORCEINLINE static T Max(T a, T b) { return std::max(a, b); }

	template<typename T>
	FORCEINLINE static T Clamp(T value, T min, T max) { return std::clamp(value, min, max); }

	template<typename T>
	FORCEINLINE static T Clamp01(T value) { return Clamp<T>(value, 0, 1); }

	FORCEINLINE static float Sin(float value) { return std::sin(value); }
	FORCEINLINE static float Cos(float value) { return std::cos(value); }
	FORCEINLINE static float Tan(float value) { return std::tan(value); }

	template<typename T>
	FORCEINLINE static T RandomRange(T min, T max) { return static_cast<T>(min + (max - min) * (rand() / static_cast<float>(RAND_MAX))); }

	template<typename T>
	FORCEINLINE static float Lerp(T a, T b, T alpha) { return (static_cast<T>(1) - alpha) * a + alpha * b; }

private:
	UMath() = default;

	FORCEINLINE static void RandomInit(unsigned int seed) { srand(seed); }
};