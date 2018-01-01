#pragma once

struct Vector2
{
public:
	Vector2();
	Vector2(float f);
	Vector2(float x, float y);

	float X;
	float Y;

	static const Vector2 Zero;
	static const Vector2 One;

public:
	Vector2 Vector2::operator+(const Vector2& v) const;
	Vector2 Vector2::operator-(const Vector2& v) const;
	Vector2 Vector2::operator-() const;
	Vector2 Vector2::operator*(float f) const;
	Vector2 Vector2::operator*=(float f) const;
	Vector2 Vector2::operator*(const Vector2& v) const;
	bool Vector2::operator==(const Vector2& v) const;
	bool Vector2::operator!=(const Vector2& v) const;
};

__forceinline Vector2::Vector2() { }

__forceinline Vector2::Vector2(float f) : X(f), Y(f) { }

__forceinline Vector2::Vector2(float x, float y) : X(x), Y(y) { }

__forceinline Vector2 Vector2::operator+(const Vector2& v) const
{
	return Vector2(X + v.X, Y + v.Y);
}

__forceinline Vector2 Vector2::operator-(const Vector2& v) const
{
	return Vector2(X - v.X, Y - v.Y);
}

__forceinline Vector2 Vector2::operator-() const
{
	return Vector2(-X, -Y);
}

__forceinline Vector2 Vector2::operator*(float f) const
{
	return Vector2(X * f, Y * f);
}

__forceinline Vector2 Vector2::operator*=(float f) const
{
	return Vector2(X * f, Y * f);
}

__forceinline Vector2 Vector2::operator*(const Vector2& v) const
{
	return Vector2(X * v.X, Y * v.Y);
}

__forceinline bool Vector2::operator==(const Vector2& v) const
{
	return X == v.X && Y == v.Y;
}

__forceinline bool Vector2::operator!=(const Vector2 & v) const
{
	return X != v.X || Y != v.Y;
}
