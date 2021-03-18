#pragma once

#include "Core/Core.h"

#include <cmath>

class GameEngine;

class UMath final
{
	friend GameEngine;

public:
	template<typename T>
	FORCEINLINE static T Min(T a, T b) { return std::min(a, b); }

	template<typename T>
	FORCEINLINE static T Max(T a, T b) { return std::max(a, b); }

	template<typename T>
	FORCEINLINE static T Clamp(T value, T min, T max) { return std::clamp(value, min, max); }

	template<typename T>
	FORCEINLINE static T Clamp01(T value) { return Clamp<T>(value, 0, 1); }

    template<typename T>
    FORCEINLINE static T Floor(T value) { return std::floor(value); }

    FORCEINLINE static int32 FloorToInt(float value) { return static_cast<int32>(Floor<float>(value)); }
    FORCEINLINE static int64 FloorToInt(double value) { return static_cast<int64>(Floor<double>(value)); }

    template<typename T>
    FORCEINLINE static T Ceil(T value) { return std::ceil(value); }

    FORCEINLINE static int32 CeilToInt(float value) { return static_cast<int32>(Ceil<float>(value)); }
    FORCEINLINE static int64 CeilToInt(double value) { return static_cast<int64>(Ceil<double>(value)); }

    template<typename T>
    FORCEINLINE static T Round(T value) { return std::round(value); }

    FORCEINLINE static uint32 RoundToInt(float value) { return static_cast<int32>(Round<float>(value)); }
    FORCEINLINE static uint64 RoundToInt(double value) { return static_cast<int64>(Round<double>(value)); }

	FORCEINLINE static float Sin(float value) { return std::sin(value); }
	FORCEINLINE static float Cos(float value) { return std::cos(value); }
	FORCEINLINE static float Tan(float value) { return std::tan(value); }

    FORCEINLINE static float Random() { return static_cast<float>(rand() / static_cast<float>(RAND_MAX)); }

	template<typename T>
	FORCEINLINE static T RandomRange(T min, T max) { return static_cast<T>(min + (max - min) * Random()); }

	template<typename T>
	FORCEINLINE static float Lerp(T a, T b, T alpha) { return (static_cast<T>(1) - alpha) * a + alpha * b; }

    template<typename T>
    FORCEINLINE static float Pow(T base, T exp) { return std::pow(base, exp); }

    template<typename T>
    static constexpr T Pow2(T exp) { return (exp == 0) ? 1 : 2 * Pow2<T>(exp - 1); }

private:
	UMath() = default;

	FORCEINLINE static void RandomInit(unsigned int seed) { srand(seed); }
};