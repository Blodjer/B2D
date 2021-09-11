#include "B2D_pch.h"
#include "Mesh.h"

#include "GameEngine.h"
#include "GHI/GraphicsHardwareInterface.h"
#include "GHI/GHIBuffer.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include <tinyobjloader/tiny_obj_loader.h>

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

                m_vertices.push_back(newVertex);
            }
            index_offset += fv;
        }
    }

//     m_vertices.resize(3);
// 
//     m_vertices[0].position = { 1.f, 1.f, 0.0f };
//     m_vertices[1].position = { -1.f, 1.f, 0.0f };
//     m_vertices[2].position = { 0.f, -1.f, 0.0f };
// 
//     m_vertices[0].normal = { 1.f, 0.f, 0.0f };
//     m_vertices[1].normal = { 0.f, 1.f, 0.0f };
//     m_vertices[2].normal = { 0.f, 0.f, 1.0f };

    uint size = m_vertices.size() * sizeof(Vertex);
    m_vertexBuffer = GameEngine::Instance()->GetGHI()->CreateBuffer(EGHIBufferType::VertexBuffer, size);
    m_vertexBuffer->Upload(m_vertices.data(), size);

    return true;
}

void Mesh::Free()
{
    // TODO
}
