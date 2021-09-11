#include "B2D_pch.h"
#include "Mesh.h"

#include "GameEngine.h"
#include "GHI/GraphicsHardwareInterface.h"
#include "GHI/GHIBuffer.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include <tinyobjloader/tiny_obj_loader.h>

namespace std
{
    template<>
    struct std::hash<Mesh::Vertex>
    {
        size_t operator()(Mesh::Vertex const& vertex) const
        {
            return // TODO
                ((std::hash<float>()(vertex.position.x + vertex.position.y + vertex.position.z) ^
                (std::hash<float>()(vertex.normal.x + vertex.normal.y + vertex.normal.z) << 1)) >> 1) ^
                (std::hash<float>()(vertex.uv.x + vertex.uv.y) << 1);
        }
    };
}

bool Mesh::Load(ResourcePath const& path)
{
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn, err;

    if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, path.c_str()))
    {
        B2D_LOG_ERROR("{}", err);
        return false;
    }

    if (!warn.empty())
    {
        B2D_LOG_WARNING("{}", warn); // TODO: Trim message \n
    }

    if (shapes.empty())
    {
        return false;
    }

    std::unordered_map<Vertex, decltype(m_indices)::value_type> uniqueVertices;
    for (auto const& shape : shapes)
    {
        // Loop over faces (polygon)
        uint index_offset = 0;
        for (uint f = 0; f < shape.mesh.num_face_vertices.size(); f++)
        {
            // Hardcode loading to triangles
            uint fv = 3;

            // Loop over vertices in the face.
            for (uint v = 0; v < fv; v++)
            {
                tinyobj::index_t idx = shape.mesh.indices[index_offset + v];

                // position
                tinyobj::real_t vx = attrib.vertices[3 * idx.vertex_index + 0];
                tinyobj::real_t vy = attrib.vertices[3 * idx.vertex_index + 1];
                tinyobj::real_t vz = attrib.vertices[3 * idx.vertex_index + 2];
               
                Mesh::Vertex newVertex;
                newVertex.position.x = vx;
                newVertex.position.y = vy;
                newVertex.position.z = vz;

                // normal
                tinyobj::real_t nx = attrib.normals[3 * idx.normal_index + 0];
                tinyobj::real_t ny = attrib.normals[3 * idx.normal_index + 1];
                tinyobj::real_t nz = attrib.normals[3 * idx.normal_index + 2];

                newVertex.normal.x = nx;
                newVertex.normal.y = ny;
                newVertex.normal.z = nz;

                // uv
                tinyobj::real_t ux = idx.texcoord_index < 0 ? 0 : attrib.texcoords[2 * idx.texcoord_index + 0];
                tinyobj::real_t uy = idx.texcoord_index < 0 ? 0 : attrib.texcoords[2 * idx.texcoord_index + 1];

                newVertex.uv.x = ux;
                newVertex.uv.y = uy;

                auto [it, isNew] = uniqueVertices.emplace(newVertex, static_cast<uint32>(m_vertices.size()));
                if (isNew)
                {
                    m_vertices.push_back(newVertex);
                }
                m_indices.push_back(it->second);
            }
            index_offset += fv;
        }
    }

    uint size = m_vertices.size() * sizeof(Vertex);
    m_vertexBuffer = GameEngine::Instance()->GetGHI()->CreateBuffer(EGHIBufferType::VertexBuffer, size);
    m_vertexBuffer->Upload(m_vertices.data(), size);

    uint indicesSize = m_indices.size() * sizeof(decltype(m_indices)::value_type);
    m_indexBuffer = GameEngine::Instance()->GetGHI()->CreateBuffer(EGHIBufferType::IndexBuffer, indicesSize);
    m_indexBuffer->Upload(m_indices.data(), indicesSize);

    return true;
}

void Mesh::Free()
{
    // TODO
}
