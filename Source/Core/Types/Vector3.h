#pragma once

#include "Core/Core.h"

struct TVec3
{
public:
    TVec3::TVec3() { }

    TVec3::TVec3(float f) : X(f), Y(f), Z(f) { }

    TVec3::TVec3(float x, float y) : X(x), Y(y), Z(0) { }

    TVec3::TVec3(float x, float y, float z) : X(x), Y(y), Z(z) { }

    float X;
    float Y;
    float Z;

    static const TVec3 Zero;
    static const TVec3 One;

    static const TVec3 Up;

public:
    FORCEINLINE TVec3 TVec3::operator+(const TVec3& v) const
    {
        return TVec3(X + v.X, Y + v.Y, Z + v.Z);
    }

    FORCEINLINE TVec3& TVec3::operator+=(const TVec3& v)
    {
        X += v.X;
        Y += v.Y;
        Z += v.Z;
        return *this;
    }

    FORCEINLINE TVec3 TVec3::operator-(const TVec3& v) const
    {
        return TVec3(X - v.X, Y - v.Y, Z - v.Z);
    }

    FORCEINLINE TVec3 TVec3::operator-() const
    {
        return TVec3(-X, -Y, -Z);
    }

    FORCEINLINE TVec3 TVec3::operator*(const float f) const
    {
        return TVec3(X * f, Y * f, Z * f);
    }

    FORCEINLINE TVec3& TVec3::operator*=(const float f)
    {
        X *= f;
        Y *= f;
        Z *= f;
        return *this;
    }

    FORCEINLINE TVec3 TVec3::operator*(const TVec3& v) const
    {
        return TVec3(X * v.X, Y * v.Y, Z * v.Z);
    }

    FORCEINLINE bool TVec3::operator==(const TVec3& v) const
    {
        return X == v.X && Y == v.Y && Z == v.Z;
    }

    FORCEINLINE bool TVec3::operator!=(const TVec3 & v) const
    {
        return X != v.X || Y != v.Y || Z != v.Z;
    }
};
