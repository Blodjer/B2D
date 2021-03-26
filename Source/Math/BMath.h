#pragma once

#include "Core/CoreAssert.h"

#include <cmath>

class GameEngine;

class UMath final
{
	friend GameEngine;

public:
	template<typename T>
	FORCEINLINE static constexpr T Min(T a, T b) { return std::min(a, b); }
    template<typename T>
    FORCEINLINE static constexpr T Min(T a) { return Min<T>(a, static_cast<T>(0)); }

	template<typename T>
	FORCEINLINE static constexpr T Max(T a, T b) { return std::max(a, b); }
    template<typename T>
    FORCEINLINE static constexpr T Max(T a) { return Max<T>(a, static_cast<T>(0)); }

	template<typename T>
	FORCEINLINE static constexpr T Clamp(T value, T min, T max) { return std::clamp(value, min, max); }
	template<typename T>
	FORCEINLINE static constexpr T Clamp01(T value) { return Clamp<T>(value, 0, 1); }

    template<typename T>
    FORCEINLINE static T Floor(T value) { return std::floor(value); }
    FORCEINLINE static int32 FloorToInt(float value) { return static_cast<int32>(Floor<float>(value)); }
    FORCEINLINE static int64 FloorToInt(double value) { return static_cast<int64>(Floor<double>(value)); }
    FORCEINLINE static uint32 FloorToUInt(float value) { B2D_ASSERT(value >= 0.0f); return static_cast<uint32>(Floor<float>(value)); }
    FORCEINLINE static uint64 FloorToUInt(double value) { B2D_ASSERT(value >= 0.0f); return static_cast<uint64>(Floor<double>(value)); }

    template<typename T>
    FORCEINLINE static T Ceil(T value) { return std::ceil(value); }
    FORCEINLINE static int32 CeilToInt(float value) { return static_cast<int32>(Ceil<float>(value)); }
    FORCEINLINE static int64 CeilToInt(double value) { return static_cast<int64>(Ceil<double>(value)); }
    FORCEINLINE static uint32 CeilToUInt(float value) { B2D_ASSERT(value >= 0.0f); return static_cast<uint32>(Ceil<float>(value)); }
    FORCEINLINE static uint64 CeilToUInt(double value) { B2D_ASSERT(value >= 0.0f); return static_cast<uint64>(Ceil<double>(value)); }

    template<typename T>
    FORCEINLINE static T Round(T value) { return std::round(value); }
    FORCEINLINE static int32 RoundToInt(float value) { return static_cast<int32>(Round<float>(value)); }
    FORCEINLINE static int64 RoundToInt(double value) { return static_cast<int64>(Round<double>(value)); }
    FORCEINLINE static uint32 RoundToUInt(float value) { B2D_ASSERT(value >= 0.0f); return static_cast<uint32>(Round<float>(value)); }
    FORCEINLINE static uint64 RoundToUInt(double value) { B2D_ASSERT(value >= 0.0f); return static_cast<uint64>(Round<double>(value)); }

    template<typename T>
    FORCEINLINE static T Sin(T value) { return static_cast<T>(std::sin(value)); }
    template<typename T>
    FORCEINLINE static T ArcSin(T value) { return static_cast<T>(std::asin(value)); }
    template<typename T>
    FORCEINLINE static T Cos(T value) { return static_cast<T>(std::cos(value)); }
    template<typename T>
    FORCEINLINE static T ArcCos(T value) { return static_cast<T>(std::acos(value)); }
    template<typename T>
    FORCEINLINE static T Tan(T value) { return static_cast<T>(std::tan(value)); }
    template<typename T>
    FORCEINLINE static T ArcTan(T value) { return static_cast<T>(std::atan(value)); }

    FORCEINLINE static float Random() { return static_cast<float>(rand() / static_cast<float>(RAND_MAX)); }
	template<typename T>
	FORCEINLINE static T RandomRange(T min, T max) { return static_cast<T>(min + (max - min) * Random()); }

	template<typename T>
	FORCEINLINE static float Lerp(T a, T b, T alpha) { return (static_cast<T>(1) - alpha) * a + alpha * b; }

    template<typename T>
    FORCEINLINE static float Pow(T base, T exp) { return std::pow(base, exp); }
    template<typename T>
    static constexpr T Pow2(T exp) { return (exp == 0) ? 1 : 2 * Pow2<T>(exp - 1); }

    static constexpr float PI = 3.1415926535897932f;
    static constexpr float DegToRad(float deg) { return deg * 0.01745329251f; };
    static constexpr float RadToDeg(float rad) { return rad * 57.2957795131f; };

private:
	UMath() = default;

	FORCEINLINE static void RandomInit(unsigned int seed) { srand(seed); }
};