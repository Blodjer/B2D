#pragma once
#include "Core/Resource.h"

class GHIBuffer;

class Mesh : public IResource
{
protected:
	bool Load(ResourcePath const& path) override;
	void Free() override;

public:
    static constexpr auto GetFallbackResourcePath() { return ""; }

public:
    struct Vertex
    {
        TVec3 position;
        TVec3 normal;
        TVec2 uv;

        bool operator==(Vertex const& other) const
        {
            return position == other.position && normal == other.normal && uv == other.uv;
        }
    };

    std::vector<Vertex> const& GetVertices() const { return m_vertices; }
    std::vector<uint32> const& GetIndices() const { return m_indices; }

    GHIBuffer const* GetVertexBuffer() const { return m_vertexBuffer; }
    GHIBuffer const* GetIndexBuffer() const { return m_indexBuffer; }    

private:
    std::vector<Vertex> m_vertices;
    std::vector<uint32> m_indices;

    GHIBuffer* m_vertexBuffer = nullptr;
    GHIBuffer* m_indexBuffer = nullptr;
};
