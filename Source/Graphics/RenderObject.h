#pragma once

#include "Core/Types/Matrix.h"
#include "Graphics/Material.h"

template<class C>
class RenderObjectBuffer
{
public:
    RenderObjectBuffer(uint32 capacity)
    {
        m_buffer = static_cast<C*>(std::malloc(sizeof(C) * capacity));
        B2D_ASSERT(m_buffer != nullptr);

        m_capacity = capacity;
    }

    ~RenderObjectBuffer()
    {
        free(m_buffer);
    }

    RenderObjectBuffer(RenderObjectBuffer const&) = delete;
    void operator=(RenderObjectBuffer const&) = delete;

public:
    FORCEINLINE C& operator[](uint32 const i) const
    {
        B2D_ASSERT(i < m_capacity);
        return m_buffer[i];
    }

    template<typename... V>
    FORCEINLINE void Add(V&... values)
    {
        if (B2D_CHECK_f(m_size >= m_capacity, "Cannot add RenderObject. RenderObjectBuffer has reached it's maximum capacity of {} elements", m_capacity))
        {
            return;
        }

        m_buffer[m_size] = C(values...);
        m_size++;
    }

    FORCEINLINE void Clear() { m_size = 0; }
    FORCEINLINE uint32 const Size() const { return m_size; }

private:
    C* m_buffer = nullptr;
    uint32 m_capacity = 0;
    uint32 m_size = 0;
};

class QuadRenderObject
{
public:
    QuadRenderObject(TMatrix const& matrix, Material* material)
        : m_matrix(matrix)
        , m_material(material)
    {
    }

    TMatrix m_matrix;
    Material* m_material;
};
