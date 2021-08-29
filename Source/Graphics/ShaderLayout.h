#pragma once

enum class EShaderType
{
    Vertex,
    Pixel,
    Compute,
    Geometry
};

struct ShaderLayout
{
    struct PushConstantBuffers
    {
        uint32 offset;
        uint size;
    };

    struct UniformBuffer
    {
        uint32 set;
        uint32 binding;
    };

    std::vector<UniformBuffer> uniformBuffers;
    std::vector<PushConstantBuffers> pushConstantBuffers;
};