#pragma once
#include "Math/BMath.h"

class World;
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

    template<class C>
    C& Sibling() const
    {
        // TODO: enforce to always have an Transform component which can be returned immediately 
        if constexpr (C::MASK == 1 && TransformComponent::MASK == 1)
        {
            return *static_cast<C* const>(*owner->mComponents._Myfirst());
        }

        constexpr size_t INVERTED_MASK = C::MASK - 1;

        size_t index = INVERTED_MASK & owner->mComponentMask;
        index = CountBits(index);

        Component* const c = owner->mComponents[index];
        return *static_cast<C* const>(c);
    }

    FORCEINLINE static _NODISCARD size_t CountBits(size_t mask) noexcept
    {	// count number of set bits
        constexpr char const* const _Bitsperbyte =
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

        unsigned char const* _Ptr = &reinterpret_cast<unsigned char const&>(mask);
        unsigned char const* const _End = _Ptr + sizeof(mask);

        size_t _Val = 0;
        for (; _Ptr != _End; ++_Ptr)
            _Val += _Bitsperbyte[*_Ptr];

        return (_Val);
    }
};
