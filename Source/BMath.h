#pragma once

#include <math.h>

class BMath
{
public:
	__forceinline static float Sin(float value) { return sin(value); };
	__forceinline static float Cos(float value) { return cos(value); };
	__forceinline static float Tan(float value) { return tan(value); };

	__forceinline static void RandomInit(int seed) { srand(seed); }
	__forceinline static float RandomRange(float min, float max) { return min + (max - min) * (rand() / (float)RAND_MAX); };

private:
	BMath() { };
};