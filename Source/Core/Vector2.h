#pragma once

struct SVector2
{
public:
	SVector2::SVector2() { }

	SVector2::SVector2(float f) : X(f), Y(f) { }

	SVector2::SVector2(float x, float y) : X(x), Y(y) { }

	float X;
	float Y;

	static const SVector2 Zero;
	static const SVector2 One;

public:
	__forceinline SVector2 SVector2::operator+(const SVector2& v) const
	{
		return SVector2(X + v.X, Y + v.Y);
	}

	__forceinline SVector2 SVector2::operator-(const SVector2& v) const
	{
		return SVector2(X - v.X, Y - v.Y);
	}

	__forceinline SVector2 SVector2::operator-() const
	{
		return SVector2(-X, -Y);
	}

	__forceinline SVector2 SVector2::operator*(const float f) const
	{
		return SVector2(X * f, Y * f);
	}

	__forceinline SVector2& SVector2::operator*=(const float f)
	{
		X *= f;
		Y *= f;
		return *this;
	}

	__forceinline SVector2 SVector2::operator*(const SVector2& v) const
	{
		return SVector2(X * v.X, Y * v.Y);
	}

	__forceinline bool SVector2::operator==(const SVector2& v) const
	{
		return X == v.X && Y == v.Y;
	}

	__forceinline bool SVector2::operator!=(const SVector2 & v) const
	{
		return X != v.X || Y != v.Y;
	}
};
