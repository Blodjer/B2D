#pragma once

#include "Core/Core.h"

struct TVec2
{
public:
	TVec2::TVec2() { }

	TVec2::TVec2(float f) : X(f), Y(f) { }

	TVec2::TVec2(float x, float y) : X(x), Y(y) { }

	TVec2::TVec2(int32 x, int32 y) : X(static_cast<float>(x)), Y(static_cast<float>(y)) { }

	float X;
	float Y;

	static const TVec2 Zero;
	static const TVec2 One;

public:
	FORCEINLINE TVec2 TVec2::operator+(const TVec2& v) const
	{
		return TVec2(X + v.X, Y + v.Y);
	}

	FORCEINLINE TVec2 TVec2::operator-(const TVec2& v) const
	{
		return TVec2(X - v.X, Y - v.Y);
	}

	FORCEINLINE TVec2 TVec2::operator-() const
	{
		return TVec2(-X, -Y);
	}

	FORCEINLINE TVec2 TVec2::operator*(const float f) const
	{
		return TVec2(X * f, Y * f);
	}

	FORCEINLINE TVec2& TVec2::operator*=(const float f)
	{
		X *= f;
		Y *= f;
		return *this;
	}

	FORCEINLINE TVec2 TVec2::operator*(const TVec2& v) const
	{
		return TVec2(X * v.X, Y * v.Y);
	}

	FORCEINLINE bool TVec2::operator==(const TVec2& v) const
	{
		return X == v.X && Y == v.Y;
	}

	FORCEINLINE bool TVec2::operator!=(const TVec2 & v) const
	{
		return X != v.X || Y != v.Y;
	}
};
