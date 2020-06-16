#pragma once
#include "Math/BMath.h"

class Entity;

#define DECLARE_COMPONENT(name, id) \
public: \
    static constexpr uint16 MASK = UMath::Pow2(id); \
public: \
    virtual uint16 GET_MASK() const override { return MASK; } \
public:

struct Component
{
public:
    Component() = default;
    virtual ~Component() = default;

public:
    virtual uint16 GET_MASK() const = 0;

public:
    Entity const* owner;
};

FORCEINLINE static size_t CountBits(size_t mask) noexcept
{
    constexpr char const* const bitsperbyte =
        "\0\1\1\2\1\2\2\3\1\2\2\3\2\3\3\4"
        "\1\2\2\3\2\3\3\4\2\3\3\4\3\4\4\5"
        "\1\2\2\3\2\3\3\4\2\3\3\4\3\4\4\5"
        "\2\3\3\4\3\4\4\5\3\4\4\5\4\5\5\6"
        "\1\2\2\3\2\3\3\4\2\3\3\4\3\4\4\5"
        "\2\3\3\4\3\4\4\5\3\4\4\5\4\5\5\6"
        "\2\3\3\4\3\4\4\5\3\4\4\5\4\5\5\6"
        "\3\4\4\5\4\5\5\6\4\5\5\6\5\6\6\7"
        "\1\2\2\3\2\3\3\4\2\3\3\4\3\4\4\5"
        "\2\3\3\4\3\4\4\5\3\4\4\5\4\5\5\6"
        "\2\3\3\4\3\4\4\5\3\4\4\5\4\5\5\6"
        "\3\4\4\5\4\5\5\6\4\5\5\6\5\6\6\7"
        "\2\3\3\4\3\4\4\5\3\4\4\5\4\5\5\6"
        "\3\4\4\5\4\5\5\6\4\5\5\6\5\6\6\7"
        "\3\4\4\5\4\5\5\6\4\5\5\6\5\6\6\7"
        "\4\5\5\6\5\6\6\7\5\6\6\7\6\7\7\x8";

    unsigned char const* ptr = &reinterpret_cast<unsigned char const&>(mask);
    unsigned char const* const end = ptr + sizeof(mask);

    size_t val = 0;
    for (; ptr != end; ++ptr)
        val += bitsperbyte[*ptr];

    return (val);
}
