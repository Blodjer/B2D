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
