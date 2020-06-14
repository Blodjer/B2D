#pragma once

#include "Core/Core.h"

struct TVec3
{
public:
    TVec3::TVec3() { }

    TVec3::TVec3(float f) : x(f), y(f), z(f) { }

    TVec3::TVec3(float x, float y, float z) : x(x), y(y), z(z) { }

    float x;
    float y;
    float z;

    static const TVec3 Zero;
    static const TVec3 One;

    static const TVec3 Up;

public:
    FORCEINLINE TVec3 TVec3::operator+(const TVec3& v) const
    {
        return TVec3(x + v.x, y + v.y, z + v.z);
    }

    FORCEINLINE TVec3& TVec3::operator+=(const TVec3& v)
    {
        x += v.x;
        y += v.y;
        z += v.z;
        return *this;
    }

    FORCEINLINE TVec3 TVec3::operator-(const TVec3& v) const
    {
        return TVec3(x - v.x, y - v.y, z - v.z);
    }

    FORCEINLINE TVec3 TVec3::operator-() const
    {
        return TVec3(-x, -y, -z);
    }

    FORCEINLINE TVec3 TVec3::operator*(const float f) const
    {
        return TVec3(x * f, y * f, z * f);
    }

    FORCEINLINE TVec3& TVec3::operator*=(const float f)
    {
        x *= f;
        y *= f;
        z *= f;
        return *this;
    }

    FORCEINLINE TVec3 TVec3::operator*(const TVec3& v) const
    {
        return TVec3(x * v.x, y * v.y, z * v.z);
    }

    FORCEINLINE bool TVec3::operator==(const TVec3& v) const
    {
        return x == v.x && y == v.y && z == v.z;
    }

    FORCEINLINE bool TVec3::operator!=(const TVec3 & v) const
    {
        return x != v.x || y != v.y || z != v.z;
    }
};
