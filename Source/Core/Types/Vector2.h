#pragma once

#include "Core/Core.h"

struct TVec2
{
public:
	TVec2() { }

	TVec2(float f) : x(f), y(f) { }

    TVec2(float x, float y) : x(x), y(y) { }

    TVec2(double x, double y) : x(static_cast<float>(x)), y(static_cast<float>(y)) { }

    TVec2(int32 x, int32 y) : x(static_cast<float>(x)), y(static_cast<float>(y)) { }

	float x;
	float y;

	static const TVec2 Zero;
	static const TVec2 One;

public:
	FORCEINLINE TVec2 operator+(const TVec2& v) const
	{
		return TVec2(x + v.x, y + v.y);
	}

	FORCEINLINE TVec2 operator-(const TVec2& v) const
	{
		return TVec2(x - v.x, y - v.y);
	}

	FORCEINLINE TVec2 operator-() const
	{
		return TVec2(-x, -y);
	}

	FORCEINLINE TVec2 operator*(const float f) const
	{
		return TVec2(x * f, y * f);
	}

	FORCEINLINE TVec2& operator*=(const float f)
	{
		x *= f;
		y *= f;
		return *this;
	}

	FORCEINLINE TVec2 operator*(const TVec2& v) const
	{
		return TVec2(x * v.x, y * v.y);
	}

	FORCEINLINE bool operator==(const TVec2& v) const
	{
		return x == v.x && y == v.y;
	}

	FORCEINLINE bool operator!=(const TVec2 & v) const
	{
		return x != v.x || y != v.y;
	}
};
