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
    };

    std::vector<Vertex> const& GetVertices() const { return m_vertices; }
    GHIBuffer const* GetVertexBuffer() const { return m_vertexBuffer; }

private:
    std::vector<Vertex> m_vertices;

    GHIBuffer* m_vertexBuffer = nullptr;
};
